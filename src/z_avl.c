#include <z_avl.h>
#include <z_math.h>

#define Z__AVL_NULL_ID INT_MAX

int z__avl_get_height(const Z_Avl_Node *node)
{
    if (node == NULL) {
        return 0;
    }

    return node->height;
}

void z__avl_update_height(Z_Avl_Node *node)
{
    node->height = 1 + z__min_char(z__avl_get_height(node->right), z__avl_get_height(node->left));
}

int z__avl_get_balance_factor(const Z_Avl_Node *node)
{
    if (node == NULL) {
        return 0;
    }

    return z__avl_get_height(node->left) - z__avl_get_height(node->right);
}

void z__avl_left_rotate(Z_Avl_Node **root)
{
    Z_Avl_Node *right = root->right;
    root->right = right->left;
    right->left = root;

    z__avl_update_height(root->right);
    z__avl_update_height(*root);
    *root = right;
}

void z__avl_right_rotate(Z_Avl_Node **root)
{
    Z_Avl_Node *right = root->left;
    root->left = left->right;
    left->right = root;

    z__avl_update_height(root->left);
    z__avl_update_height(*root);
    *root = left;
}

void z__avl_left_right_rotate(Z_Avl_Tree *tree, unsigned int node)
{
    z__avl_left_rotate(tree, z__avl_get_node(tree, node)->left);
    z__avl_right_rotate(node);
}

void z__avl_right_left_rotate(Z_Avl_Node **root)
{
    z__avl_right_rotate(tree, z__avl_get_node(tree, node)->right);
    z__avl_left_rotate(node);
}

void z__avl_rebalance_node(Z_Avl_Tree *tree, unsigned int node)
{
    z__avl_update_height(tree, node);
    int balance_factor = z__avl_get_balance_factor(tree, node);

    unsigned int left = z__avl_get_node(tree, node)->left;
    unsigned int right = z__avl_get_node(tree, node)->right;

    if (balance_factor > 1 && z__avl_get_balance_factor(tree, left) >= 0) {
        z__avl_right_rotate(tree, node);
    } else if (balance_factor < -1 && z__avl_get_balance_factor(tree, right) <= 0) {
        z__avl_left_rotate(tree, node);
    } else if (balance_factor > 1 && z__avl_get_balance_factor(tree, left) < 0) {
        z__avl_left_right_rotate(tree, node);
    } else if (balance_factor < -1 && z__avl_get_balance_factor(tree, right) > 0) {
        z__avl_right_left_rotate(tree, node);
    }
}

Z_Avl_Tree z_avl_tree_new(Z_Heap *heap, Z_Compare_Fn compare_keys)
{
    Z_Avl_Tree tree = {
        .root = Z__AVL_NULL_ID,
        .nodes = z_array_new(Z_Avl_Node_Array, heap),
        .free_list = z_array_new(Z_Free_List, heap),
        .compare_keys = compare_keys,
    };

    return tree;
}

Z_Avl_Node *z__avl_find_min(const Z_Avl_Tree *tree, unsigned int node)
{
    unsigned int curr = node;
    unsigned int tmp;

    while ((tmp = z__avl_get_node(tree, curr)->left) != Z__AVL_NULL_ID) {
        curr = tmp;
    }

    return curr;
}

Z_Avl_Node *z__avl_find_node(const Z_Avl_Tree *tree, const void *key)
{
    Z_Avl_Node node = tree->root;

    while (node != NULL) {
        int compare_result = tree->compare_keys(key, node->key);

        if (compare_result > 0) {
            node = node->right;
        } else if (compare_result < 0) {
            node = node->left;
        } else {
            return node;
        }
    }

    return NULL;
}

void *z__avl_try_get(const Z_Avl_Tree *tree, const void *key, void *fallback)
{
    Z_Avl_Node *node = z__avl_find_node(tree, key);
    return node ? node->value : fallback;
}

void *z__avl_get(const Z_Avl_Tree *tree, const void *key)
{
    return z__avl_try_get(tree, key, NULL);
}

bool z__avl_contains(const Z_Avl_Tree *tree, void *key)
{
    return z__avl_find_node(tree, key) != NULL;
}

Z_Key_Value z__avl_put(Z_Avl_Tree *tree, void *key, void *value)
{

}

Z_Key_Value z__avl_remove(Z_Avl_Tree *tree, void *key)
{

}

Z_Key_Value_Array z__avl_to_array(Z_Heap *heap, const Z_Avl_Tree *tree)
{
    Z_Key_Value_Array array = z_array_new(heap, Z_Key_Value_Array);

    for (size_t i = 0; i < tree->nodes.length; i++) {

        Z_Key_Value pair = {
            .key = tree->nodes.ptr[i].key,
            .value = tree->nodes.ptr[i].value,
        };

        z_array_push(&array, pair);
    }

    return array;
}
