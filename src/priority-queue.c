#include "../include/priority-queue.h"
#include "../include/internal/min-max-heap.h"
#include <stdint.h>

Z_Priority_Queue z_priority_queue_new(Z_Pool *pool, Z_Compare_Fn compare)
{
    return z_priority_queue_new_with_max_size(pool, compare, SIZE_MAX);
}

Z_Priority_Queue z_priority_queue_new_with_max_size(Z_Pool *pool, Z_Compare_Fn compare, size_t max_size)
{
    Z_Priority_Queue queue = {
        .heap = z_min_max_heap_new(pool, compare),
        .max_size = max_size,
    };

    return queue;
}

void *z_priority_queue_push(Z_Priority_Queue *queue, void *element)
{
    z_min_max_heap_push(&queue->heap, element);

    if (queue->max_size < z_min_max_heap_size(&queue->heap)) {
        return z_min_max_heap_pop_min(&queue->heap);
    }

    return NULL;
}

void *z_priority_queue_pop_min(Z_Priority_Queue *queue)
{
    return z_min_max_heap_pop_min(&queue->heap);
}

void *z_priority_queue_pop_max(Z_Priority_Queue *queue)
{
    return z_min_max_heap_pop_max(&queue->heap);
}

void *z_priority_queue_peek_max(const Z_Priority_Queue *queue)
{
    return z_min_max_heap_peek_max(&queue->heap);
}

void *z_priority_queue_peek_min(const Z_Priority_Queue *queue)
{
    return z_min_max_heap_peek_min(&queue->heap);
}

size_t z_priority_queue_size(const Z_Priority_Queue *queue)
{
    return z_min_max_heap_size(&queue->heap);
}
