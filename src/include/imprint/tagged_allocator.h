/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/imprint
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef IMPRINT_TAGGED_ALLOCATOR_H
#define IMPRINT_TAGGED_ALLOCATOR_H

#include <imprint/allocator.h>
#include <imprint/linear_allocator.h>

#include <stddef.h>

struct ImprintTaggedPageAllocator;

typedef struct ImprintTaggedAllocator {
    ImprintAllocator info; // MUST BE FIRST
    struct ImprintTaggedPageAllocator* taggedPageAllocator;
    const char* debug;
    ImprintLinearAllocator linear;
    size_t cachedPageSize;
    uint64_t tag;
} ImprintTaggedAllocator;

void imprintTaggedAllocatorInit(ImprintTaggedAllocator* self,
    struct ImprintTaggedPageAllocator* taggedPageAllocator, uint64_t tag, const char* debug);

void imprintTaggedAllocatorFreeAll(ImprintTaggedAllocator* self);
void imprintTaggedAllocatorClearAll(ImprintTaggedAllocator* self);

#endif
