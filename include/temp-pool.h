#ifndef Z_TEMP_POOL_H
#define Z_TEMP_POOL_H

#include <stdio.h>
#include "flat-pool.h"

void *z_temp_pool_allocate(size_t size);
void z_temp_pool_clear();
char *z_format(const char *format, ...);
char *z_format_va(const char *format, va_list args);

#endif
