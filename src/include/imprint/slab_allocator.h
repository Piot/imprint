/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef IMPRINT_SLAB_ALLOCATOR_H
#define IMPRINT_SLAB_ALLOCATOR_H

#include <imprint/allocator.h>
#include <imprint/slab_cache.h>

typedef struct ImprintSlabAllocator {
    ImprintAllocatorWithFree info;
    ImprintSlabCache caches[4];
    size_t cacheCount;
    size_t maxCapacity;
} ImprintSlabAllocator;

void imprintSlabAllocatorInit(ImprintSlabAllocator* self, ImprintAllocator* allocator, size_t powerOfTwo,
                              size_t capacity, size_t arraySize, const char* debug);

void imprintSlabAllocatorAdd(ImprintSlabAllocator* self, ImprintAllocator* allocator, size_t powerOfTwo,
                             size_t arraySize, const char* debug);

#endif
