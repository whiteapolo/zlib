#ifndef Z_AVL_H
#define Z_AVL_H

#include "../types.h"
#include "../array.h"
#include "../compare.h"
#include "../pool.h"
#include <stdbool.h>

typedef struct Z_Avl_Node {
    size_t right;
    size_t left;
    void *key;
    void *value;
    char height;
} Z_Avl_Node;

Z_DEFINE_ARRAY(Z_Avl_Node_Array, Z_Avl_Node);
Z_DEFINE_ARRAY(Z_Avl_Id_Array, size_t);

typedef struct {
    size_t root;
    Z_Avl_Node_Array nodes;
    Z_Avl_Id_Array free_list;
    Z_Compare_Fn compare_keys;
} Z_Avl_Tree;

Z_Avl_Tree z_avl_tree_new(Z_Pool *pool, Z_Compare_Fn compare_keys);
const void *z_avl_tree_try_get(const Z_Avl_Tree *tree, const void *key, const void *fallback);
const void *z_avl_tree_get(const Z_Avl_Tree *tree, const void *key);
bool z_avl_tree_contains(const Z_Avl_Tree *tree, const void *key);
Z_Maybe_Pair z_avl_tree_put(Z_Avl_Tree *tree, void *key, void *value);
Z_Maybe_Pair z_avl_tree_delete(Z_Avl_Tree *tree, void *key);
size_t z_avl_tree_size(const Z_Avl_Tree *tree);
Z_Pair_Array z_avl_tree_to_array(Z_Pool *pool, const Z_Avl_Tree *tree);
Z_Maybe_Pair z_avl_tree_get_max(const Z_Avl_Tree *tree);
Z_Maybe_Pair z_avl_tree_get_min(const Z_Avl_Tree *tree);

bool z_avl_tree_is_healthy(const Z_Avl_Tree *tree);

void z_avl_tree_foreach(const Z_Avl_Tree *tree,
                        void callback(void *key, void *value, void *arg),
                        void *arg);

#endif
