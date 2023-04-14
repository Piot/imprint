/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef IMPRINT_ALLOCATOR_H
#define IMPRINT_ALLOCATOR_H

#include <stddef.h>
#include <tiny-libc/tiny_libc.h>

typedef void* (*ImprintAllocDebugFn)(void* self_, size_t size, const char* sourceFile, size_t lineNumber,
                                     const char* description);

typedef void* (*ImprintCallocDebugFn)(void* self_, size_t size, const char* sourceFile, size_t lineNumber,
                                      const char* description);

typedef void* (*ImprintAllocFn)(void* self_, size_t size);
typedef void* (*ImprintCallocFn)(void* self_, size_t size);

typedef struct ImprintAllocator {
    ImprintAllocDebugFn allocDebugFn;
    ImprintCallocDebugFn callocDebugFn;
    ImprintAllocFn allocFn;
    ImprintCallocFn callocFn;
} ImprintAllocator;

typedef void (*ImprintFreeDebugFn)(void* self, void* ptr, const char* sourceFile, size_t lineNumber,
                                   const char* description);

typedef void (*ImprintFreeFn)(void* self, void* ptr);

typedef struct ImprintAllocatorWithFree {
    ImprintAllocator allocator;
    ImprintFreeDebugFn freeDebugFn;
    ImprintFreeFn freeFn;
} ImprintAllocatorWithFree;

#if CONFIGURATION_DEBUG
#define IMPRINT_ALLOC(allocatorInfo, size, description)                                                                \
    (allocatorInfo)->allocDebugFn(allocatorInfo, size, __FILE__, __LINE__, description)
#define IMPRINT_CALLOC(allocatorInfo, size, description) \
    (allocatorInfo)->callocDebugFn(allocatorInfo, size, __FILE__, __LINE__, description)
#define IMPRINT_FREE(allocatorInfoWithFree, ptr)                                                                       \
    (allocatorInfoWithFree)->freeDebugFn(allocatorInfoWithFree, (void*) (ptr), __FILE__, __LINE__, "")
#else
#define IMPRINT_ALLOC(allocatorInfo, size, description) (allocatorInfo)->allocFn(allocatorInfo, size)
#define IMPRINT_CALLOC(allocatorInfo, size, description) (allocatorInfo)->callocFn(allocatorInfo, size)
#define IMPRINT_FREE(allocatorInfoWithFree, ptr) (allocatorInfoWithFree)->freeFn(allocatorInfoWithFree, (void*) (ptr))
#endif



#define IMPRINT_CALLOC_TYPE_COUNT(allocatorInfo, type, count)                                                          \
    (type*) IMPRINT_CALLOC(allocatorInfo, count * sizeof(type), #type)
#define IMPRINT_CALLOC_TYPE(allocatorInfo, type) (type*) IMPRINT_CALLOC(allocatorInfo, sizeof(type), #type)
#define IMPRINT_ALLOC_TYPE(allocatorInfo, type) (type*) IMPRINT_ALLOC(allocatorInfo, sizeof(type), #type)
#define IMPRINT_ALLOC_TYPE_COUNT(allocatorInfo, type, count)                                                           \
    (type*) IMPRINT_ALLOC(allocatorInfo, count * sizeof(type), #type)


static inline TC_FORCE_INLINE char* imprintStrDup(ImprintAllocator* allocator, const char* str)
{
  size_t len = tc_strlen(str);
  char* buf = IMPRINT_ALLOC_TYPE_COUNT(allocator, char, len+1);
  tc_memcpy_octets(buf, str, len+1);

  return buf;
}

#endif
