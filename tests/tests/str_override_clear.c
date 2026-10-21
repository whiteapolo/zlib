#include <stdio.h>
#include "../all.h"

void main() {
    Z_Pool_Auto pool = {0};
    Z_String s = z_str_new(&pool, "old value");
    z_str_override(&s, "new %s", "value");
    z_sv_println(z_sv(&s));
    z_str_clear(&s);
    printf("cleared length: %zu\n", s.length);
}
