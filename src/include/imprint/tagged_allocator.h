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


#endif
