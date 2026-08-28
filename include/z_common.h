#ifndef Z_COMMON_H
#define Z_COMMON_H

#include <z_array.h>

typedef struct {
    void *key;
    void *value;
} Z_Pair;

Z_DEFINE_ARRAY(Z_Pair_Array, Z_Pair);

Z_Pair z_make_pair(void *key, void *value);

#endif
