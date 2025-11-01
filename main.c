#include <stdio.h>
#include "zlib.h"

int main(void)
{
    Z_String *s = z_str_new("Hello %s", "World");
    printf("s: %s\n", s);

    z_str_append(&s, ", number %d", 42);
    printf("after append: %s\n", s);

    z_str_append_char(&s, '!');
    printf("after append_char: %s\n", s);

    char last = z_str_pop_char(&s);
    printf("after pop_char: %s, popped: %c\n", s, last);

    z_str_free(&s);
    printf("s freed, pointer: %p\n", (void*)s);
}
