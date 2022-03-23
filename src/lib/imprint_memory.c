/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <imprint/memory.h>
#include <tiny-libc/tiny_libc.h>

static void imprintMemoryInitEx(ImprintMemory *self, uint8_t *memory,
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

void imprintMemoryInit(ImprintMemory *self, size_t size, const char *debug) {
  uint8_t *memory = tc_malloc(size);
  imprintMemoryInitEx(self, memory, size, debug);
}

void imprint_memory_clear(struct ImprintMemory *self) {
  self->next = self->memory;
}

static void size_string(char *buf, int size, int max_size) {
  const int KILOBYTE = 1024;
  const int MEGABYTE = 1024 * KILOBYTE;
  int factor;
  char *suffix;

  if (size >= MEGABYTE) {
    suffix = "M";
    factor = MEGABYTE;
  } else if (size >= KILOBYTE) {
    suffix = "K";
    factor = KILOBYTE;
  } else {
    suffix = "b";
    factor = 1;
  }

  int value = size / factor;
  int percentage = 100;
  if (max_size != 0) {
    percentage = 100 * size / max_size;
  }
  sprintf(buf, "%d %s (%d %%)", value, suffix, percentage);
}

void imprint_memory_print_debug(const ImprintMemory *self) {
  size_t allocated = self->next - self->memory;
  char buf[32];
  size_string(buf, allocated, self->size);
  CLOG_INFO("mem %s %s", self->debug, buf)
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
