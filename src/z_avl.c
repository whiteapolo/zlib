// #include <internal/z_avl.h>
// #include <math.h>

// #define Z__AVL_NULL_ID 0

// size_t z_avl_tree_size(const Z_Avl_Tree *tree)
// {
//     return tree->nodes.length - tree->free_list.length;
// }

// static inline Z_Avl_Node *z__avl_node_by_id(const Z_Avl_Tree *tree, size_t node_id)
// {
//     return &tree->nodes[node_id - 1];
// }

// int z_avl_tree_get_height(const Z_Avl_Tree *tree, size_t node_id)
// {
//     if (node_id == Z__AVL_NULL_ID) {
//         return 0;
//     }

//     return z__avl_node_by_id(tree, node_id)->height;
// }

// void z__avl_update_height(const Z_Avl_Tree *tree, size_t node_id)
// {
//     Z_Avl_Node *node = z__avl_node_by_id(tree, node_id);
//     node->height = 1 + Z_MIN(z_avl_tree_get_height(tree, node->right), z_avl_tree_get_height(tree, node->left));
// }

// int z_avl_tree_get_balance_factor(const Z_Avl_Tree *tree, size_t node_id)
// {
//     if (node_id == Z__AVL_NULL_ID) {
//         return 0;
//     }

//     Z_Avl_Node *node = z__avl_node_by_id(tree, node_id);
//     return z_avl_tree_get_height(tree, node->left) - z_avl_tree_get_height(tree, node->right);
// }

// void z__avl_left_rotate(Z_Avl_Tree *tree, size_t root_id)
// {
//     Z_Avl_Node *right = root->right;
//     root->right = right->left;
//     right->left = root;

//     z__avl_update_height(root->right);
//     z__avl_update_height(*root);
//     *root = right;
// }

// void z__avl_right_rotate(Z_Avl_Tree *tree, size_t root_id)
// {
//     Z_Avl_Node *right = root->left;
//     root->left = left->right;
//     left->right = root;

//     z__avl_update_height(root->left);
//     z__avl_update_height(*root);
//     *root = left;
// }

// void z__avl_left_right_rotate(Z_Avl_Tree *tree, size_t root_id)
// {
//     z__avl_left_rotate(tree, z__avl_node_by_id(tree, node)->left);
//     z__avl_right_rotate(node);
// }

// void z__avl_right_left_rotate(Z_Avl_Tree *tree, size_t root_id)
// {
//     z__avl_right_rotate(tree, z__avl_node_by_id(tree, node)->right);
//     z__avl_left_rotate(node);
// }

// void z__avl_rebalance_node(Z_Avl_Tree *tree, size_t node_id)
// {
//     Z_Avl_Node *node = z__avl_node_by_id(tree, node_id);
//     z__avl_update_height(tree, node_id);
//     int balance_factor = z_avl_tree_get_balance_factor(node);

//     if (balance_factor > 1 && z_avl_tree_get_balance_factor(tree, node->left) >= 0) {
//         z__avl_right_rotate(tree, node_id);
//     } else if (balance_factor < -1 && z_avl_tree_get_balance_factor(tree, node->right) <= 0) {
//         z__avl_left_rotate(tree, node_id);
//     } else if (balance_factor > 1 && z_avl_tree_get_balance_factor(tree, node->left) < 0) {
//         z__avl_left_right_rotate(tree, node_id);
//     } else if (balance_factor < -1 && z_avl_tree_get_balance_factor(tree, node->right) > 0) {
//         z__avl_right_left_rotate(tree, node_id);
//     }
// }

// size_t z_avl_tree_get_next_id(Z_Avl_Tree *tree)
// {
//     if (tree->free_list.length > 0) {
//         return z_array_pop(&tree->free_list);
//     }

//     z_array_ensure_capacity(&tree->nodes, tree->nodes.length + 1);
//     tree->nodes.length++;
//     return tree->nodes.length;
// }

