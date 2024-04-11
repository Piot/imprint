/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/imprint
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef IMPRINT_TAGGED_PAGE_ALLOCATOR_H
#define IMPRINT_TAGGED_PAGE_ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>

#include <imprint/page_allocator.h>

typedef struct ImprintTaggedPageEntry {
    ImprintPageIdList pageIds;
    uint64_t tag;
} ImprintTaggedPageEntry;

typedef struct ImprintTaggedPageAllocator {
    ImprintPageAllocator* pageAllocator;
    ImprintTaggedPageEntry entries[32];
    size_t entryCapacity;
} ImprintTaggedPageAllocator;

void imprintTaggedPageAllocatorInit(ImprintTaggedPageAllocator* self, ImprintPageAllocator* pageAllocator);
void imprintTaggedPageAllocatorDestroy(ImprintTaggedPageAllocator* self);
void imprintTaggedPageAllocatorAlloc(ImprintTaggedPageAllocator* self, uint64_t tag, size_t pageCount,
                                     ImprintPageResult* result);
void imprintTaggedPageAllocatorFree(ImprintTaggedPageAllocator* self, uint64_t tag);

#endif
