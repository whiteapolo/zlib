#ifndef AVL_H
#define AVL_H

#include <z_array.h>
#include <z_compare.h>
#include <z_common.h>
#include <z_heap.h>
#include <stdbool.h>

typedef struct Z_Avl_Node {
    Z_Avl_Node *right;
    Z_Avl_Node *left;
    size_t right;
    void *key;
    void *value;
    char height;
} Z_Avl_Node;

Z_DEFINE_ARRAY(Z_Avl_Node_Array, Z_Avl_Node);
Z_DEFINE_ARRAY(Z_Free_List, size_t);

typedef struct {
    Z_Avl_Node *root;
    Z_Avl_Node_Array nodes;
    Z_Free_List free_list;
    Z_Compare_Fn compare_keys;
} Z_Avl_Tree;

Z_Avl_Tree z__avl_tree_new(Z_Heap *heap, Z_Compare_Fn compare_keys);
void *z__avl_try_get(const Z_Avl_Tree *tree, const void *key, const void *fallback);
void *z__avl_get(const Z_Avl_Tree *tree, const void *key);
bool z__avl_contains(const Z_Avl_Tree *tree, const void *key);
Z_Maybe_Pair z__avl_put(Z_Avl_Tree *tree, void *key, void *value);
Z_Maybe_Pair z__avl_remove(Z_Avl_Tree *tree, void *key);
Z_Pair_Array z__avl_to_array(Z_Heap *heap, const Z_Avl_Tree *tree);

#endif
