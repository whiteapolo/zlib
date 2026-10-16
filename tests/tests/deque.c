#include <stdio.h>
#include "../all.h"
void main() {
    Z_Pool_Auto pool={0}; Z_Deque d=z_deque_new(&pool); int a=1,b=2,c=3,dv=4;
    z_deque_push_back(&d,&a); z_deque_push_front(&d,&b); z_deque_push_back(&d,&c);
    printf("size=%zu at1=%d\n", z_deque_size(&d), *(int*)z_deque_at(&d,1));
    printf("front=%d back=%d\n", *(int*)z_deque_pop_front(&d), *(int*)z_deque_pop_back(&d));
    z_deque_push_front(&d,&dv);
    printf("size=%zu front=%d\n", z_deque_size(&d), *(int*)z_deque_pop_front(&d));
}
