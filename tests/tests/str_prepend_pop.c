#include <stdio.h>
#include "../all.h"

void main() {
    Z_Pool_Auto pool = {0};
    Z_String s = z_str_new(&pool, "middle");
    z_str_push_front(&s, "start-");
    z_str_push_front_char(&s, '[');
    char last = z_str_pop_char(&s);
    z_sv_println(z_sv(&s));
    printf("popped: %c\n", last);
}
