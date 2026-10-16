#include <stdio.h>
#include "../all.h"

void main() {
    Z_Pool_Auto pool = {0};
    Z_String_View view = z_sv("copy me");
    Z_String copied = z_str_new_from_sv(&pool, view);
    char *cstr = z_sv_to_cstr(&pool, view);
    char *duplicate = z_cstr_dup(&pool, "duplicate");
    z_sv_println(z_sv(copied));
    printf("%s %s\n", cstr, duplicate);
}
