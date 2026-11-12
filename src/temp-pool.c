#include "../include/temp-pool.h"
#include "../include/string.h"
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>

static Z_Flat_Pool temp_pool = {0};

void *z_temp_pool_allocate(size_t size)
{
    if (temp_pool.buffer == NULL) {
        temp_pool = z_flat_pool_new(Z_FLAT_POOL_DEFAULT_VIRTUAL_SIZE);
    }

    return z_flat_pool_allocate(&temp_pool, size);
}

void z_temp_pool_clear()
{
    temp_pool.offset = 0;
}

char *z_format(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char *s = z_format_va(format, args);
    va_end(args);

    return s;
}

char *z_format_va(const char *format, va_list args)
{
    int size = z_get_format_length(format, args);
    assert(size >= 0);
    char *s = z_temp_pool_allocate((size_t)size + 1);
    vsnprintf(s, (size_t)size + 1, format, args);

    return s;
}
