#ifndef Z_AVL_H
#define Z_AVL_H

#include "../pair.h"
#include "../array.h"
#include "../pool.h"
#include <stdbool.h>

typedef struct Z_Avl_Node {
    struct Z_Avl_Node *left;
    struct Z_Avl_Node *right;
    void *key;
    void *value;
    char height;
} Z_Avl_Node;

typedef struct {
    Z_Pool *pool;
    Z_Avl_Node *root;
    Z_Compare_Fn compare_keys;
    size_t size;
} Z_Avl_Tree;

Z_Avl_Tree z_avl_new(Z_Pool *pool, Z_Compare_Fn compare_keys);

void *z_avl_get(const Z_Avl_Tree *tree, const void *key);
bool z_avl_contains(const Z_Avl_Tree *tree, const void *key);

Z_Maybe_Pair z_avl_put(Z_Avl_Tree *tree, void *key, void *value);
Z_Maybe_Pair z_avl_delete(Z_Avl_Tree *tree, void *key);

size_t z_avl_size(const Z_Avl_Tree *tree);
Z_Pair_Array z_avl_to_array(Z_Pool *pool, const Z_Avl_Tree *tree);

void *z_avl_get_min(const Z_Avl_Tree *tree);
void *z_avl_get_max(const Z_Avl_Tree *tree);

void z_avl_foreach(const Z_Avl_Tree *tree,
                   void callback(void *key, void *value, void *arg),
                   void *arg);

#endif
