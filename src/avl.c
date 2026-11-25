#include "../include/internal/avl.h"
#include <math.h>
#include <stdbool.h>

#define Z_AVL_TREE_NULL_ID 0

static inline Z_Avl_Node *z_avl_tree_node_by_id(const Z_Avl_Tree *tree, size_t node_id);
char z_avl_tree_get_height(const Z_Avl_Tree *tree, size_t node_id);
void z_avl_tree_update_node_height(const Z_Avl_Tree *tree, size_t node_id);
int z_avl_tree_get_node_balance_factor(const Z_Avl_Tree *tree, size_t node_id);
size_t z_avl_tree_right_rotate(Z_Avl_Tree *tree, size_t root_id);
size_t z_avl_tree_left_rotate(Z_Avl_Tree *tree, size_t root_id);
size_t z_avl_tree_left_right_rotate(Z_Avl_Tree *tree, size_t root_id);
size_t z_avl_tree_right_left_rotate(Z_Avl_Tree *tree, size_t root_id);
size_t z_avl_tree_rebalance_node(Z_Avl_Tree *tree, size_t node_id);
size_t z_avl_tree_generate_next_id(Z_Avl_Tree *tree);
size_t z_avl_tree_new_node(Z_Avl_Tree *tree, void *key, void *value);
void z_avl_tree_free_node(Z_Avl_Tree *tree, size_t node_id);
size_t z_avl_tree_get_min_node(const Z_Avl_Tree *tree, size_t node_id);
size_t z_avl_tree_get_node(const Z_Avl_Tree *tree, const void *key);
size_t z_avl_tree_put_impl(Z_Avl_Tree *tree, size_t node_id, void *key, void *value, Z_Maybe_Pair *pair);
size_t z_avl_tree_delete_impl(Z_Avl_Tree *tree, size_t node_id, void *key, Z_Maybe_Pair *pair);
bool z_avl_tree_is_healthy(const Z_Avl_Tree *tree);
bool z_avl_tree_is_healthy_impl(const Z_Avl_Tree *tree, size_t node_id);
void z_avl_tree_foreach_impl(const Z_Avl_Tree *tree,
                             size_t node_id,
                             void callback(void *key, void *value, void *arg),
                             void *arg);

size_t z_avl_tree_size(const Z_Avl_Tree *tree)
{
    return tree->nodes.length - tree->free_list.length;
}

static inline Z_Avl_Node *z_avl_tree_node_by_id(const Z_Avl_Tree *tree, size_t node_id)
{
    return &tree->nodes.ptr[node_id - 1];
}

char z_avl_tree_get_height(const Z_Avl_Tree *tree, size_t node_id)
{
    if (node_id == Z_AVL_TREE_NULL_ID) {
        return 0;
    }

    return z_avl_tree_node_by_id(tree, node_id)->height;
}

void z_avl_tree_update_node_height(const Z_Avl_Tree *tree, size_t node_id)
{
    Z_Avl_Node *node = z_avl_tree_node_by_id(tree, node_id);
    node->height = 1 + (char)Z_MAX(z_avl_tree_get_height(tree, node->right), z_avl_tree_get_height(tree, node->left));
}

int z_avl_tree_get_node_balance_factor(const Z_Avl_Tree *tree, size_t node_id)
{
    if (node_id == Z_AVL_TREE_NULL_ID) {
        return 0;
    }

    Z_Avl_Node *node = z_avl_tree_node_by_id(tree, node_id);
    return z_avl_tree_get_height(tree, node->left) - z_avl_tree_get_height(tree, node->right);
}

size_t z_avl_tree_left_rotate(Z_Avl_Tree *tree, size_t root_id)
{
    Z_Avl_Node *root = z_avl_tree_node_by_id(tree, root_id);
    size_t pivot_id = root->right;
    Z_Avl_Node *pivot = z_avl_tree_node_by_id(tree, pivot_id);

    size_t pivot_left_id = pivot->left;
    pivot->left = root_id;
    root->right = pivot_left_id;

    z_avl_tree_update_node_height(tree, root_id);
    z_avl_tree_update_node_height(tree, pivot_id);

    return pivot_id;
}

