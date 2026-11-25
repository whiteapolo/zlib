#include "../include/pair.h"

Z_Pair z_pair_make(void *key, void *value)
{
    Z_Pair pair = {
        .key = key,
        .value = value,
    };

    return pair;
}

Z_Maybe_Pair z_pair_make_ok(void *key, void *value)
{
    Z_Maybe_Pair pair = {
        .ok = true,
        .pair = z_pair_make(key, value),
    };

    return pair;
}

Z_Maybe_Pair z_pair_make_empty()
{
    Z_Maybe_Pair pair = {
        .ok = false,
    };

    return pair;
}

