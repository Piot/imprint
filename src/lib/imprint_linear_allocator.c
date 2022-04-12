#include <imprint/linear_allocator.h>

#include <clog/clog.h>
#include <imprint/tagged_allocator.h>
#include <imprint/utils.h>



void imprintLinearAllocatorSelfAlloc(ImprintLinearAllocator *self, ImprintAllocator* allocator,
                                     size_t size, const char *debug) {
  imprintLinearAllocatorInit(self, IMPRINT_ALLOC(allocator, size, debug), size, debug);
}

void imprintLinearAllocatorReset(ImprintLinearAllocator* self)
{
  self->next = self->memory;
}


static inline void *imprintLinearAllocatorAlloc(ImprintLinearAllocator *self, size_t size) {
  if (self == 0) {
    CLOG_ERROR("NULL memory %lu  '%s'", size, self->debug)
  }
  if (size == 0) {
    return 0;
  }

  size_t align = 8;
  if (self->memory == 0) {
    CLOG_ERROR("Null memory!")
  }

  size_t rest = (uintptr_t )self->next % align;
  if (align != 0) {
    self->next += align - rest;
  }

  size_t allocated = self->next - self->memory;

  char buf[64];
  CLOG_VERBOSE("linear allocate %zu %s", size, imprintSizeAndPercentageToString(buf, 64, allocated, self->size))

  if (allocated + size > self->size) {
    CLOG_ERROR("Error: Out of memory! %s %zu %zu (%zu/%zu)", self->debug, size,
               allocated / size, allocated, self->size)
  }
  uint8_t *next = self->next;
  self->next += size;


  // IMPRINT_LOG("alloc '%s' %d (%d / %d)", self->debug, size, allocated,
  // self->size);

  return next;
}


void *imprintLinearAllocatorAllocDebug(ImprintLinearAllocator *self, size_t size,
                                 const char *source_file, int line,
                                 const char *description) {
  if (self == 0) {
    CLOG_ERROR("NULL memory %zu %s:%d '%s'", size, source_file, line,
               description)
  }
  if (self->memory == 0) {
    CLOG_ERROR("Memory is null")
  }
  void *p = imprintLinearAllocatorAlloc(self, size);
  if (size > 0 && p == 0) {
    CLOG_ERROR("Out of memory %zu %s:%d '%s'", size, source_file, line,
              description)
  }
  return p;
}


static void* imprintLinearAllocatorAllocDebugInternal(void* self_, size_t size, const char *sourceFile, size_t lineNumber,
                                                                                    const char *description)
{
  ImprintLinearAllocator *self = (ImprintLinearAllocator *)self_;

  return imprintLinearAllocatorAllocDebug(self, size, sourceFile, lineNumber, description);
}



static void *imprintLinearAllocatorCalloc(ImprintLinearAllocator *self, size_t size) {
  void *p = imprintLinearAllocatorAlloc(self, size);
  if (p == 0) {
    return p;
  }
  tc_mem_clear(p, size);
  return p;
}


void *imprintLinearAllocatorCallocDebug(ImprintLinearAllocator *self, size_t size,
                                  const char *source_file, int line,
                                  const char *description) {
  if (self == 0) {
    CLOG_ERROR("NULL memory %zu %s:%d '%s'", size, source_file, line,
               description)
  }

  // IMPRINT_LOG("Allocating zeroed memory from '%s':%d", source_file, line);
  void *p = imprintLinearAllocatorCalloc(self, size);
  if (size > 0 && p == 0) {
    CLOG_ERROR("Out of memory %zu %s:%d '%s'", size, source_file, line,
               description)
  }
  return p;
}

static void* imprintLinearAllocatorCallocDebugInternal(void* self_, size_t size, const char *sourceFile, size_t lineNumber,
                                                      const char *description)
{
  ImprintLinearAllocator *self = (ImprintLinearAllocator *)self_;

  return imprintLinearAllocatorCallocDebug(self, size, sourceFile, lineNumber, description);
}


void imprintLinearAllocatorInit(ImprintLinearAllocator *self, uint8_t *memory,
                                size_t size, const char *debug) {
  self->memory = memory;
  self->next = self->memory;
  self->size = size;
  self->debug = debug;
  self->info.allocDebugFn = imprintLinearAllocatorAllocDebugInternal;
  self->info.callocDebugFn = imprintLinearAllocatorCallocDebugInternal;
}

