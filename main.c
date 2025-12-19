#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wh/messure_time.h>
#include "zlib.h"
#include <assert.h>

void print_deque(const Z_Deque *deque)
{
  for (size_t i = 0; i < deque->length; i++) {
    printf("%d\n", z_deque_at(deque, i));
  }
}

/* assumes:
   void print_deque(const Z_Deque *deque);
*/

static void test_push_back(void)
{
    Z_Deque d = z_deque_new();

    for (int i = 0; i < 5; i++)
        z_deque_push_back(&d, i);

    for (int i = 0; i < 5; i++)
        assert(z_deque_at(&d, i) == i);
}

static void test_push_front(void)
{
    Z_Deque d = z_deque_new();

    for (int i = 0; i < 5; i++)
        z_deque_push_front(&d, i);

    /* front pushes reverse order */
    for (int i = 0; i < 5; i++)
        assert(z_deque_at(&d, i) == 4 - i);
}

static void test_mixed_push(void)
{
    Z_Deque d = z_deque_new();

    z_deque_push_back(&d, 1);   // [1]
    z_deque_push_front(&d, 2);  // [2,1]
    z_deque_push_back(&d, 3);   // [2,1,3]
    z_deque_push_front(&d, 4);  // [4,2,1,3]

    int expect[] = {4, 2, 1, 3};
    for (int i = 0; i < 4; i++)
        assert(z_deque_at(&d, i) == expect[i]);
}

static void test_pop_front(void)
{
    Z_Deque d = z_deque_new();

    for (int i = 0; i < 5; i++)
        z_deque_push_back(&d, i);

    for (int i = 0; i < 5; i++)
        assert(z_deque_pop_front(&d) == i);
}

static void test_pop_back(void)
{
    Z_Deque d = z_deque_new();

    for (int i = 0; i < 5; i++)
        z_deque_push_back(&d, i);

    for (int i = 4; i >= 0; i--)
        assert(z_deque_pop_back(&d) == i);
}

static void test_alternating_ops(void)
{
    Z_Deque d = z_deque_new();

    z_deque_push_back(&d, 1);
    z_deque_push_back(&d, 2);
    z_deque_push_front(&d, 0);

    assert(z_deque_pop_front(&d) == 0);
    z_deque_push_front(&d, -1);
    assert(z_deque_pop_back(&d) == 2);
    assert(z_deque_pop_back(&d) == 1);
    assert(z_deque_pop_front(&d) == -1);
}

static void test_reuse_after_empty(void)
{
    Z_Deque d = z_deque_new();

    for (int i = 0; i < 3; i++)
        z_deque_push_back(&d, i);

    for (int i = 0; i < 3; i++)
        z_deque_pop_front(&d);

    /* reuse */
    z_deque_push_front(&d, 42);
    z_deque_push_back(&d, 43);

    assert(z_deque_at(&d, 0) == 42);
    assert(z_deque_at(&d, 1) == 43);
}

int main(void)
{
    test_push_back();
    test_push_front();
    test_mixed_push();
    test_pop_front();
    test_pop_back();
    test_alternating_ops();
    test_reuse_after_empty();

    printf("all deque tests passed ✓\n");
    return 0;
}