size_t z_avl_tree_right_rotate(Z_Avl_Tree *tree, size_t root_id)
{
    Z_Avl_Node *root = z_avl_tree_node_by_id(tree, root_id);
    size_t pivot_id = root->left;
    Z_Avl_Node *pivot = z_avl_tree_node_by_id(tree, pivot_id);

    size_t pivot_right_id = pivot->right;
    pivot->right = root_id;
    root->left = pivot_right_id;

    z_avl_tree_update_node_height(tree, root_id);
    z_avl_tree_update_node_height(tree, pivot_id);

    return pivot_id;
}

size_t z_avl_tree_left_right_rotate(Z_Avl_Tree *tree, size_t root_id)
{
    size_t new_id = z_avl_tree_left_rotate(tree, z_avl_tree_node_by_id(tree, root_id)->left);
    return z_avl_tree_right_rotate(tree, new_id);
}

size_t z_avl_tree_right_left_rotate(Z_Avl_Tree *tree, size_t root_id)
{
    size_t new_id = z_avl_tree_right_rotate(tree, z_avl_tree_node_by_id(tree, root_id)->right);
    return z_avl_tree_left_rotate(tree, new_id);
}

size_t z_avl_tree_rebalance_node(Z_Avl_Tree *tree, size_t node_id)
{
    Z_Avl_Node *node = z_avl_tree_node_by_id(tree, node_id);
    z_avl_tree_update_node_height(tree, node_id);
    int balance_factor = z_avl_tree_get_node_balance_factor(tree, node_id);

    if (balance_factor > 1 && z_avl_tree_get_node_balance_factor(tree, node->left) >= 0) {
        return z_avl_tree_right_rotate(tree, node_id);
    } else if (balance_factor < -1 && z_avl_tree_get_node_balance_factor(tree, node->right) <= 0) {
        return z_avl_tree_left_rotate(tree, node_id);
    } else if (balance_factor > 1 && z_avl_tree_get_node_balance_factor(tree, node->left) < 0) {
        return z_avl_tree_left_right_rotate(tree, node_id);
    } else if (balance_factor < -1 && z_avl_tree_get_node_balance_factor(tree, node->right) > 0) {
        return z_avl_tree_right_left_rotate(tree, node_id);
    }

    return node_id;
}

size_t z_avl_tree_generate_next_id(Z_Avl_Tree *tree)
{
    if (tree->free_list.length > 0) {
        return z_array_pop(&tree->free_list);
    }

    tree->nodes.length++;
    z_array_reserve(&tree->nodes, tree->nodes.length);
    return tree->nodes.length;
}

size_t z_avl_tree_new_node(Z_Avl_Tree *tree, void *key, void *value)
{
    size_t id = z_avl_tree_generate_next_id(tree);
    Z_Avl_Node *node = z_avl_tree_node_by_id(tree, id);

    node->key = key;
    node->value = value;
    node->height = 0;
    node->right = Z_AVL_TREE_NULL_ID;
    node->left = Z_AVL_TREE_NULL_ID;

    return id;
}

void z_avl_tree_free_node(Z_Avl_Tree *tree, size_t node_id)
{
    z_array_push(&tree->free_list, node_id);
}

Z_Avl_Tree z_avl_tree_new(Z_Pool *pool, Z_Compare_Fn compare_keys)
{
    Z_Avl_Tree tree = {
        .root = Z_AVL_TREE_NULL_ID,
        .nodes = z_array_new(pool, Z_Avl_Node_Array),
        .free_list = z_array_new(pool, Z_Avl_Id_Array),
        .compare_keys = compare_keys,
    };

    return tree;
}

size_t z_avl_tree_get_min_node(const Z_Avl_Tree *tree, size_t node_id)
{
    size_t last = node_id;
    size_t lookahead = z_avl_tree_node_by_id(tree, node_id)->left;

    while (lookahead != Z_AVL_TREE_NULL_ID) {
        last = lookahead;
        lookahead = z_avl_tree_node_by_id(tree, lookahead)->left;
    }

    return last;
}

