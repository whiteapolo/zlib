#include <stdio.h>
#include "../all.h"

void main() {
    Z_String_View s = z_sv("hello world");
    printf("find world: %zd\n", z_sv_find_index(s, z_sv("world")));
    printf("contains hello: %d\n", z_sv_contains(s, z_sv("hello")));
    printf("starts hello: %d\n", z_sv_starts_with(s, z_sv("hello")));
    printf("ends world: %d\n", z_sv_ends_with(s, z_sv("world")));
}
