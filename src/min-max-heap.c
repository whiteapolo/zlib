#include "../include/internal/min-max-heap.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

static size_t z__min_max_heap_parent(size_t i);
static size_t z__min_max_heap_grandparent(size_t i);
static size_t z__min_max_heap_left(size_t i);
static size_t z__min_max_heap_right(size_t i);
static bool z__min_max_heap_is_min_level(size_t i);

void z__min_max_heap_heapify_up_min(Z_Min_Max_Heap *heap, size_t i);
void z__min_max_heap_heapify_up_max(Z_Min_Max_Heap *heap, size_t i);
size_t z__min_max_heap_get_smallest_child_or_grandchild_index(const Z_Min_Max_Heap *heap, size_t i);
size_t z__min_max_heap_get_largest_child_or_grandchild_index(const Z_Min_Max_Heap *heap, size_t i);
void z__min_max_heap_heapify_down_min(Z_Min_Max_Heap *heap, size_t i);
void z__min_max_heap_heapify_down_max(Z_Min_Max_Heap *heap, size_t i);
void z__min_max_heap_heapify_down(Z_Min_Max_Heap *heap, size_t i);
size_t z__min_max_heap_get_max_index(const Z_Min_Max_Heap *heap);

static size_t z__min_max_heap_parent(size_t i)
{
	return (i - 1) / 2;
}

static size_t z__min_max_heap_grandparent(size_t i)
{
    return z__min_max_heap_parent(z__min_max_heap_parent(i));
}

static bool z__min_max_heap_has_grandparent(size_t i)
{
    return i != 0 && z__min_max_heap_parent(i) != 0;
}

static size_t z__min_max_heap_has_children(const Z_Min_Max_Heap *heap, size_t i)
{
    return heap->elements.length > z__min_max_heap_left(i);
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
    size_t depth = (size_t)log2((double)i + 1.0);
    return depth % 2 == 0;
}

static int z__min_max_heap_compare(const Z_Min_Max_Heap *heap, size_t a, size_t b)
{
    return heap->compare(heap->elements.ptr[a], heap->elements.ptr[b]);
}

static bool z__min_max_heap_is_grandchild(size_t possible_grandchild, size_t i)
{
    return z__min_max_heap_has_grandparent(possible_grandchild)
        && z__min_max_heap_grandparent(possible_grandchild) == i;
}

static void z__min_max_heap_swap(Z_Min_Max_Heap *heap, size_t a, size_t b)
{
    void *tmp = heap->elements.ptr[a];
    heap->elements.ptr[a] = heap->elements.ptr[b];
    heap->elements.ptr[b] = tmp;
}

Z_Min_Max_Heap z_min_max_heap_new(Z_Pool *pool, Z_Compare_Fn compare)
{
    Z_Min_Max_Heap heap = {
        .compare = compare,
        .elements = z_array_new(pool, Z_Min_Max_Heap_Elements),
    };

    return heap;
}

void z__min_max_heap_heapify_up_min(Z_Min_Max_Heap *heap, size_t i)
{
    if (!z__min_max_heap_has_grandparent(i)) {
        return;
    }

    size_t grandparent = z__min_max_heap_grandparent(i);
    int compare_result = z__min_max_heap_compare(heap, i, grandparent);

    if (compare_result < 0) {
        z__min_max_heap_swap(heap, i, grandparent);
        z__min_max_heap_heapify_up_min(heap, grandparent);
    }
}

void z__min_max_heap_heapify_up_max(Z_Min_Max_Heap *heap, size_t i)
{
    if (!z__min_max_heap_has_grandparent(i)) {
        return;
    }

    size_t grandparent = z__min_max_heap_grandparent(i);
    int compare_result = z__min_max_heap_compare(heap, i, grandparent);

    if (compare_result > 0) {
        z__min_max_heap_swap(heap, i, grandparent);
        z__min_max_heap_heapify_up_max(heap, grandparent);
    }
}

