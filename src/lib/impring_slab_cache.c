#include <clog/clog.h>
#include <imprint/allocator.h>
#include <imprint/slab_cache.h>
#include <imprint/tagged_allocator.h>

void imprintSlabCacheInit(ImprintSlabCache *self, ImprintAllocator *allocator,
                          size_t powerOfTwo, size_t capacity,
                          const char *debug) {
  size_t octetCount = 1 << powerOfTwo;
  self->structSize = octetCount;
  self->capacity = capacity;
  self->allocatedCount = 0;
  self->totalSize = self->structSize * self->capacity;
  self->debug = debug;

  self->memory = IMPRINT_ALLOC(allocator, self->totalSize, debug);
  self->entries = IMPRINT_CALLOC_TYPE_COUNT(allocator, ImprintSlabCacheEntry,
                                            self->capacity);

  self->firstFreeEntry = &self->entries[0];

  for (size_t i = 0; i < self->capacity; ++i) {
    ImprintSlabCacheEntry *e = &self->entries[i];
    e->isAllocated = false;
    e->line = -1;
    e->file = 0;
    e->nextFreeEntry = 0;

    if (i > 0) {
      self->entries[i - 1].nextFreeEntry = &self->entries[i];
    }
  }
}

void *imprintSlabCacheAlloc(ImprintSlabCache *self, size_t size,
                            const char *file, size_t line, const char *debug) {
  CLOG_ASSERT(self->allocatedCount < self->capacity,
              "Out of memory in self (%zu) %s %s:%d", self->allocatedCount,
              self->debug, file, line)

  if (self->firstFreeEntry == 0) {
    CLOG_INFO("First free is not available")
    return 0;
  }
  ImprintSlabCacheEntry *e = self->firstFreeEntry;
  CLOG_ASSERT(e, "first free is null")

  size_t foundIndex = (e - self->entries) / self->structSize;
  self->firstFreeEntry = e->nextFreeEntry;
  self->allocatedCount++;
  e->isAllocated = true;

  uint8_t *m = (uint8_t *)self->memory + (foundIndex * self->structSize);

  return m;
}

static inline int indexFromAllocation(const ImprintSlabCache *cache,
                                      const void *p) {

  return ((const uint8_t *)p - cache->memory) / cache->structSize;
}

static inline ImprintSlabCacheEntry *
entryFromAllocation(const ImprintSlabCache *self, const void *p) {
  int index = indexFromAllocation(self, p);
  if (index >= self->capacity || index < 0) {
    CLOG_ERROR("illegal free pointer in slab cache")
  }

  return &self->entries[index];
}

static inline ImprintSlabCacheEntry *
findEntryFromAllocation(const ImprintSlabCache *self, const void *p) {
  int index = indexFromAllocation(self, p);
  if (index >= self->capacity || index < 0) {
    return 0;
  }

  return &self->entries[index];
}

static inline void freeEntry(ImprintSlabCache *self, ImprintSlabCacheEntry *e) {
  e->isAllocated = false;
  self->allocatedCount--;

  e->nextFreeEntry = self->firstFreeEntry;
  self->firstFreeEntry = e;
}

bool imprintSlabCacheTryToFree(ImprintSlabCache *self, void *ptr) {
  ImprintSlabCacheEntry *foundEntry = findEntryFromAllocation(self, ptr);
  if (foundEntry == 0) {
    return false;
  }

  freeEntry(self, foundEntry);

  return true;
}
