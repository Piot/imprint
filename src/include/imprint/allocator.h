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


#if 1
#define IMPRINT_ALLOC(allocatorInfo, size, description)                          \
  (allocatorInfo)->allocDebugFn(allocatorInfo, size, __FILE__, __LINE__, description)
#define IMPRINT_CALLOC(allocatorInfo, size, description)                         \
  (allocatorInfo)->callocDebugFn(allocatorInfo, size, __FILE__, __LINE__, description)

#define IMPRINT_FREE(allocatorInfoWithFree, ptr)                         \
  (allocatorInfoWithFree)->freeDebugFn(allocatorInfoWithFree, (void*)(ptr), __FILE__, __LINE__, "")


#define IMPRINT_CALLOC_TYPE_COUNT(allocatorInfo, type, count)                    \
  (type *)IMPRINT_CALLOC(allocatorInfo, count * sizeof(type), #type)
#define IMPRINT_CALLOC_TYPE(allocatorInfo, type)                                 \
  (type *)IMPRINT_CALLOC(allocatorInfo, sizeof(type), #type)
#define IMPRINT_ALLOC_TYPE(allocatorInfo, type)                                  \
  (type *)IMPRINT_ALLOC(allocatorInfo, sizeof(type), #type)
#define IMPRINT_ALLOC_TYPE_COUNT(allocatorInfo, type, count)                     \
  (type *)IMPRINT_ALLOC(allocatorInfo, count * sizeof(type), #type)
#else
#define IMPRINT_ALLOC(allocatorInfo, size, description)                          \
  malloc(size)
#define IMPRINT_CALLOC(allocatorInfo, size, description)                         \
  calloc(1, size)

#define IMPRINT_CALLOC_TYPE_COUNT(allocatorInfo, type, count)                    \
  (type *)IMPRINT_CALLOC(allocatorInfo, count * sizeof(type), #type);
#define IMPRINT_CALLOC_TYPE(allocatorInfo, type)                                 \
  (type *)IMPRINT_CALLOC(allocatorInfo, sizeof(type), #type);
#define IMPRINT_ALLOC_TYPE(allocatorInfo, type)                                  \
  (type *)IMPRINT_ALLOC(allocatorInfo, sizeof(type), #type);
#define IMPRINT_ALLOC_TYPE_COUNT(allocatorInfo, type, count)                     \
  (type *)IMPRINT_ALLOC(allocatorInfo, count * sizeof(type), #type);
#endif


#endif
