#include <stdio.h>
#include "../all.h"

void main() {
    Z_Pool_Auto pool = {0};
    Z_String s = z_str_new(&pool, "red blue red");
    z_str_replace(&s, z_sv("red"), z_sv("green"));
    z_sv_println(z_sv(&s));
}
