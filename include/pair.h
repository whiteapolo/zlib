#ifndef Z_PAIR_H
#define Z_PAIR_H

typedef struct {
    void *key;
    void *value;
} Z_Pair;

typedef struct {
    bool ok;
    Z_Pair pair;
} Z_Maybe_Pair;

Z_DEFINE_ARRAY(Z_Pair_Array, Z_Pair);

Z_Pair z_pair_make(void *key, void *value);
Z_Maybe_Pair z_pair_make_ok(void *key, void *value);
Z_Maybe_Pair z_pair_make_empty();

#endif
