#include <stdio.h>
#include <assert.h>
#include <string.h>
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
    Z_Map_Handlers handlers = z_map_create_handlers(
        (Z_Compare_Fn)strcmp,
        (Z_Clone_Fn)strdup,
        (Z_Clone_Fn)strdup,
        free,
        free
    );
    Z_Map *map = z_map_new(handlers);
    z_map_put(map, "Hey", "Bye");
    z_map_put(map, "Foo", "One");
    z_map_put(map, "Bar", "Two");
    z_map_put(map, "Baz", "Three");
    z_map_put(map, "Qux", "Four");
    z_map_put(map, "Alice", "Five");
    z_map_put(map, "Bob", "Six");
    z_map_put(map, "Carol", "Seven");
    z_map_put(map, "Dave", "Eight");
    z_map_put(map, "Eve", "Nine");

    z_map_print(map, (Z_Print_Fn)z_print_string_with_double_quotes, (Z_Print_Fn)z_print_string_with_double_quotes);
    z_map_free(map);

//   test_deque();
}