Z_Maybe_Pair z_avl_tree_get_max(const Z_Avl_Tree *tree)
{
    if (tree->root == Z_AVL_TREE_NULL_ID) {
        Z_Maybe_Pair result = { .ok = false };
        return result;
    }

    size_t curr = tree->root;
    size_t lookahead = z_avl_tree_node_by_id(tree, tree->root)->right;

    while (lookahead != Z_AVL_TREE_NULL_ID) {
        curr = lookahead;
        lookahead = z_avl_tree_node_by_id(tree, lookahead)->right;
    }

    Z_Avl_Node *node = z_avl_tree_node_by_id(tree, curr);

    Z_Maybe_Pair result = {
        .ok = true,
        .pair = z_pair_make(node->key, node->value),
    };

    return result;
}

Z_Maybe_Pair z_avl_tree_get_min(const Z_Avl_Tree *tree)
{
    if (tree->root == Z_AVL_TREE_NULL_ID) {
        Z_Maybe_Pair result = { .ok = false };
        return result;
    }

    size_t curr = tree->root;
    size_t lookahead = z_avl_tree_node_by_id(tree, tree->root)->left;

    while (lookahead != Z_AVL_TREE_NULL_ID) {
        curr = lookahead;
        lookahead = z_avl_tree_node_by_id(tree, lookahead)->left;
    }

    Z_Avl_Node *node = z_avl_tree_node_by_id(tree, curr);

    Z_Maybe_Pair result = {
        .ok = true,
        .pair = z_pair_make(node->key, node->value),
    };

    return result;
}

size_t z_avl_tree_get_node(const Z_Avl_Tree *tree, const void *key)
{
    size_t curr = tree->root;

    while (curr != Z_AVL_TREE_NULL_ID) {
        Z_Avl_Node *node = z_avl_tree_node_by_id(tree, curr);
        int compare_result = tree->compare_keys(key, node->key);

        if (compare_result > 0) {
            curr = node->right;
        } else if (compare_result < 0) {
            curr = node->left;
        } else {
            return curr;
        }
    }

    return curr;
}

const void *z_avl_tree_try_get(const Z_Avl_Tree *tree, const void *key, const void *fallback)
{
    size_t node_id = z_avl_tree_get_node(tree, key);

    if (node_id == Z_AVL_TREE_NULL_ID) {
        return fallback;
    }

    return z_avl_tree_node_by_id(tree, node_id)->value;
}

const void *z_avl_tree_get(const Z_Avl_Tree *tree, const void *key)
{
    return z_avl_tree_try_get(tree, key, NULL);
}

bool z_avl_tree_contains(const Z_Avl_Tree *tree, const void *key)
{
    return z_avl_tree_get_node(tree, key) != Z_AVL_TREE_NULL_ID;
}

size_t z_avl_tree_put_impl(Z_Avl_Tree *tree, size_t node_id, void *key, void *value, Z_Maybe_Pair *pair)
{
    if (node_id == Z_AVL_TREE_NULL_ID) {
        pair->ok = false;
        return z_avl_tree_new_node(tree, key, value);
    }

    int compare_result = tree->compare_keys(key, z_avl_tree_node_by_id(tree, node_id)->key);

    if (compare_result > 0) {
        size_t new_right = z_avl_tree_put_impl(tree, z_avl_tree_node_by_id(tree, node_id)->right, key, value, pair);;
        z_avl_tree_node_by_id(tree, node_id)->right = new_right;
    } else if (compare_result < 0) {
        size_t new_left = z_avl_tree_put_impl(tree, z_avl_tree_node_by_id(tree, node_id)->left, key, value, pair);;
        z_avl_tree_node_by_id(tree, node_id)->left = new_left;
    } else {
        pair->ok = true;
        pair->pair.key = z_avl_tree_node_by_id(tree, node_id)->key;
        pair->pair.value = z_avl_tree_node_by_id(tree, node_id)->value;
        z_avl_tree_node_by_id(tree, node_id)->key = key;
        z_avl_tree_node_by_id(tree, node_id)->value = value;
        return node_id;
    }

    return z_avl_tree_rebalance_node(tree, node_id);
}

