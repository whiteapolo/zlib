#include "../include/error.h"
#include "../include/heap.h"
#include "../include/string.h"
#include <stdio.h>

void z_die(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    z_die_va(format, args);
    va_end(args);
}

void z_die_va(const char *format, va_list args)
{
    vfprintf(stderr, format, args);
    exit(EXIT_FAILURE);
}

void z_perror(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    z_perror_va(format, args);
    va_end(args);
}

void z_perror_va(const char *format, va_list args)
{
    Z_Heap_Auto heap = {0};
    Z_String s = z_str_new(&heap, "");
    z_str_append_va(&s, format, args);
    perror(s.ptr);
}

void z_enforce(bool predicate, const char *format, ...)
{
    if (predicate) {
        return;
    }

    va_list args;
    va_start(args, format);
    z_die_va(format, args);
    va_end(args);
}
