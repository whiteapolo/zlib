#include "../include/internal/avl.h"
#include <math.h>
#include <stdbool.h>

Z_Avl_Node *z__avl_new_node(Z_Pool *pool, void *key, void *value);
Z_Avl_Node *z__avl_get_node(const Z_Avl_Tree *tree, const void *key);
void *z_avl_get(const Z_Avl_Tree *tree, const void *key);
Z_Avl_Node *z__avl_get_min_node(Z_Avl_Node *root);
Z_Avl_Node *z__avl_get_max_node(Z_Avl_Node *root);

char z__avl_get_node_height(const Z_Avl_Node *node);
void z__avl_update_node_height(Z_Avl_Node *node);
int z__avl_get_node_balance_factor(const Z_Avl_Node *node);
void z__avl_left_rotate(Z_Avl_Node **root);
void z__avl_right_rotate(Z_Avl_Node **root);
void z__avl_right_left_rotate(Z_Avl_Node **root);
void z__avl_left_right_rotate(Z_Avl_Node **root);
void z__avl_rebalance_node(Z_Avl_Node **root);

Z_Maybe_Pair z__avl_put(Z_Avl_Node **root, void *key, void *value, Z_Compare_Fn compare_keys, Z_Pool *pool);
Z_Maybe_Pair z__avl_delete(Z_Avl_Node **root, void *key, Z_Compare_Fn compare_keys, Z_Pool *pool);

void z__avl_tree_push_to_array(void *key, void *value, void *arg);
void z__avl_foreach(const Z_Avl_Node *root, void callback(void *key, void *value, void *arg), void *arg);

Z_Avl_Tree z_avl_new(Z_Pool *pool, Z_Compare_Fn compare_keys)
{
    Z_Avl_Tree tree = {
        .pool = pool,
        .compare_keys = compare_keys,
        .root = NULL,
    };

    return tree;
}