// size_t z__avl_new_node(Z_Avl_Tree *tree, void *key, void *value)
// {
//     Z_Avl_Node node = {
//         .key = key,
//         .value = value,
//         .height = 0,
//         .right = Z__AVL_NULL_ID,
//         .left = Z__AVL_NULL_ID,
//     };

//     size_t id = z_avl_tree_get_next_id(tree);
//     tree->nodes.ptr[id] = node;
//     return id;
// }

// Z_Avl_Tree z_avl_tree_new(Z_Heap *heap, Z_Compare_Fn compare_keys)
// {
//     Z_Avl_Tree tree = {
//         .root = Z__AVL_NULL_ID,
//         .nodes = z_array_new(Z_Avl_Node_Array, heap),
//         .free_list = z_array_new(Z_Avl_Id_Array, heap),
//         .compare_keys = compare_keys,
//     };

//     return tree;
// }

// size_t z__avl_find_min(const Z_Avl_Tree *tree, size_t node_id)
// {
//     size_t last = node_id;
//     size_t lookahead = z__avl_node_by_id(tree, node_id)->left;

//     while (lookahead != Z__AVL_NULL_ID) {
//         last = lookahead;
//         lookahead = z__avl_node_by_id(tree, lookahead)->left;
//     }

//     return last;
// }

// size_t z__avl_find_node(const Z_Avl_Tree *tree, const void *key)
// {
//     size_t curr = tree->root;

//     while (curr != Z__AVL_NULL_ID) {
//         Z_Avl_Node *node = z__avl_node_by_id(tree, curr);
//         int compare_result = tree->compare_keys(key, node->key);

//         if (compare_result > 0) {
//             curr = node->right;
//         } else if (compare_result < 0) {
//             curr = node->left;
//         } else {
//             return curr;
//         }
//     }

//     return curr;
// }

// void *z_avl_tree_try_get(const Z_Avl_Tree *tree, const void *key, void *fallback)
// {
//     size_t node_id = z__avl_find_node(tree, key);

//     if (node_id == Z__AVL_NULL_ID) {
//         return fallback;
//     }

//     return z__avl_node_by_id(tree, node_id)->value;
// }

// void *z_avl_tree_get(const Z_Avl_Tree *tree, const void *key)
// {
//     return z_avl_tree_try_get(tree, key, NULL);
// }

// bool z_avl_tree_contains(const Z_Avl_Tree *tree, void *key)
// {
//     return z__avl_find_node(tree, key) != Z__AVL_NULL_ID;
// }

// Z_Key_Value z_avl_tree_put(Z_Avl_Tree *tree, void *key, void *value)
// {

// }

// Z_Key_Value z_avl_tree_delete(Z_Avl_Tree *tree, void *key)
// {

// }

// Z_Avl_tree_Iter z_avl_tree_iter(Z_Heap *heap, const Z_Avl_Tree *tree)
// {
//     Z_Avl_tree_Iter iter = {
//         .tree = tree,
//         .did_visit_left = false,
//         .did_visit_curr = false,
//         .did_visit_right = false,
//         .stack = z_array_new(heap, Z_Avl_Id_Array),
//     };

//     size_t curr = tree->root;

//     while (curr != Z__AVL_NULL_ID) {
//         z_array_push(&iter.stack, curr);
//         curr = z__avl_node_by_id(tree, curr)->left;
//     }

//     return iter;
// }

// bool z_avl_tree_iter_next(Z_Avl_tree_Iter *iter, Z_Pair *pair)
// {

// }

// Z_Pair_Array z_avl_tree_to_array(Z_Heap *heap, const Z_Avl_Tree *tree)
// {
//     Z_Pair_Array array = z_array_new(heap, Z_Pair_Array);

//     for (size_t i = 0; i < tree->nodes.length; i++) {

//         Z_Pair pair = {
//             .key = tree->nodes.ptr[i].key,
//             .value = tree->nodes.ptr[i].value,
//         };

//         z_array_push(&array, pair);
//     }

//     return array;
// }
