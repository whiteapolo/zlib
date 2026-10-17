#include <stdio.h>
#include "../all.h"

void main() {
    Z_Pool_Auto pool = {0};
    Z_String s = z_str_new(&pool, "one");
    z_str_push_back(&s, " %d", 2);
    z_str_push_back_cstr(&s, " three");
    z_str_push_back_char(&s, '!');
    z_sv_println(z_sv(&s));
}
