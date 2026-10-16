#include <stdio.h>
#include "../all.h"

void main() {
    Z_String_View s = z_sv("abcdef");
    z_sv_println(z_sv_substring(s, 1, 3));
    z_sv_println(z_sv_advance(s, 4));
    printf("top: %c\n", z_sv_top(s));
}
