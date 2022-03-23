/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <imprint/memory.h>
#include <imprint/memory_pool.h>
#include <tiny-libc/tiny_libc.h>

static void imprint_memory_pool_initialize_entries(imprint_memory_pool *pool) {
  imprint_memory_pool_entry *previous = 0;

  for (size_t i = 0; i < pool->max_count; ++i) {
    imprint_memory_pool_entry *e = &pool->entries[i];

    if (i == 0) {
      pool->first_free_index = 0;
    }
    e->allocated = false;
    e->line = -1;
    e->file = 0;
    e->next_free_index = -1;
    e->pool = pool;

    if (previous) {
      previous->next_free_index = i;
    }
    previous = e;
  }

  pool->count = 0;
}

void imprint_memory_pool_clear(imprint_memory_pool *pool) {
  imprint_memory_pool_initialize_entries(pool);
}

imprint_memory_pool *imprint_memory_pool_construct(ImprintMemory *memory,
                                                   size_t struct_size,
                                                   size_t count,
                                                   const char *type,
                                                   const char *file, int line) {
  CLOG_INFO("Memory alloc '%s' count:%zu", type, count)
  imprint_memory_pool *pool =
      IMPRINT_MEMORY_ALLOC(memory, sizeof(imprint_memory_pool), "Memory pool");

  pool->entries =
      IMPRINT_MEMORY_ALLOC(memory, (sizeof(imprint_memory_pool_entry)) * count,
                         "Memory pool entries");
  pool->struct_size = struct_size;
  const int alignment = 16;
  int rest = struct_size % alignment;
  pool->aligned_struct_size =
      rest == 0 ? struct_size : struct_size + (alignment - rest);
  pool->memory = IMPRINT_MEMORY_CALLOC(
      memory, (pool->aligned_struct_size) * count, "Memory pool entries");
  pool->type_string = type;
  pool->debug_source_file_line = line;
  pool->debug_source_file_name = file;

  pool->max_count = (int)count;
  imprint_memory_pool_initialize_entries(pool);
  return pool;
}

static void *imprint_memory_pool_alloc(imprint_memory_pool *pool) {
  CLOG_ASSERT(pool->count < pool->max_count,
              "Out of memory in pool (%zu) %s %s:%d", pool->count,
              pool->type_string, pool->debug_source_file_name,
              pool->debug_source_file_line)

  if (pool->first_free_index == -1) {
    CLOG_INFO("First free is not available")
    return 0;
  }
  int found_index = pool->first_free_index;
  imprint_memory_pool_entry *e = &pool->entries[found_index];
  CLOG_ASSERT(e, "first free is null")
  pool->first_free_index = e->next_free_index;
  pool->count++;
  uint8_t *m = (uint8_t *)pool->memory + (found_index * pool->aligned_struct_size);
  e->allocated = true;
  // CLOG_ASSERT(((tyran_pointer_to_number)p) % alignment == 0, "alignment
  // error");
  return m;
}

void *imprint_memory_pool_alloc_debug(imprint_memory_pool *pool,
                                      const char *type_name,
                                      size_t struct_size) {
  CLOG_ASSERT(tc_str_equal(pool->type_string, type_name),
              "Type name mismatch. Expected:%s received %s", pool->type_string,
              type_name)
  CLOG_ASSERT(pool->struct_size == struct_size,
              "Struct size mismatch. Expected %zu, received %zu",
              pool->struct_size, struct_size)
  return imprint_memory_pool_alloc(pool);
}

static void *imprint_memory_pool_calloc(imprint_memory_pool *pool) {
  void *p = imprint_memory_pool_alloc(pool);

  CLOG_ASSERT(p, "not null")
  tc_mem_clear(p, pool->struct_size);
  return p;
}

void *imprint_memory_pool_calloc_debug(imprint_memory_pool *pool,
                                       const char *type_name,
                                       size_t struct_size) {
  if (pool == 0) {
    CLOG_WARN("pool is null!!!")
  }
  CLOG_ASSERT(tc_strcmp(pool->type_string, type_name) == 0,
              "Type name mismatch. Expected:%s received %s", pool->type_string,
              type_name)
  CLOG_ASSERT(pool->struct_size == struct_size,
              "Struct size mismatch. Expected %zu, received %zu",
              pool->struct_size, struct_size)

  return imprint_memory_pool_calloc(pool);
}

void *imprint_memory_pool_pointer(imprint_memory_pool *pool, int index) {
  uint8_t *m = (uint8_t *)pool->memory + (index * pool->aligned_struct_size);

  return m;
}

static int index_from_allocation(const imprint_memory_pool *pool,
                                 const void *p) {
  int index_to_free =
      ((const uint8_t *)p - pool->memory) / pool->aligned_struct_size;

  return index_to_free;
}

static imprint_memory_pool_entry *
entry_from_allocation(const imprint_memory_pool *pool, const void *p,
                      int *fetch_index) {
  int index = index_from_allocation(pool, p);

  if (fetch_index != 0) {
    *fetch_index = index;
  }

  return &pool->entries[index];
}

bool imprint_memory_pool_is_marked(const imprint_memory_pool *pool,
                                            const void *allocation) {
  int index_to_free;
  const imprint_memory_pool_entry *e =
      entry_from_allocation(pool, allocation, &index_to_free);

  return e->marked_as_keep;
}

static void imprint_memory_pool_free_entry(imprint_memory_pool *pool,
                                           imprint_memory_pool_entry *e,
                                           int index_to_free) {
  e->allocated = false;
  pool->count--;

  // IMPRINT_LOG("Freeing from memory pool '%s' (%zu) -> index: %d (count:%zu)",
  // pool->type_string, pool->struct_size, index_to_free, pool->count);
  e->next_free_index = e->pool->first_free_index;
  pool->first_free_index = index_to_free;
}

void imprint_memory_pool_free(imprint_memory_pool *pool, void *p) {
  int index_to_free;
  imprint_memory_pool_entry *e = entry_from_allocation(pool, p, &index_to_free);

  imprint_memory_pool_free_entry(pool, e, index_to_free);
}



void imprint_memory_pool_clear_marks(imprint_memory_pool *pool) {
  for (size_t i = 0; i < pool->max_count; ++i) {
    imprint_memory_pool_entry *e = &pool->entries[i];
    e->marked_as_keep = false;
  }
}

void imprint_memory_pool_mark(imprint_memory_pool *pool, void *allocation,
                              int generation) {
  int index_to_free;
  imprint_memory_pool_entry *e =
      entry_from_allocation(pool, allocation, &index_to_free);

  e->marked_as_keep = true;
  e->generation = generation;
}

int imprint_memory_pool_sweep(imprint_memory_pool *pool, int max_generation) {
  imprint_memory_pool_entry *entries = pool->entries;
  int freed = 0;

  for (size_t i = 0; i < pool->max_count; ++i) {
    imprint_memory_pool_entry *e = &entries[i];

    if (!e->marked_as_keep && e->generation <= max_generation) {
      imprint_memory_pool_free_entry(pool, e, i);
      freed++;
    }
  }

  return freed;
}

void imprint_memory_pool_print_debug(const imprint_memory_pool *pool) {
  CLOG_INFO(" pool %s count:%zu max:%zu", pool->type_string, pool->count,
            pool->max_count)
}
