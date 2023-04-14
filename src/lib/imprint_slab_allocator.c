/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <imprint/slab_allocator.h>

static inline void* imprintSlabAllocatorAllocDebug(void* self_, size_t size, const char* sourceFile, size_t line,
                                                   const char* description)
{
    ImprintSlabAllocator* self = (ImprintSlabAllocator*) self_;
    for (size_t i = 0; i < self->cacheCount; ++i) {
        ImprintSlabCache* cache = &self->caches[i];
        if (size <= cache->structSize) {
            return imprintSlabCacheAllocDebug(cache, size, sourceFile, line, description);
        }
    }

    CLOG_ERROR("unsupported size %zu", size);
}

#if !CONFIGURATION_DEBUG
static inline void* imprintSlabAllocatorAlloc(void* self_, size_t size)
{
    ImprintSlabAllocator* self = (ImprintSlabAllocator*) self_;
    for (size_t i = 0; i < self->cacheCount; ++i) {
        ImprintSlabCache* cache = &self->caches[i];
        if (size <= cache->structSize) {
            return imprintSlabCacheAlloc(cache, size);
        }
    }

    CLOG_ERROR("unsupported size %zu", size);
}
#endif

#if CONFIGURATION_DEBUG

static void* imprintSlabAllocatorCallocDebug(void* self_, size_t size, const char* sourceFile, size_t line,
                                               const char* description)
{
    if (size == 0) {
        return 0;
    }
    ImprintSlabAllocator* self = (ImprintSlabAllocator*) self_;

    void* memory = imprintSlabAllocatorAllocDebug(self, size, sourceFile, line, description);
    tc_mem_clear(memory, size);

    return memory;
}
#endif

#if !CONFIGURATION_DEBUG
static void* imprintSlabAllocatorCalloc(void* self_, size_t size)
{
    if (size == 0) {
        return 0;
    }
    ImprintSlabAllocator* self = (ImprintSlabAllocator*) self_;

    void* memory = imprintSlabAllocatorAlloc(self, size);
    tc_mem_clear(memory, size);

    return memory;
}
#endif


#if CONFIGURATION_DEBUG

static void imprintSlabAllocatorFreeDebug(void* self_, void* ptr, const char* sourceFile, size_t line,
                                          const char* description)
{
    ImprintSlabAllocator* self = (ImprintSlabAllocator*) self_;
    for (size_t i = 0; i < self->cacheCount; ++i) {
        ImprintSlabCache* cache = &self->caches[i];
        bool worked = imprintSlabCacheTryToFree(cache, ptr);
        if (worked) {
            return;
        }
    }

    CLOG_ERROR("illegal free")
}
#endif

#if !CONFIGURATION_DEBUG
static inline void imprintSlabAllocatorFree(void* self_, void* ptr)
{
    ImprintSlabAllocator* self = (ImprintSlabAllocator*) self_;
    for (size_t i = 0; i < self->cacheCount; ++i) {
        ImprintSlabCache* cache = &self->caches[i];
        bool worked = imprintSlabCacheTryToFree(cache, ptr);
        if (worked) {
            return;
        }
    }

    CLOG_ERROR("illegal free")
}
#endif


void imprintSlabAllocatorAdd(ImprintSlabAllocator* self, ImprintAllocator* allocator, size_t powerOfTwo,
                             size_t arraySize, const char* debug)
{
    if (self->cacheCount >= self->maxCapacity) {
        CLOG_ERROR("imprintSlabAllocatorAdd: could not add")
    }
    imprintSlabCacheInit(&self->caches[self->cacheCount++], allocator, powerOfTwo, arraySize, debug);
}
void imprintSlabAllocatorInit(ImprintSlabAllocator* self, ImprintAllocator* allocator, size_t powerOfTwo,
                              size_t cacheCount, size_t arraySize, const char* debug)
{
    self->maxCapacity = 4;
    if (cacheCount > self->maxCapacity) {
        CLOG_ERROR("slab allocator: error cache count greater than max capacity. cacheCount:%zu maxCapacity:%zu", cacheCount, self->maxCapacity)
    }

    for (size_t i = 0; i < cacheCount; ++i) {
        imprintSlabCacheInit(&self->caches[i], allocator, powerOfTwo + i, arraySize, debug);
    }
    self->cacheCount = cacheCount;

#if CONFIGURATION_DEBUG
    self->info.allocator.allocDebugFn = imprintSlabAllocatorAllocDebug;
    self->info.allocator.callocDebugFn = imprintSlabAllocatorCallocDebug;
    self->info.freeDebugFn = imprintSlabAllocatorFreeDebug;
#else
    self->info.allocator.allocFn = imprintSlabAllocatorAlloc;
    self->info.allocator.callocFn = imprintSlabAllocatorCalloc;
    self->info.freeFn = imprintSlabAllocatorFree;
#endif
}
