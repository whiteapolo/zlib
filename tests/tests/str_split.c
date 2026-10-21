#include <stdio.h>
#include "../all.h"

void main() {
    Z_Pool_Auto pool = {0};
    Z_String_Array parts = z_array_new(&pool, Z_String_Array);
    z_str_split(z_sv("a,b,c"), z_sv(","), &parts);
    for (size_t i = 0; i < parts.length; i++) z_sv_println(z_sv(parts.ptr[i]));
    z_sv_println(z_sv_split_part(z_sv("a:b:c"), z_sv(":"), 1));
}
