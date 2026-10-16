#include <stdio.h>
#include "../all.h"

void main() {
    Z_Pool_Auto pool = {0};
    Z_String s = z_str_new(&pool, "one");
    z_str_append(&s, " %d", 2);
    z_str_append_cstr(&s, " three");
    z_str_append_char(&s, '!');
    z_sv_println(z_sv(&s));
}
