#include <clog/clog.h>
#include <imprint/slab_allocator.h>

static void *imprintSlabAllocatorAlloc(void *self_, size_t size,
                                       const char *sourceFile, size_t line,
                                       const char *description) {
  ImprintSlabAllocator *self = (ImprintSlabAllocator *)self_;
  for (size_t i = 0; i < self->cacheCount; ++i) {
    ImprintSlabCache *cache = &self->caches[i];
    if (size <= cache->structSize) {
      return imprintSlabCacheAlloc(cache, size, sourceFile, line, description);
    }
  }

  CLOG_ERROR("unsupported size %zu", size);
}

static void imprintSlabAllocatorFree(void *self_, void *ptr,
                                     const char *sourceFile, size_t line,
                                     const char *description) {
  ImprintSlabAllocator *self = (ImprintSlabAllocator *)self_;
  for (size_t i = 0; i < self->cacheCount; ++i) {
    ImprintSlabCache *cache = &self->caches[i];
    bool worked = imprintSlabCacheTryToFree(cache, ptr);
    if (worked) {
      return;
    }
  }

  CLOG_ERROR("illegal free")
}

void imprintSlabAllocatorAdd(ImprintSlabAllocator *self, ImprintAllocator *allocator, size_t powerOfTwo, size_t arraySize, const char* debug)
{
    if (self->cacheCount >= self->maxCapacity) {
        CLOG_ERROR("could not add")
    }
    imprintSlabCacheInit(&self->caches[self->cacheCount++], allocator, powerOfTwo, arraySize,
                         debug);
}
void imprintSlabAllocatorInit(ImprintSlabAllocator *self,
                              ImprintAllocator *allocator, size_t powerOfTwo,
                              size_t cacheCount, size_t arraySize, const char *debug) {
    self->maxCapacity = 4;
  if (cacheCount > self->maxCapacity) {
    CLOG_ERROR("not supported")
  }

  for (size_t i = 0; i < cacheCount; ++i) {
    imprintSlabCacheInit(&self->caches[i], allocator, powerOfTwo + i, arraySize,
                         debug);
  }
  self->cacheCount = cacheCount;

  self->info.allocator.allocDebugFn = imprintSlabAllocatorAlloc;
  self->info.allocator.callocDebugFn = 0;
  self->info.freeDebugFn = imprintSlabAllocatorFree;
}
