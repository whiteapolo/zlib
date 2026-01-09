#include <z_heap.h>
#include <z_config.h>
#include <stdbool.h>
#include <stdint.h>

#define Z_PTR_TABLE_MIN_CAPACITY 16
#define Z_PTR_TABLE_MAX_LOAD_FACTOR 0.7
#define Z_PTR_TABLE_TOMBSTONE ((void*)1)

static inline size_t z__heap_max(size_t a, size_t b)
{
  return a > b ? a : b;
}

static inline bool z__pointer_table_can_insert(void *slot)
{
  return slot == NULL || slot == Z_PTR_TABLE_TOMBSTONE;
}

static inline void **z__pointer_table_find_slot_for_insert(const Z_Pointer_Table *table, const void *pointer)
{
  if (table->capacity == 0) {
    return NULL;
  }

  size_t i = (uintptr_t)pointer % table->capacity;

  while (!z__pointer_table_can_insert(table->pointers[i]) && table->pointers[i] != pointer) {
    i = (i + 1) % table->capacity;
  }

  return &table->pointers[i];
}

static inline void **z__pointer_table_find_slot(const Z_Pointer_Table *table, const void *pointer)
{
  if (table->capacity == 0) {
    return NULL;
  }

  size_t i = (uintptr_t)pointer % table->capacity;

  while (table->pointers[i] && table->pointers[i] != pointer) {
    i = (i + 1) % table->capacity;
  }

  return &table->pointers[i];
}

static inline float z_ptr_table_load_factor(const Z_Pointer_Table *table)
{
  return (float)table->occupied / (float)table->capacity;
}

static inline void z__pointer_table_resize(Z_Pointer_Table *table, size_t new_capacity)
{
  Z_Pointer_Table new_table = {
    .pointers = calloc(new_capacity, sizeof(void *)),
    .capacity = new_capacity,
  };

  for (size_t i = 0; i < table->capacity; i++) {
    if (table->pointers[i] && table->pointers[i] != Z_PTR_TABLE_TOMBSTONE) {
      void **slot = z__pointer_table_find_slot_for_insert(&new_table, table->pointers[i]);
      *slot = table->pointers[i];
      new_table.occupied++;
    }
  }

  free(table->pointers);
  *table = new_table;
}

static inline void z__pointer_table_insert(Z_Pointer_Table *table, void *pointer)
{
  if (table->capacity == 0 || z_ptr_table_load_factor(table) >= Z_PTR_TABLE_MAX_LOAD_FACTOR) {
    size_t new_capacity = z__heap_max(Z_PTR_TABLE_MIN_CAPACITY, table->capacity * Z_BUFFER_GROWTH_FACTOR);
    z__pointer_table_resize(table, new_capacity);
  }

  void **slot =  z__pointer_table_find_slot_for_insert(table, pointer);

  if (*slot == NULL) {
    table->occupied++;
  }

  *slot = pointer;
}

static inline void z__pointer_table_delete(Z_Pointer_Table *table, const void *pointer)
{
  void **slot = z__pointer_table_find_slot(table, pointer);
  table->occupied--;
  *slot = Z_PTR_TABLE_TOMBSTONE;
}

static inline void z__pointer_table_free_cells(Z_Pointer_Table *table)
{
  for (size_t i = 0; i < table->capacity; i++) {
    if (table->pointers[i] && table->pointers[i] != Z_PTR_TABLE_TOMBSTONE) {
      free(table->pointers[i]);
    }
  }
}

static inline void z__pointer_table_reset(Z_Pointer_Table *table)
{
  z__pointer_table_free_cells(table);
  memset(table->pointers, 0, table->capacity * sizeof(void *));
}

static inline void z__pointer_table_free(Z_Pointer_Table *table)
{
  z__pointer_table_free_cells(table);
  free(table->pointers);
}

void *z_heap_malloc(Z_Heap *heap, size_t bytes)
{
  void *pointer = malloc(bytes);
  z__pointer_table_insert(&heap->table, pointer);
  return pointer;
}

void *z_heap_realloc(Z_Heap *heap, void *pointer, size_t new_size)
{
  if (pointer == NULL) {
    return z_heap_malloc(heap, new_size);
  }

  void *new_pointer = realloc(pointer, new_size);

  if (new_pointer != pointer) {
    z__pointer_table_delete(&heap->table, pointer);
    z__pointer_table_insert(&heap->table, new_pointer);
  }

  return new_pointer;
}

void z_heap_free_pointer(Z_Heap *heap, void *pointer)
{
  if (pointer == NULL) {
    return;
  }

  free(pointer);
  z__pointer_table_delete(&heap->table, pointer);
}

void z_heap_free(Z_Heap *heap)
{
  z__pointer_table_free(&heap->table);
}

void z_heap_reset(Z_Heap *heap)
{
  z__pointer_table_reset(&heap->table);
}
