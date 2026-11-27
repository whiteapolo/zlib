#ifndef Z_MIN_MAX_HEAP
#define Z_MIN_MAX_HEAP

#include "../array.h"
#include "types.h"

Z_DEFINE_ARRAY(Z_Min_Max_Heap_Elements, void *);

typedef struct {
    Z_Min_Max_Heap_Elements elements;
    z_compare_fn compare;
} Z_Min_Max_Heap;

Z_Min_Max_Heap z_min_max_heap_new(Z_Pool *pool, z_compare_fn compare);

void z_min_max_heap_push(Z_Min_Max_Heap *heap, void *element);
void *z_min_max_heap_pop_min(Z_Min_Max_Heap *heap);
void *z_min_max_heap_pop_max(Z_Min_Max_Heap *heap);

void *z_min_max_heap_peek_max(const Z_Min_Max_Heap *heap);
void *z_min_max_heap_peek_min(const Z_Min_Max_Heap *heap);

size_t z_min_max_heap_size(const Z_Min_Max_Heap *heap);

#endif
