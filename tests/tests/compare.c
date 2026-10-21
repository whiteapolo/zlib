#include <stdio.h>
#include "../all.h"
void main() {
    int a=1,b=2; float c=3.0f,d=2.0f; double e=2.0,f=2.0;
    const char *g="alpha", *h="beta";
    printf("%d %d %d %d\n", z_compare_int_pointers(&a,&b), z_compare_float_pointers(&c,&d), z_compare_double_pointers(&e,&f), z_compare_string_pointers(&g,&h));
}