Z_Maybe_Pair z_avl_tree_put(Z_Avl_Tree *tree, void *key, void *value)
{
    Z_Maybe_Pair pair;
    tree->root = z_avl_tree_put_impl(tree, tree->root, key, value, &pair);
    return pair;
}

size_t z_avl_tree_delete_impl(Z_Avl_Tree *tree, size_t node_id, void *key, Z_Maybe_Pair *pair)
{
    if (node_id == Z_AVL_TREE_NULL_ID) {
        pair->ok = false;
        return node_id;
    }

    Z_Avl_Node *node = z_avl_tree_node_by_id(tree, node_id);
    int compare_result = tree->compare_keys(key, node->key);

    if (compare_result > 0) {
        node->right = z_avl_tree_delete_impl(tree, node->right, key, pair);
    } else if (compare_result < 0) {
        node->left = z_avl_tree_delete_impl(tree, node->left, key, pair);
    } else {
        if (node->left == Z_AVL_TREE_NULL_ID) {
            pair->ok = true;
            pair->pair = z_pair_make(node->key, node->value);
            z_avl_tree_free_node(tree, node_id);
            return node->right;
        } else if (node->right == Z_AVL_TREE_NULL_ID) {
            pair->ok = true;
            pair->pair = z_pair_make(node->key, node->value);
            z_avl_tree_free_node(tree, node_id);
            return node->left;
        }

        size_t successor_id = z_avl_tree_get_min_node(tree, node->right);
        Z_Avl_Node *successor = z_avl_tree_node_by_id(tree, successor_id);
        node->key = successor->key;
        node->value = successor->value;
        z_avl_tree_delete_impl(tree, node->right, successor->key, pair);
    }

    return z_avl_tree_rebalance_node(tree, node_id);
}

Z_Maybe_Pair z_avl_tree_delete(Z_Avl_Tree *tree, void *key)
{
    Z_Maybe_Pair pair = {0};
    tree->root = z_avl_tree_delete_impl(tree, tree->root, key, &pair);
    return pair;
}

void z_avl_tree_foreach_impl(const Z_Avl_Tree *tree,
                             size_t node_id,
                             void callback(void *key, void *value, void *arg),
                             void *arg)
{
    if (node_id == Z_AVL_TREE_NULL_ID) {
        return;
    }

    Z_Avl_Node *node = z_avl_tree_node_by_id(tree, node_id);
    z_avl_tree_foreach_impl(tree, node->left, callback, arg);
    callback(node->key, node->value, arg);
    z_avl_tree_foreach_impl(tree, node->right, callback, arg);
}

void z_avl_tree_foreach(const Z_Avl_Tree *tree,
                        void callback(void *key, void *value, void *arg),
                        void *arg)
{
    return z_avl_tree_foreach_impl(tree, tree->root, callback, arg);
}

Z_Pair_Array z_avl_tree_to_array(Z_Pool *pool, const Z_Avl_Tree *tree)
{
    Z_Pair_Array array = z_array_new(pool, Z_Pair_Array);

    for (size_t i = 0; i < tree->nodes.length; i++) {
        z_array_push(&array, z_pair_make(tree->nodes.ptr[i].key, tree->nodes.ptr[i].value));
    }

    return array;
}

bool z_avl_tree_is_healthy_impl(const Z_Avl_Tree *tree, size_t node_id)
{
    if (node_id == Z_AVL_TREE_NULL_ID) {
        return true;
    }

    int bf = z_avl_tree_get_node_balance_factor(tree, node_id);
    Z_Avl_Node *node = z_avl_tree_node_by_id(tree, node_id);

    if (bf < -1 || bf > 1) {
        return false;
    }

    return z_avl_tree_is_healthy_impl(tree, node->left) && z_avl_tree_is_healthy_impl(tree, node->right);
}

bool z_avl_tree_is_healthy(const Z_Avl_Tree *tree)
{
    return z_avl_tree_is_healthy_impl(tree, tree->root);
}



// ----------------------------



