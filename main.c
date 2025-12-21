#include "zlib.h"
#include <printf.h>

int main(void)
{
    Z_Clock clock = z_get_clock();
    Z_Heap heap = {0};
    Z_String s = z_str_new(&heap, "Itay");

    for (int i = 0; i < 100000; i++) {
        z_str_append_cstr(&s, "i");
    }

    z_print_elapsed_seconds(clock);

    return 0;

    Z_Deque_Int deque = z_deque_new(&heap);
    z_deque_push_back(Z_Deque_Int, &deque, 10);
}
