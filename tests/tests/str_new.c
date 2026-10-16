#include <stdio.h>
#include "../all.h"

void main() {
    Z_Pool_Auto pool = {0};
    z_sv_println(z_sv(z_str_new(&pool, "plain text")));
    z_sv_println(z_sv(z_str_new(&pool, "%s:%d", "item", 42)));
    z_sv_println(z_sv(z_str_new_from_cstr(&pool, "from c string")));
}
