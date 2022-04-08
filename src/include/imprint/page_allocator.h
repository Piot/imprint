#ifndef IMPRINT_PAGE_ALLOCATOR_H
#define IMPRINT_PAGE_ALLOCATOR_H

#include <stdint.h>
#include <stddef.h>

typedef uint64_t ImprintPageIdList;

typedef struct ImprintPageResult {
    ImprintPageIdList pageIds;
    void* memory;
} ImprintPageResult;

typedef struct ImprintPageAllocator {
    uint64_t freePages;
    uint8_t* basePointerForPages;
    size_t pageCount;
    size_t pageSizeInOctets;
} ImprintPageAllocator;

void imprintPageAllocatorInit(ImprintPageAllocator* self, size_t pageCount);
void imprintPageAllocatorDestroy(ImprintPageAllocator* self);
void imprintPageAllocatorAlloc(ImprintPageAllocator* self, size_t pageCount, ImprintPageResult* result);
void imprintPageAllocatorFree(ImprintPageAllocator* self, ImprintPageIdList pageId);
void imprintPageAllocatorFreeSeparate(ImprintPageAllocator* self, ImprintPageIdList pageIds);

#endif
