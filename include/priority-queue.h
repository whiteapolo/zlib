#ifndef Z_PRIORITY_QUEUE
#define Z_PRIORITY_QUEUE

#include "internal/min-max-heap.h"
#include "internal/types.h"

typedef struct {
    Z_Min_Max_Heap heap;
    size_t max_size;
} Z_Priority_Queue;

Z_Priority_Queue z_priority_queue_new(Z_Pool *pool, Z_Compare_Fn compare);
Z_Priority_Queue z_priority_queue_new_with_max_size(Z_Pool *pool, Z_Compare_Fn compare, size_t max_size);

// return the min item if max_size exceed.
void *z_priority_queue_push(Z_Priority_Queue *queue, void *element);

void *z_priority_queue_pop_min(Z_Priority_Queue *queue);
void *z_priority_queue_pop_max(Z_Priority_Queue *queue);

void *z_priority_queue_peek_max(const Z_Priority_Queue *queue);
void *z_priority_queue_peek_min(const Z_Priority_Queue *queue);

size_t z_priority_queue_size(const Z_Priority_Queue *queue);

#endif
