static int z__avl_max(int a, int b)
{
  return a > b ? a : b;
}

int z__avl_get_height(const Z_Avl_Node *node)
{
  return node ? node->height : 0;
}

void z__avl_update_height(Z_Avl_Node *node)
{
  node->height = 1 + z__avl_max(z__avl_get_height(node->right), z__avl_get_height(node->left));
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
  Z_Avl_Node *new_root = (*root)->right;
  Z_Avl_Node *tmp = new_root->left;
  new_root->left = *root;
  (*root)->right = tmp;
  z__avl_update_height(new_root->left);
  z__avl_update_height(new_root);
  *root = new_root;
}

void z__avl_right_rotate(Z_Avl_Node **root)
{
  Z_Avl_Node *new_root = (*root)->left;
  Z_Avl_Node *tmp = new_root->right;
  new_root->right = *root;
  (*root)->left = tmp;
  z__avl_update_height(new_root->right);
  z__avl_update_height(new_root);
  *root = new_root;
}

void z__avl_left_right_rotate(Z_Avl_Node **root)
{
  z__avl_left_rotate(&(*root)->left);
  z__avl_right_rotate(root);
}

void z__avl_right_left_rotate(Z_Avl_Node **root)
{
  z__avl_right_rotate(&(*root)->right);
  z__avl_left_rotate(root);
}

void z__avl_rebalance_node(Z_Avl_Node **node)
{
  z__avl_update_height(*node);
  int balance_factor = z__avl_get_balance_factor(*node);

  if (balance_factor > 1 && z__avl_get_balance_factor((*node)->left) >= 0) {
    z__avl_right_rotate(node);
  } else if (balance_factor < -1 && z__avl_get_balance_factor((*node)->right) <= 0) {
    z__avl_left_rotate(node);
  } else if (balance_factor > 1 && z__avl_get_balance_factor((*node)->left) < 0) {
    z__avl_left_right_rotate(node);
  } else if (balance_factor < -1 && z__avl_get_balance_factor((*node)->right) > 0) {
    z__avl_right_left_rotate(node);
  }
}

Z_Avl_Tree z_avl_tree_new(Z_Heap *heap, Z_Compare_Fn compare_keys)
{
  Z_Avl_Tree tree = {
    .root = NULL,
    .nodes = z_array_new(Z_Avl_Node_Array, heap),
    .compare_keys = compare_keys,
  };

  return tree;
}

Z_Avl_Node *z__avl_find_min(Z_Avl_Node *root)
{
  return root->left ? z__avl_find_min(root->left) : root;
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

void z__avl_print(const Z_Avl_Tree *root, Z_Print_Fn print_key, Z_Print_Fn print_value)
{

}

void z__avl_print_hierarchy(Z_Avl_Tree *tree, Z_Print_Fn print_key, Z_Print_Fn print_value, const char *separator)
{

}