void z_min_max_heap_push(Z_Min_Max_Heap *heap, void *element)
{
    z_array_push(&heap->elements, element);
    if (heap->elements.length == 1) return;

    size_t i = heap->elements.length - 1;
    size_t parent = z__min_max_heap_parent(i);
    int compare_result = z__min_max_heap_compare(heap, i, parent);

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

size_t z__min_max_heap_get_smallest_child_or_grandchild_index(const Z_Min_Max_Heap *heap, size_t i)
{
    size_t possible_children[] = {
        z__min_max_heap_left(i),
        z__min_max_heap_right(i),
        z__min_max_heap_left(z__min_max_heap_left(i)),
        z__min_max_heap_right(z__min_max_heap_left(i)),
        z__min_max_heap_left(z__min_max_heap_right(i)),
        z__min_max_heap_right(z__min_max_heap_right(i)),
    };

    size_t min_index = SIZE_MAX;

    for (size_t j = 0; j < 6; j++) {

        size_t possible_min_child = possible_children[j];

        if (possible_min_child >= heap->elements.length) {
            continue;
        }

        if (min_index == SIZE_MAX || z__min_max_heap_compare(heap, possible_min_child, min_index) < 0) {
            min_index = possible_min_child;
        }
    }

    return min_index;
}
size_t z__min_max_heap_get_largest_child_or_grandchild_index(const Z_Min_Max_Heap *heap, size_t i)
{
    size_t possible_children[] = {
        z__min_max_heap_left(i),
        z__min_max_heap_right(i),
        z__min_max_heap_left(z__min_max_heap_left(i)),
        z__min_max_heap_right(z__min_max_heap_left(i)),
        z__min_max_heap_left(z__min_max_heap_right(i)),
        z__min_max_heap_right(z__min_max_heap_right(i)),
    };

    size_t max_index = SIZE_MAX;

    for (size_t j = 0; j < 6; j++) {

        size_t possible_max_child = possible_children[j];

        if (possible_max_child >= heap->elements.length) {
            continue;
        }

        if (max_index == SIZE_MAX || z__min_max_heap_compare(heap, possible_max_child, max_index) > 0) {
            max_index = possible_max_child;
        }
    }

    return max_index;
}

void z__min_max_heap_heapify_down_min(Z_Min_Max_Heap *heap, size_t i)
{
    if (!z__min_max_heap_has_children(heap, i)) {
        return;
    }

    size_t m = z__min_max_heap_get_smallest_child_or_grandchild_index(heap, i);

    if (z__min_max_heap_compare(heap, m, i) < 0) {
        z__min_max_heap_swap(heap, m, i);

        if (z__min_max_heap_is_grandchild(m, i)) {
            if (z__min_max_heap_compare(heap, m, z__min_max_heap_parent(m)) < 0) {
                z__min_max_heap_swap(heap, m, z__min_max_heap_parent(m));
            }

            z__min_max_heap_heapify_down_min(heap, m);
        }
    }
}

void z__min_max_heap_heapify_down_max(Z_Min_Max_Heap *heap, size_t i)
{
    if (!z__min_max_heap_has_children(heap, i)) {
        return;
    }

    size_t m = z__min_max_heap_get_largest_child_or_grandchild_index(heap, i);

    if (z__min_max_heap_compare(heap, m, i) > 0) {
        z__min_max_heap_swap(heap, m, i);

        if (z__min_max_heap_is_grandchild(m, i)) {
            if (z__min_max_heap_compare(heap, m, z__min_max_heap_parent(m)) > 0) {
                z__min_max_heap_swap(heap, m, z__min_max_heap_parent(m));
            }

            z__min_max_heap_heapify_down_max(heap, m);
        }
    }
}

void z__min_max_heap_heapify_down(Z_Min_Max_Heap *heap, size_t i)
{
    if (z__min_max_heap_is_min_level(i)) {
        z__min_max_heap_heapify_down_min(heap, i);
    } else {
        z__min_max_heap_heapify_down_max(heap, i);
    }
}

void *z_min_max_heap_pop_min(Z_Min_Max_Heap *heap)
{
    assert(heap->elements.length > 0);

    if (heap->elements.length == 1) {
        return z_array_pop(&heap->elements);
    }

    void *min = heap->elements.ptr[0];
    heap->elements.ptr[0] = z_array_pop(&heap->elements);
    z__min_max_heap_heapify_down(heap, 0);

    return min;
}

void *z_min_max_heap_pop_max(Z_Min_Max_Heap *heap)
{
    assert(heap->elements.length > 0);

    if (heap->elements.length == 1) {
        return z_array_pop(&heap->elements);
    }

    size_t max_index = z__min_max_heap_get_max_index(heap);
    void *max = heap->elements.ptr[max_index];
    heap->elements.ptr[max_index] = z_array_pop(&heap->elements);

    return max;
}

size_t z__min_max_heap_get_max_index(const Z_Min_Max_Heap *heap)
{
    assert(heap->elements.length > 0);

    if (heap->elements.length == 1) return 0;
    if (heap->elements.length == 2) return 1;

    size_t left = z__min_max_heap_left(0);
    size_t right = z__min_max_heap_right(0);

    int compare_result = z__min_max_heap_compare(heap, left, right);
    return compare_result > 0 ? left : right;
}

void *z_min_max_heap_peek_max(const Z_Min_Max_Heap *heap)
{
    assert(heap->elements.length > 0);

    size_t max_index = z__min_max_heap_get_max_index(heap);
    return heap->elements.ptr[max_index];
}

void *z_min_max_heap_peek_min(const Z_Min_Max_Heap *heap)
{
    assert(heap->elements.length > 0);

    return heap->elements.ptr[0];
}

size_t z_min_max_heap_size(const Z_Min_Max_Heap *heap)
{
    return heap->elements.length;
}
