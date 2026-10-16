#include <stdio.h>
#include "../all.h"

void main() {
    Z_Pool_Auto pool = {0};
    Z_String s = z_str_new(&pool, " \t hello \n");
    z_str_trim(&s);
    z_sv_println(z_sv(&s));
    z_sv_println(z_sv_trim_cset(z_sv("***value**"), z_sv("*")));
}
