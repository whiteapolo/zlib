#include <stdio.h>
#include <assert.h>
#include "zlib.h"

void test_deque()
{
    Z_Deque *dq = z_deque_new();

    // Test push_back
    printf("Testing push_back...\n");
    for (int i = 0; i < 5; i++) {
        z_deque_push_back(dq, (void *)(size_t)i);
    }

    assert(z_deque_length(dq) == 5);

    // Test push_front
    printf("Testing push_front...\n");
    for (int i = 5; i < 10; i++) {
        z_deque_push_front(dq, (void *)(size_t)i);
    }

    assert(z_deque_length(dq) == 10);

    // Print deque elements
    printf("Deque elements:\n");
    for (size_t i = 0; i < z_deque_length(dq); i++) {
        printf("%zu ", (size_t)*z_deque_at(dq, i));
    }
    printf("\n");

    // Test pop_back
    printf("Testing pop_back...\n");
    for (int i = 0; i < 3; i++) {
        size_t val = (size_t)z_deque_pop_back(dq);
        printf("pop_back: %zu\n", val);
    }

    // Test pop_front
    printf("Testing pop_front...\n");
    for (int i = 0; i < 3; i++) {
        size_t val = (size_t)z_deque_pop_front(dq);
        printf("pop_front: %zu\n", val);
    }

    printf("Remaining elements:\n");
    for (size_t i = 0; i < z_deque_length(dq); i++) {
        printf("%zu ", (size_t)*z_deque_at(dq, i));
    }
    printf("\n");

    free(dq->ptr);
    free(dq);
    printf("All tests passed!\n");
}

int main(void)
{
  test_deque();
}
