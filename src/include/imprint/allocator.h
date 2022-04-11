#ifndef IMPRINT_ALLOCATOR_H
#define IMPRINT_ALLOCATOR_H

#include <stddef.h>

typedef void* (*ImprintAllocDebugFn)(void* self_, size_t size, const char *sourceFile, size_t lineNumber,
                                     const char *description);

typedef void* (*ImprintCallocDebugFn)(void* self_, size_t size, const char *sourceFile, size_t lineNumber,
                                      const char *description);

typedef struct ImprintAllocator {
  ImprintAllocDebugFn allocDebugFn;
  ImprintCallocDebugFn callocDebugFn;
} ImprintAllocator;


typedef void (*ImprintFreeDebugFn)(void* self, void* ptr, const char *sourceFile, size_t lineNumber,
                                      const char *description);

typedef struct ImprintAllocatorWithFree {
  ImprintAllocator allocator;
  ImprintFreeDebugFn freeDebugFn;
} ImprintAllocatorWithFree;

#endif
