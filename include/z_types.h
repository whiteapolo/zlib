#ifndef Z_TYPES_H
#define Z_TYPES_H

#include <stdlib.h>
#include <stdbool.h>

typedef void (*Z_Print_Fn)(const void *);
typedef int (*Z_Compare_Fn)(const void *, const void *);
typedef size_t (*Z_Hash_Fn)(const void *);
typedef bool (*Z_Equal_Fn)(const void *, const void *);

#endif