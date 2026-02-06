#ifndef HEAP_H
#define HEAP_H

#include <stdlib.h>

typedef struct {
  void **pointers;
  size_t occupied;
  size_t capacity;
} Z_Pointer_Table;

typedef struct {
  Z_Pointer_Table table;
} Z_Heap;

typedef void (*Z_Free_Fn)(Z_Heap *, void *);

#define Z_Heap_Auto __attribute__((cleanup(z_heap_free))) Z_Heap

Z_Heap z_heap_new();
void *z_heap_malloc(Z_Heap *heap, size_t size);
void *z_heap_calloc(Z_Heap *heap, size_t size);
void *z_heap_realloc(Z_Heap *heap, void *pointer, size_t new_size);
void z_heap_free_pointer(Z_Heap *heap, void *pointer);
void z_heap_free(Z_Heap *heap);
void z_heap_reset(Z_Heap *heap);

#endif
