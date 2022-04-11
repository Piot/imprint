#ifndef IMPRINT_TAGGED_ALLOCATOR_H
#define IMPRINT_TAGGED_ALLOCATOR_H

#include <imprint/linear_allocator.h>
#include <imprint/allocator.h>

#include <stddef.h>

struct ImprintTaggedPageAllocator;

typedef struct ImprintTaggedAllocator {
  ImprintAllocator info; // MUST BE FIRST
    struct ImprintTaggedPageAllocator* taggedPageAllocator;
    ImprintLinearAllocator linear;
    size_t cachedPageSize;
    uint64_t tag;
} ImprintTaggedAllocator;

void imprintTaggedAllocatorInit(ImprintTaggedAllocator* self, struct ImprintTaggedPageAllocator* taggedPageAllocator, uint64_t tag);

//void* imprintTaggedAllocatorAllocDebug(ImprintTaggedAllocator* self, size_t size, const char *sourceFile, int line,
  //                                     const char *description);

//void* imprintTaggedAllocatorCallocDebug(ImprintTaggedAllocator* self, size_t size,  const char *sourceFile, int line,
  //                                     const char *description);


void imprintTaggedAllocatorFreeAll(ImprintTaggedAllocator* self);
void imprintTaggedAllocatorClearAll(ImprintTaggedAllocator* self);

#if 1
#define IMPRINT_ALLOC(allocatorInfo, size, description)                          \
  allocatorInfo->allocDebugFn(allocatorInfo, size, __FILE__, __LINE__, description)
#define IMPRINT_CALLOC(allocatorInfo, size, description)                         \
  allocatorInfo->callocDebugFn(allocatorInfo, size, __FILE__, __LINE__, description)

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
