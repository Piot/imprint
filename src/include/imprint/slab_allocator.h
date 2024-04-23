/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/imprint
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef IMPRINT_SLAB_ALLOCATOR_H
#define IMPRINT_SLAB_ALLOCATOR_H

#include <imprint/allocator.h>
#include <imprint/slab_cache.h>

#define IMPRINT_SLAB_CACHE_MAX_COUNT (8)

typedef struct ImprintSlabAllocator {
    ImprintAllocatorWithFree info;
    ImprintSlabCache caches[IMPRINT_SLAB_CACHE_MAX_COUNT];
    size_t cacheCount;
    size_t maxCapacity;
} ImprintSlabAllocator;

void imprintSlabAllocatorInit(ImprintSlabAllocator* self);

void imprintSlabAllocatorAdd(ImprintSlabAllocator* self, ImprintAllocator* allocator, size_t powerOfTwo,
                             size_t arraySize, const char* debug);

void imprintSlabAllocatorDebugOutput(const ImprintSlabAllocator* self);

#endif
