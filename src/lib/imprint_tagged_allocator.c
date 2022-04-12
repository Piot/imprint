#include <clog/clog.h>
#include <imprint/tagged_allocator.h>
#include <imprint/tagged_page_allocator.h>
#include <tiny-libc/tiny_libc.h>

static inline void prepareMemory(ImprintTaggedAllocator *self, size_t size) {
  size_t currentUsedSize = (uintptr_t)(self->linear.next - self->linear.memory);

  if (currentUsedSize + size <= self->linear.size) {
    return;
  }
  ImprintPageResult result;
  size_t neededPageCount = (size / self->cachedPageSize) + 1;
  imprintTaggedPageAllocatorAlloc(self->taggedPageAllocator, self->tag,
                                  neededPageCount, &result);
  size_t receivedMemorySize = neededPageCount * self->cachedPageSize;
  imprintLinearAllocatorInit(&self->linear, result.memory, receivedMemorySize,
                             "tagged allocator");
}

static void *imprintTaggedAllocatorAllocDebug(void *self_,
                                       size_t size, const char *sourceFile,
                                       size_t line, const char *description) {

  if (size == 0) {
    return 0;
  }

  ImprintTaggedAllocator* self = (ImprintTaggedAllocator*) self_;
  CLOG_VERBOSE("allocate %zu out of %zu", size, self->linear.size - (uintptr_t)(self->linear.next - self->linear.memory));

  prepareMemory(self, size);

  void* memory = imprintLinearAllocatorAllocDebug(&self->linear, size, sourceFile, line,
                                          description);

  tc_memset_octets(memory, 0xfa, size);

  return memory;
}

static void *imprintTaggedAllocatorCallocDebug(void *self_,
                                        size_t size, const char *sourceFile,
                                        size_t line, const char *description) {
  if (size == 0) {
    return 0;
  }
  ImprintTaggedAllocator* self = (ImprintTaggedAllocator*) self_;

  void *memory = imprintTaggedAllocatorAllocDebug(self, size, sourceFile, line,
                                                  description);
  tc_mem_clear(memory, size);

  return memory;
}

void imprintTaggedAllocatorInit(ImprintTaggedAllocator *self,
                                ImprintTaggedPageAllocator *taggedPageAllocator,
                                uint64_t tag) {
  self->taggedPageAllocator = taggedPageAllocator;
  self->tag = tag;
  self->linear.memory = 0;
  self->linear.next = 0;
  self->linear.size = 0;
  self->cachedPageSize = taggedPageAllocator->pageAllocator->pageSizeInOctets;
  self->info.allocDebugFn = imprintTaggedAllocatorAllocDebug;
  self->info.callocDebugFn = imprintTaggedAllocatorCallocDebug;
}


void imprintTaggedAllocatorFreeAll(ImprintTaggedAllocator* self)
{
  imprintTaggedPageAllocatorFree(self->taggedPageAllocator, self->tag);
  self->tag = 0;
  self->linear.memory = 0;
  self->linear.next = 0;
  self->linear.size = 0;
}

void imprintTaggedAllocatorClearAll(ImprintTaggedAllocator* self)
{
  imprintTaggedPageAllocatorFree(self->taggedPageAllocator, self->tag);
  self->linear.memory = 0;
  self->linear.next = 0;
  self->linear.size = 0;
}
