#include "../include/internal/min-max-heap.h"

size_t parent(size_t i)
{
	return i / 2;
}

size_t right(size_t i)
{
	return (i * 2) + 2;
}

size_t left(size_t i)
{
	return (i * 2) + 1;
}

Z_Min_Max_Heap z_min_max_heap_new(Z_Pool *pool, z_compare_fn compare)
{
    Z_Min_Max_Heap heap = {
        .compare = compare,
        .elements = z_array_new(pool, Z_Min_Max_Heap_Elements),
    };

    return heap;
}

void z_min_max_heap_push(Z_Min_Max_Heap *heap, void *element);
void *z_min_max_heap_pop_min(Z_Min_Max_Heap *heap);
void *z_min_max_heap_pop_max(Z_Min_Max_Heap *heap);

void *z_min_max_heap_peek_max(const Z_Min_Max_Heap *heap);
void *z_min_max_heap_peek_min(const Z_Min_Max_Heap *heap);

size_t z_min_max_heap_size(const Z_Min_Max_Heap *heap)
{
    return heap->elements.length;
}

