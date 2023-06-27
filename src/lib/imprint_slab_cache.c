/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <imprint/allocator.h>
#include <imprint/slab_cache.h>
#include <imprint/utils.h>

void imprintSlabCacheInit(ImprintSlabCache* self, ImprintAllocator* allocator, size_t powerOfTwo, size_t capacity,
                          const char* debug)
{
    size_t octetCount = 1 << powerOfTwo;
    self->structSize = octetCount;
    self->capacity = capacity;
    self->allocatedCount = 0;
    self->totalSize = self->structSize * self->capacity;
    self->debug = debug;
    self->structAlign = 8;
    if (self->structSize < self->structAlign) {
        self->structAlign = self->structSize;
    }

    self->memory = IMPRINT_ALLOC(allocator, self->totalSize, debug);
    self->entries = IMPRINT_CALLOC_TYPE_COUNT(allocator, ImprintSlabCacheEntry, self->capacity);

    self->firstFreeEntry = &self->entries[0];

    for (size_t i = 0; i < self->capacity; ++i) {
        ImprintSlabCacheEntry* e = &self->entries[i];
        e->isAllocated = false;
        e->line = (size_t) -1;
        e->file = 0;
        e->nextFreeEntry = 0;
        e->debugIndex = i;
        if (i > 0) {
            self->entries[i - 1].nextFreeEntry = &self->entries[i];
        }
    }
}

static inline void* imprintSlabCacheAllocInternal(ImprintSlabCache* self, size_t size, ImprintSlabCacheEntry** outEntry)
{
    CLOG_ASSERT(self->allocatedCount < self->capacity, "Out of memory in self (%zu out of %zu) for size %zu. %s",
                self->allocatedCount, self->capacity, self->structSize, self->debug)

    if (self->firstFreeEntry == 0) {
        CLOG_INFO("First free is not available")
        return 0;
    }
    ImprintSlabCacheEntry* e = self->firstFreeEntry;
    CLOG_ASSERT(e, "first free is null")
    CLOG_ASSERT(!e->isAllocated, "entry already allocated")

    size_t foundIndex = (size_t)(e - self->entries);

    if (e->debugIndex != foundIndex) {
        CLOG_ERROR("internal error looking up foundIndex")
    }

    uint8_t* m = (uint8_t*) self->memory + (foundIndex * self->structSize);
    *outEntry = e;
    self->firstFreeEntry = e->nextFreeEntry;
    self->allocatedCount++;
    e->isAllocated = true;
    e->allocatedPointer = m;
    e->usedOctetSize = size;

    return m;
}

void* imprintSlabCacheAlloc(ImprintSlabCache* self, size_t size)
{
    ImprintSlabCacheEntry* e;
    void* m = imprintSlabCacheAllocInternal(self, size, &e);
    return m;
}

void* imprintSlabCacheAllocDebug(ImprintSlabCache* self, size_t size, const char* file, size_t line, const char* debug)
{
    CLOG_ASSERT(self->allocatedCount < self->capacity, "Out of memory in self (%zd out of %zd) for size %zd. %s %s:%zd",
                self->allocatedCount, self->capacity, self->structSize, self->debug, file, line)
    ImprintSlabCacheEntry* e;
    void* m = imprintSlabCacheAllocInternal(self, size, &e);
    e->line = line;
    e->file = file;
    e->description = debug;

#if defined CLOG_LOG_ENABLED
    char buf[32];
    char buf1[32];
    CLOG_VERBOSE(">>>> slab: allocate index %zu %s (%s %zu/%zu)", e->debugIndex,
                 imprintSizeToString(buf1, 32, e->usedOctetSize), imprintSizeToString(buf, 32, self->structSize),
                 self->allocatedCount, self->capacity)
#endif
    return m;
}

static inline int findIndexFromAllocation(const ImprintSlabCache* cache, const void* p)
{

    if ((uintptr_t) p % cache->structAlign != 0) {
        return -1;
    }

    return (int)(((uintptr_t) p - (uintptr_t) cache->memory) / cache->structSize);
}

static inline ImprintSlabCacheEntry* findEntryFromAllocation(const ImprintSlabCache* self, const void* p)
{
    int index = findIndexFromAllocation(self, p);
    if (index >= (int)self->capacity || index < 0) {
        return 0;
    }

    ImprintSlabCacheEntry* e = &self->entries[index];
    if (e->allocatedPointer != p) {
        CLOG_ERROR("allocated pointer differs")
    }
    return e;
}

static inline void freeEntry(ImprintSlabCache* self, ImprintSlabCacheEntry* e)
{
    if (!e->isAllocated) {
        CLOG_ERROR("can not free this")
    }
    e->isAllocated = false;
    self->allocatedCount--;

    e->nextFreeEntry = self->firstFreeEntry;
    self->firstFreeEntry = e;
}

bool imprintSlabCacheTryToFree(ImprintSlabCache* self, void* ptr)
{
    ImprintSlabCacheEntry* foundEntry = findEntryFromAllocation(self, ptr);
    if (foundEntry == 0) {
        return false;
    }

    if (!foundEntry->isAllocated) {
        CLOG_ERROR("multiple free")
        return false;
    }
#if defined CLOG_LOG_ENABLED

    char buf[32];
    char buf1[32];
    CLOG_VERBOSE(">>>> slab: release index %zu free:%s (%s %zu/%zu)", foundEntry->debugIndex,
                 imprintSizeToString(buf1, 32, foundEntry->usedOctetSize),
                 imprintSizeToString(buf, 32, self->structSize), self->allocatedCount, self->capacity)
#endif

    freeEntry(self, foundEntry);

    return true;
}
