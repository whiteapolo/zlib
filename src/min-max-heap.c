#include "../include/internal/min-max-heap.h"
#include <stdbool.h>

static size_t z__min_max_heap_parent(size_t i);
static size_t z__min_max_heap_right(size_t i);
static size_t z__min_max_heap_left(size_t i);

static size_t z__min_max_heap_parent(size_t i)
{
	return (i - 1) / 2;
}

static size_t z__min_max_heap_left(size_t i)
{
	return (i * 2) + 1;
}

static size_t z__min_max_heap_right(size_t i)
{
	return (i * 2) + 2;
}

static bool z__min_max_heap_is_min_level(size_t i)
{
    return i % 2 == 0;
}

static bool z__min_max_heap_swap(Z_Min_Max_Heap *heap, size_t a, size_t b)
{
    void *tmp = heap->elements.ptr[a];
    heap->elements.ptr[a] = heap->elements.ptr[b];
    heap->elements.ptr[b] = tmp;
}

Z_Min_Max_Heap z_min_max_heap_new(Z_Pool *pool, z_compare_fn compare)
{
    Z_Min_Max_Heap heap = {
        .compare = compare,
        .elements = z_array_new(pool, Z_Min_Max_Heap_Elements),
    };

    return heap;
}

void z__min_max_heap_heapify_up(Z_Min_Max_Heap *heap, size_t i);
void z__min_max_heap_heapify_down(Z_Min_Max_Heap *heap, size_t i);

void z__min_max_heap_heapify_up_min(Z_Min_Max_Heap *heap, size_t i);
void z__min_max_heap_heapify_down_min(Z_Min_Max_Heap *heap, size_t i);

void z__min_max_heap_heapify_up_max(Z_Min_Max_Heap *heap, size_t i);
void z__min_max_heap_heapify_down_max(Z_Min_Max_Heap *heap, size_t i);

void z_min_max_heap_push(Z_Min_Max_Heap *heap, void *element)
{
    z_array_push(&heap->elements, element);
    if (heap->elements.length == 1) return;

    size_t i = heap->elements.length - 1;
    size_t parent = z__min_max_heap_parent(i);
    int compare_result = heap->compare(element, heap->elements.ptr[parent]);

    if (z__min_max_heap_is_min_level(i)) {
        if (compare_result > 0) {
            z__min_max_heap_swap(heap, i, parent);
            z__min_max_heap_heapify_up_max(heap, parent);
        } else {
            z__min_max_heap_heapify_up_min(heap, i);
        }
    } else {
        if (compare_result < 0) {
            z__min_max_heap_swap(heap, i, parent);
            z__min_max_heap_heapify_up_min(heap, parent);
        } else {
            z__min_max_heap_heapify_up_max(heap, i);
        }
    }
}

void *z_min_max_heap_pop_min(Z_Min_Max_Heap *heap);
void *z_min_max_heap_pop_max(Z_Min_Max_Heap *heap);

void *z_min_max_heap_peek_max(const Z_Min_Max_Heap *heap)
{
    if (heap->elements.length == 0) return NULL;
    if (heap->elements.length == 1) return heap->elements.ptr[0];
    if (heap->elements.length == 2) return heap->elements.ptr[1];

    void *left = heap->elements.ptr[z__min_max_heap_left(0)];
    void *right = heap->elements.ptr[z__min_max_heap_right(0)];

    return heap->compare(left, right) > 0 ? left : right;
}

void *z_min_max_heap_peek_min(const Z_Min_Max_Heap *heap)
{
    if (heap->elements.length == 0) {
        return NULL;
    }

    return heap->elements.ptr[0];
}

size_t z_min_max_heap_size(const Z_Min_Max_Heap *heap)
{
    return heap->elements.length;
}

