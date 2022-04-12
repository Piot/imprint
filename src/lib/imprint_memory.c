/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <imprint/allocator.h>
#include <imprint/memory.h>
#include <tiny-libc/tiny_libc.h>
#include <imprint/utils.h>

void imprintMemoryInitEx(ImprintMemory *self, uint8_t *memory,
                                   size_t size, const char *debug) {
  self->memory = memory;
  self->next = self->memory;
  self->size = size;
  self->debug = debug;
}

void imprintMemoryDestroy(ImprintMemory *self)
{
    tc_free(self->memory);
    self->memory = 0;
}

void imprintMemoryInit(ImprintMemory *self, size_t size, const char *debug, ImprintAllocator* allocator) {
  uint8_t *memory = IMPRINT_ALLOC(allocator, size, "memoryInit");
  imprintMemoryInitEx(self, memory, size, debug);
}

void imprint_memory_clear(struct ImprintMemory *self) {
  self->next = self->memory;
}



void imprint_memory_print_debug(const ImprintMemory *self) {
  size_t allocated = self->next - self->memory;
  char buf[32];

  CLOG_INFO("mem %s %s", self->debug, imprintSizeAndPercentageToString(buf, 32, allocated, self->size))
}

static void *imprintMemoryAlloc(ImprintMemory *self, size_t size) {
  if (self == 0) {
    CLOG_ERROR("NULL memory %lu  '%s'", size, self->debug)
  }
  if (size == 0) {
    return 0;
  }

  if (self->memory == 0) {
    CLOG_ERROR("Null memory!")
  }
  size_t allocated = self->next - self->memory;
  if (allocated + size > self->size) {
    CLOG_ERROR("Error: Out of memory! %s %zu %zu (%zu/%zu)", self->debug, size,
               allocated / size, allocated, self->size)
  }
  uint8_t *next = self->next;
  self->next += size;
  allocated += size;

  // IMPRINT_LOG("alloc '%s' %d (%d / %d)", self->debug, size, allocated,
  // self->size);

  return next;
}

static void *imprintMemoryCalloc(ImprintMemory *self, size_t size) {
  void *p = imprintMemoryAlloc(self, size);
  if (p == 0) {
    return p;
  }
  tc_mem_clear(p, size);
  return p;
}

void imprint_memory_construct(struct ImprintMemory *self,
                              struct ImprintMemory *parent, size_t size,
                              const char *debug) {
  void *memory = imprintMemoryCalloc(parent, size);
  CLOG_INFO("Prepare memory %zu '%s/%s'", size, parent->debug, debug)
  imprintMemoryInitEx(self, memory, size, debug);
}

void *imprint_memory_alloc_debug(ImprintMemory *self, size_t size,
                                 const char *source_file, int line,
                                 const char *description) {
  if (self == 0) {
    CLOG_ERROR("NULL memory %zu %s:%d '%s'", size, source_file, line,
               description)
  }
  if (self->memory == 0) {
    CLOG_ERROR("Memory is null")
  }
  void *p = imprintMemoryAlloc(self, size);
  if (size > 0 && p == 0) {
    CLOG_WARN("Out of memory %zu %s:%d '%s'", size, source_file, line,
              description)
  }
  return p;
}

void *imprint_memory_calloc_debug(ImprintMemory *self, size_t size,
                                  const char *source_file, int line,
                                  const char *description) {
  if (self == 0) {
    CLOG_ERROR("NULL memory %zu %s:%d '%s'", size, source_file, line,
               description)
  }

  // IMPRINT_LOG("Allocating zeroed memory from '%s':%d", source_file, line);
  void *p = imprintMemoryCalloc(self, size);
  if (size > 0 && p == 0) {
    CLOG_ERROR("Out of memory %zu %s:%d '%s'", size, source_file, line,
               description)
  }
  return p;
}

void imprintMemoryFree(ImprintMemory *memory, void *p) {
    (void) memory;
    (void) p;
  // tc_free(p);
}
