/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef IMPRINT_MEMORY_H
#define IMPRINT_MEMORY_H

#include <stdbool.h>
#include <memory.h>
#include <stdint.h>

typedef struct ImprintMemory {
  uint8_t *memory;
  size_t size;
  uint8_t *next;
  const char *debug;
} ImprintMemory;

void *imprint_memory_alloc_debug(struct ImprintMemory *memory, size_t size,
                                 const char *source_file, int line,
                                 const char *description);

void *imprint_memory_calloc_debug(struct ImprintMemory *memory, size_t size,
                                  const char *source_file, int line,
                                  const char *description);

void imprintMemoryInit(struct ImprintMemory *memory, size_t size,
                         const char *debug);
void imprintMemoryDestroy(ImprintMemory *self);
void imprint_memory_clear(struct ImprintMemory *memory);
void imprint_memory_print_debug(const struct ImprintMemory *memory);
void imprint_memory_construct(struct ImprintMemory *memory,
                              struct ImprintMemory *parent, size_t size,
                              const char *debug);

void imprintMemoryFree(struct ImprintMemory *memory, void *p);
#define IMPRINT_ENABLE
#if defined IMPRINT_ENABLE

#define IMPRINT_MEMORY_ALLOC(memory, size, description)                          \
  imprint_memory_alloc_debug(memory, size, __FILE__, __LINE__, description)
#define IMPRINT_MEMORY_CALLOC(memory, size, description)                         \
  imprint_memory_calloc_debug(memory, size, __FILE__, __LINE__, description)

#define IMPRINT_MEMORY_CALLOC_TYPE_COUNT(memory, type, count)                    \
  (type *)IMPRINT_MEMORY_CALLOC(memory, count * sizeof(type), #type)
#define IMPRINT_MEMORY_CALLOC_TYPE(memory, type)                                 \
  (type *)IMPRINT_MEMORY_CALLOC(memory, sizeof(type), #type)
#define IMPRINT_MEMORY_ALLOC_TYPE(memory, type)                                  \
  (type *)IMPRINT_MEMORY_ALLOC(memory, sizeof(type), #type)
#define IMPRINT_MEMORY_ALLOC_TYPE_COUNT(memory, type, count)                     \
  (type *)IMPRINT_MEMORY_ALLOC(memory, count * sizeof(type), #type)
#else
#define IMPRINT_MEMORY_ALLOC(memory, size, description)                          \
  malloc(size)
#define IMPRINT_MEMORY_CALLOC(memory, size, description)                         \
  calloc(1, size)

#define IMPRINT_MEMORY_CALLOC_TYPE_COUNT(memory, type, count)                    \
  (type *)IMPRINT_MEMORY_CALLOC(memory, count * sizeof(type), #type);
#define IMPRINT_MEMORY_CALLOC_TYPE(memory, type)                                 \
  (type *)IMPRINT_MEMORY_CALLOC(memory, sizeof(type), #type);
#define IMPRINT_MEMORY_ALLOC_TYPE(memory, type)                                  \
  (type *)IMPRINT_MEMORY_ALLOC(memory, sizeof(type), #type);
#define IMPRINT_MEMORY_ALLOC_TYPE_COUNT(memory, type, count)                     \
  (type *)IMPRINT_MEMORY_ALLOC(memory, count * sizeof(type), #type);
#endif


#endif
