#include <stdio.h>
#include "../all.h"
Z_DEFINE_ARRAY(Z_Int_Array, int);
Z_DEFINE_ARRAY_VIEW(Z_Int_Array_View, int);

void main() {
    Z_Pool_Auto pool = {0};
    Z_Int_Array a = z_array_new(&pool, Z_Int_Array);
    z_array_push(&a, 10); z_array_push(&a, 30);
    z_array_push_at(&a, 20, 1);
    int more[] = {40, 50};
    Z_Int_Array_View view = { .ptr = more, .length = 2 };
    z_array_push_array(&a, view);
    printf("len=%zu first=%d last=%d peek=%d\n", a.length, a.ptr[0], a.ptr[4], z_array_peek(&a));
    int inserted[] = {15, 16}; Z_Int_Array_View insert_view = { .ptr = inserted, .length = 2 };
    z_array_push_array_at(&a, insert_view, 1);
    z_array_reserve_space_at(&a, 0, 1); a.ptr[0] = 5;
    z_array_ensure_capacity(&a, a.length + 4);
    printf("inserted=%d,%d first=%d capacity=%d\n", a.ptr[1], a.ptr[2], a.ptr[0], a.capacity >= a.length + 4);
    printf("pop=%d\n", z_array_pop(&a));
    z_array_zero_terminate(&a);
    printf("terminator=%d\n", a.ptr[a.length]);
}
