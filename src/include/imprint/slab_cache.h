/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/imprint
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef IMPRINT_SLAB_CACHE_H
#define IMPRINT_SLAB_CACHE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct ImprintAllocator;

typedef struct ImprintSlabCacheEntryDebug {
    const char* file;
    int line;
    const char* description;
} ImprintSlabCacheEntryDebug;

typedef struct ImprintSlabCacheEntry {
    bool isAllocated;
    struct ImprintSlabCacheEntry* nextFreeEntry;
    const char* file;
    size_t line;
    const char* description;
    void* allocatedPointer;
    size_t debugIndex;
    size_t usedOctetSize;
} ImprintSlabCacheEntry;

typedef struct ImprintSlabCache {
    uint8_t* memory;
    size_t structSize;
    size_t structAlign;
    size_t totalSize;
    size_t capacity;
    size_t allocatedCount;
    ImprintSlabCacheEntry* entries;
    ImprintSlabCacheEntry* firstFreeEntry;
    const char* debug;
} ImprintSlabCache;

void imprintSlabCacheInit(ImprintSlabCache* self, struct ImprintAllocator* allocator, size_t powerOfTwo, size_t capacity,
                          const char* debug);

void* imprintSlabCacheAllocDebug(ImprintSlabCache* self, size_t octetCount, const char* file, size_t line,
                                 const char* debug);
void* imprintSlabCacheAlloc(ImprintSlabCache* self, size_t size);
bool imprintSlabCacheTryToFree(ImprintSlabCache* self, void* ptr);

#endif