Z_Avl_Tree z_avl_new(Z_Pool *pool, Z_Compare_Fn compare_keys)
{
    Z_Avl_Tree tree = {
        .pool = pool,
        .compare_keys = compare_keys,
        .root = NULL,
    };

    return tree;
}

Z_Avl_Node *z__avl_get_node(const Z_Avl_Tree *tree, const void *key)
{
    Z_Avl_Node *curr = tree->root;

    while (curr) {
        int cmp = tree->compare_keys(curr->key, key);

        if (cmp > 0) {
            curr = curr->left;
        } else if (cmp < 0) {
            curr = curr->right;
        } else {
            return curr;
        }
    }

    return NULL;
}

void *z_avl_get(const Z_Avl_Tree *tree, const void *key)
{
    Z_Avl_Node *node = z__avl_get_node(tree, key);

    if (!node) {
        return NULL;
    }

    return node->value;
}

bool z_avl_contains(const Z_Avl_Tree *tree, const void *key)
{
    Z_Avl_Node *node = z__avl_get_node(tree, key);
    return node != NULL;
}

void z__avl_rebalance_node(Z_Avl_Node **root, Z_Compare_Fn compare_keys)
{

}

Z_Maybe_Pair z__avl_put_impl(Z_Avl_Node **root, void *key, void *value, Z_Compare_Fn compare_keys, Z_Pool *pool)
{
    if (*root == NULL) {
        *root = z_avl_new_node(pool, key, value);
        return z_make_empty_pair();
    }

    int compare_result = compare_keys(key, (*root)->key);

    if (compare_result == 0) {
        Z_Maybe_Pair old_pair = z_make_ok_pair((*root)->key, (*root)->value);
        (*root)->key = key;
        (*root)->value = value;
        return old_pair;
    }

    Z_Maybe_Pair ret;

    if (compare_result > 0) {
        ret = z__avl_put_impl((*root)->right, key, value, compare_keys, pool);
    } else
        ret = z__avl_put_impl((*root)->left, key, value, compare_keys, pool);
    }

    z__avl_rebalance_node(root, compare_keys);

    return ret;
}

Z_Maybe_Pair z_avl_put(Z_Avl_Tree *tree, void *key, void *value)
{
    return z__avl_put_impl(&tree->root, key, value, tree->compare_keys)
}

Z_Maybe_Pair z_avl_delete(Z_Avl_Node *root, void *key);

size_t z_avl_size(const Z_Avl_Tree *tree)
{
    return tree->size;
}

void z__avl_tree_push_to_array(void *key, void *value, void *arg)
{
    Z_Pair_Array *array = arg;
    Z_Pair pair = z_pair_make(key, value);
    z_array_push(array, pair);
}

Z_Pair_Array z_avl_to_array(Z_Pool *pool, const Z_Avl_Tree *tree)
{
    Z_Pair_Array array = z_array_new(pool, Z_Pair_Array);
    z_avl_foreach(tree, z__avl_tree_push_to_array, &array);

    return array;
}

void *z_avl_get_min(const Z_Avl_Tree *tree)
{
    if (tree->root == NULL) {
        return NULL;
    }

    Z_Avl_Node *curr = tree->root;

    while (curr->left) {
        curr = curr->left;
    }

    return curr;
}

void *z_avl_get_max(const Z_Avl_Tree *tree)
{
    if (tree->root == NULL) {
        return NULL;
    }

    Z_Avl_Node *curr = tree->root;

    while (curr->right) {
        curr = curr->right;
    }

    return curr;
}

void z__avl_tree_foreach_impl(const Z_Avl_Node *root,
                             void callback(void *key, void *value, void *arg),
                             void *arg)
{
    if (root == NULL) {
        return;
    }

    z_avl_foreach_impl(root->left, callback, arg);
    callback(root->key, root->value, arg);
    z_avl_foreach_impl(root->right, callback, arg);
}

void z_avl_foreach(const Z_Avl_Tree *tree,
                        void callback(void *key, void *value, void *arg),
                        void *arg)
{
    z_avl_foreach_impl(tree->root, callback, arg);
}