Z_Avl_Node *z__avl_new_node(Z_Pool *pool, void *key, void *value)
{
    Z_Avl_Node *node = z_pool_malloc(pool, sizeof(Z_Avl_Node));

    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;

    return node;
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

char z__avl_get_node_height(const Z_Avl_Node *node)
{
    if (node == NULL) {
        return 0;
    }

    return node->height;
}

void z__avl_update_node_height(Z_Avl_Node *node)
{
    node->height = 1 + (char)Z_MAX(z__avl_get_node_height(node->right), z__avl_get_node_height(node->left));
}

int z__avl_get_node_balance_factor(const Z_Avl_Node *node)
{
    if (node == NULL) {
        return 0;
    }

    return z__avl_get_node_height(node->left) - z__avl_get_node_height(node->right);
}

void z__avl_left_rotate(Z_Avl_Node **root)
{
    Z_Avl_Node *new_root = (*root)->right;
    Z_Avl_Node *tmp = new_root->left;

    new_root->left = *root;
    (*root)->right = tmp;

    z__avl_update_node_height(new_root->left);
    z__avl_update_node_height(new_root);

	*root = new_root;
}

void z__avl_right_rotate(Z_Avl_Node **root)
{
    Z_Avl_Node *new_root = (*root)->left;
    Z_Avl_Node *tmp = new_root->right;

    new_root->right = *root;
    (*root)->left = tmp;

    z__avl_update_node_height(new_root->right);
    z__avl_update_node_height(new_root);

	*root = new_root;
}

void z__avl_right_left_rotate(Z_Avl_Node **root)
{
    z__avl_right_rotate(root);
    z__avl_left_rotate(root);
}

void z__avl_left_right_rotate(Z_Avl_Node **root)
{
    z__avl_left_rotate(root);
    z__avl_right_rotate(root);
}

void z__avl_rebalance_node(Z_Avl_Node **root)
{
    z__avl_update_node_height(*root);
    int balance_factor = z__avl_get_node_balance_factor(*root);

    if (balance_factor > 1 && z__avl_get_node_balance_factor((*root)->left) >= 0) {
        z__avl_right_rotate(root);
    } else if (balance_factor < -1 && z__avl_get_node_balance_factor((*root)->right) <= 0) {
        z__avl_left_rotate(root);
    } else if (balance_factor > 1 && z__avl_get_node_balance_factor((*root)->left) < 0) {
        z__avl_left_right_rotate(root);
    } else if (balance_factor < -1 && z__avl_get_node_balance_factor((*root)->right) > 0) {
        z__avl_right_left_rotate(root);
    }
}

Z_Maybe_Pair z__avl_put(Z_Avl_Node **root, void *key, void *value, Z_Compare_Fn compare_keys, Z_Pool *pool)
{
    if (*root == NULL) {
        *root = z__avl_new_node(pool, key, value);
        return z_pair_make_empty();
    }

    int compare_result = compare_keys(key, (*root)->key);

    if (compare_result == 0) {
        Z_Maybe_Pair old_pair = z_pair_make_ok((*root)->key, (*root)->value);
        (*root)->key = key;
        (*root)->value = value;
        return old_pair;
    }

    Z_Maybe_Pair ret;

    if (compare_result > 0) {
        ret = z__avl_put(&(*root)->right, key, value, compare_keys, pool);
    } else {
        ret = z__avl_put(&(*root)->left, key, value, compare_keys, pool);
    }

    z__avl_rebalance_node(root);

    return ret;
}

Z_Maybe_Pair z_avl_put(Z_Avl_Tree *tree, void *key, void *value)
{
    return z__avl_put(&tree->root, key, value, tree->compare_keys, tree->pool);
}

Z_Maybe_Pair z__avl_delete(Z_Avl_Node **root, void *key, Z_Compare_Fn compare_keys, Z_Pool *pool)
{
    if (*root == NULL) {
        return z_pair_make_empty();
    }

    int compare_result = compare_keys(key, (*root)->key);

    if (compare_result > 0) {
        return z__avl_delete(&((*root)->right), key, compare_keys, pool);
    } else if (compare_result < 0) {
        return z__avl_delete(&((*root)->left), key, compare_keys, pool);
    } else {
        if ((*root)->left == NULL) {
            Z_Maybe_Pair old_pair = z_pair_make_ok((*root)->key, (*root)->value);
            Z_Avl_Node *tmp = (*root)->right;
            z_pool_free(pool, *root);
            *root = tmp;
            return old_pair;
        }

        if ((*root)->right == NULL) {
            Z_Maybe_Pair old_pair = z_pair_make_ok((*root)->key, (*root)->value);
            Z_Avl_Node *tmp = (*root)->left;
            z_pool_free(pool, *root);
            *root = tmp;
            return old_pair;
        }

        Z_Avl_Node *successor = z__avl_get_min_node(*root);

        (*root)->key = successor->key;
        (*root)->value = successor->value;

        z__avl_delete(&(*root)->right, successor->key, compare_keys, pool);
    }

    z__avl_rebalance_node(root);

    return z_pair_make_empty();
}

Z_Maybe_Pair z_avl_delete(Z_Avl_Tree *tree, void *key)
{
    Z_Maybe_Pair ret = z__avl_delete(&tree->root, key, tree->compare_keys, tree->pool);

    if (ret.ok) {
        tree->size--;
    }

    return ret;
}

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

Z_Avl_Node *z__avl_get_min_node(Z_Avl_Node *root)
{
    if (root == NULL) {
        return NULL;
    }

    Z_Avl_Node *curr = root;

    while (curr->left) {
        curr = curr->left;
    }

    return curr;
}

void *z_avl_get_min(const Z_Avl_Tree *tree)
{
    Z_Avl_Node *min = z__avl_get_min_node(tree->root);

    if (min == NULL) {
        return NULL;
    }

    return min->value;
}

Z_Avl_Node *z__avl_get_max_node(Z_Avl_Node *root)
{
    if (root == NULL) {
        return NULL;
    }

    Z_Avl_Node *curr = root;

    while (curr->right) {
        curr = curr->right;
    }

    return curr;
}

void *z_avl_get_max(const Z_Avl_Tree *tree)
{
    Z_Avl_Node *max = z__avl_get_max_node(tree->root);

    if (max == NULL) {
        return NULL;
    }

    return max->value;
}

void z__avl_foreach(const Z_Avl_Node *root,
                         void callback(void *key, void *value, void *arg),
                         void *arg)
{
    if (root == NULL) {
        return;
    }

    z__avl_foreach(root->left, callback, arg);
    callback(root->key, root->value, arg);
    z__avl_foreach(root->right, callback, arg);
}

void z_avl_foreach(const Z_Avl_Tree *tree,
                   void callback(void *key, void *value, void *arg),
                   void *arg)
{
    z__avl_foreach(tree->root, callback, arg);
}

