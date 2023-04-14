/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <imprint/tagged_allocator.h>
#include <imprint/tagged_page_allocator.h>
#include <imprint/utils.h>
#include <tiny-libc/tiny_libc.h>

#define IMPRINT_TAGGED_ALLOCATOR_DETAILED_LOG (0)

static inline void prepareMemory(ImprintTaggedAllocator* self, size_t size)
{
    size_t currentUsedSize = (uintptr_t) (self->linear.next - self->linear.memory);

    if (currentUsedSize + size <= self->linear.size) {
        return;
    }
    ImprintPageResult result;
    size_t neededPageCount = (size / self->cachedPageSize) + 1;
    imprintTaggedPageAllocatorAlloc(self->taggedPageAllocator, self->tag, neededPageCount, &result);
    size_t receivedMemorySize = neededPageCount * self->cachedPageSize;
    imprintLinearAllocatorInit(&self->linear, result.memory, receivedMemorySize, "tagged allocator");
}

static void* imprintTaggedAllocatorAllocDebug(void* self_, size_t size, const char* sourceFile, size_t line,
                                              const char* description)
{

    if (size == 0) {
        return 0;
    }

    ImprintTaggedAllocator* self = (ImprintTaggedAllocator*) self_;

#if IMPRINT_TAGGED_ALLOCATOR_DETAILED_LOG
    char buf[32];
    char buf1[32];
    CLOG_VERBOSE(">>>> tag allocate %016X %s (%s)", self->tag, imprintSizeToString(buf1, 32, size),
                 imprintSizeAndPercentageToString(buf, 32, (uintptr_t) (self->linear.next - self->linear.memory),
                                                  self->linear.size));
#endif

    prepareMemory(self, size);

    void* memory = imprintLinearAllocatorAllocDebug(&self->linear, size, sourceFile, line, description);

    tc_memset_octets(memory, 0xfa, size);

    return memory;
}

#if !CONFIGURATION_DEBUG

static void* imprintTaggedAllocatorAlloc(void* self_, size_t size)
{

    if (size == 0) {
        return 0;
    }

    ImprintTaggedAllocator* self = (ImprintTaggedAllocator*) self_;

    CLOG_EXECUTE(char buf[32];)
    CLOG_EXECUTE(char buf1[32];)
    CLOG_VERBOSE(">>>> allocate %s (%s)", imprintSizeToString(buf1, 32, size),
                 imprintSizeAndPercentageToString(buf, 32, (uintptr_t) (self->linear.next - self->linear.memory),
                                                  self->linear.size));

    prepareMemory(self, size);

    void* memory = imprintLinearAllocatorAlloc(&self->linear, size);

    // tc_memset_octets(memory, 0xfa, size);

    return memory;
}
#endif


#if CONFIGURATION_DEBUG
static void* imprintTaggedAllocatorCallocDebug(void* self_, size_t size, const char* sourceFile, size_t line,
                                               const char* description)
{
    if (size == 0) {
        return 0;
    }
    ImprintTaggedAllocator* self = (ImprintTaggedAllocator*) self_;

    void* memory = imprintTaggedAllocatorAllocDebug(self, size, sourceFile, line, description);
    tc_mem_clear(memory, size);

    return memory;
}
#endif

#if !CONFIGURATION_DEBUG
static void* imprintTaggedAllocatorCalloc(void* self_, size_t size)
{
    if (size == 0) {
        return 0;
    }
    ImprintTaggedAllocator* self = (ImprintTaggedAllocator*) self_;

    void* memory = imprintTaggedAllocatorAlloc(self, size);
    tc_mem_clear(memory, size);

    return memory;
}
#endif

void imprintTaggedAllocatorInit(ImprintTaggedAllocator* self, ImprintTaggedPageAllocator* taggedPageAllocator,
                                uint64_t tag)
{
    self->taggedPageAllocator = taggedPageAllocator;
    self->tag = tag;
    self->linear.memory = 0;
    self->linear.next = 0;
    self->linear.size = 0;
    self->cachedPageSize = taggedPageAllocator->pageAllocator->pageSizeInOctets;
#if CONFIGURATION_DEBUG
    self->info.allocDebugFn = imprintTaggedAllocatorAllocDebug;
    self->info.callocDebugFn = imprintTaggedAllocatorCallocDebug;
#else
    self->info.allocFn = imprintTaggedAllocatorAlloc;
    self->info.callocFn = imprintTaggedAllocatorCalloc;
#endif
}

void imprintTaggedAllocatorFreeAll(ImprintTaggedAllocator* self)
{
    imprintTaggedPageAllocatorFree(self->taggedPageAllocator, self->tag);
    self->tag = 0;
    self->linear.memory = 0;
    self->linear.next = 0;
    self->linear.size = 0;
}

void imprintTaggedAllocatorClearAll(ImprintTaggedAllocator* self)
{
    imprintTaggedPageAllocatorFree(self->taggedPageAllocator, self->tag);
    self->linear.memory = 0;
    self->linear.next = 0;
    self->linear.size = 0;
}
