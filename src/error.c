#include "../include/error.h"
#include "../include/pool.h"
#include "../include/string.h"
#include <stdio.h>

void z_die(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    z_die_va(format, ap);
    va_end(ap);
}

void z_die_va(const char *format, va_list ap)
{
    vfprintf(stderr, format, ap);
    exit(EXIT_FAILURE);
}

void z_perror(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    z_perror_va(format, ap);
    va_end(ap);
}

void z_perror_va(const char *format, va_list ap)
{
    Z_Pool_Auto pool = {0};
    Z_String s = z_str_new(&pool, "");
    z_str_push_va(&s, format, ap);
    perror(s.ptr);
}

void z_enforce(bool predicate, const char *format, ...)
{
    if (predicate) {
        return;
    }

    va_list ap;
    va_start(ap, format);
    z_die_va(format, ap);
    va_end(ap);
}

void z_perror_die(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    z_perror_die_va(format, ap);
    va_end(ap);
}

void z_perror_die_va(const char *format, va_list ap)
{
    z_perror_va(format, ap);
    exit(EXIT_FAILURE);
}
