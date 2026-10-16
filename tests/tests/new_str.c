#include <stdio.h>
#include "../all.h"

void main()
{
    Z_Pool_Auto pool = {0};
    z_sv_println(z_sv(z_str_new(&pool, "Hello world")));
    z_sv_println(z_sv(z_str_new(&pool, "Hello world %d", 10)));
    z_sv_println(z_sv(z_str_new(&pool, "Hello world %s %s", "", "121")));
}
