/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef imprint_memory_pool_h
#define imprint_memory_pool_h

#include <stdbool.h>
#include <stdint.h>

struct imprint_memory_pool;
struct ImprintMemory;

typedef struct imprint_memory_pool_entry {
  const char *file;
  int line;
  bool allocated;
  int next_free_index;
  struct imprint_memory_pool *pool;

  int generation;
  bool marked_as_keep;
} imprint_memory_pool_entry;

typedef struct imprint_memory_pool {
  uint8_t *memory;
  size_t struct_size;
  size_t aligned_struct_size;
  size_t count;
  size_t max_count;
  imprint_memory_pool_entry *entries;
  const char *type_string;
  int first_free_index;
  const char *debug_source_file_name;
  int debug_source_file_line;
} imprint_memory_pool;

imprint_memory_pool *
imprint_memory_pool_construct(struct ImprintMemory *memory, size_t struct_size,
                              size_t count, const char *type, const char *file,
                              int line);

void imprint_memory_pool_init(imprint_memory_pool* self, struct ImprintMemory *memory, size_t struct_size,
                              size_t count, const char *type, const char *file,
                              int line);

void *imprint_memory_pool_alloc_debug(imprint_memory_pool *pool,
                                      const char *type_name,
                                      size_t struct_size);
void *imprint_memory_pool_calloc_debug(imprint_memory_pool *pool,
                                       const char *type_name,
                                       size_t struct_size);
void *imprint_memory_pool_pointer(imprint_memory_pool *pool, int index);
void imprint_memory_pool_free(imprint_memory_pool *pool, void *allocation);
void imprint_memory_pool_clear(imprint_memory_pool *pool);
void imprint_memory_pool_print_debug(const imprint_memory_pool *pool);
void imprint_memory_pool_clear_marks(imprint_memory_pool *pool);
bool imprint_memory_pool_is_marked(const imprint_memory_pool *pool,
                                            const void *allocation);
void imprint_memory_pool_mark(imprint_memory_pool *pool, void *allocation,
                              int generation);
int imprint_memory_pool_sweep(imprint_memory_pool *pool, int max_generation);

#define IMPRINT_MEMORY_POOL_CONSTRUCT(memory, T, count)                          \
  imprint_memory_pool_construct(memory, sizeof(T), count, #T, __FILE__,        \
                                __LINE__)
#define IMPRINT_MEMORY_POOL_RAW(pool, index, T)                                  \
  (T *)imprint_memory_pool_pointer(pool, index)
#define IMPRINT_CALLOC_TYPE(pool, T)                                             \
  (T *)imprint_memory_pool_calloc_debug(pool, #T, sizeof(T));
#define IMPRINT_ALLOC_TYPE(pool, type)                                           \
  (type *)imprint_memory_pool_alloc_debug(pool, #type, sizeof(type));
#define IMPRINT_ALLOC_FREE(pool, p) imprint_memory_pool_free(pool, p);
#define IMPRINT_MEMORY_POOL_CLEAR(pool) imprint_memory_pool_clear(pool);

#endif
