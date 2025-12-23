#include <internal/z_avl.h>
#include <stdio.h>

int z__max(int a, int b)
{
  return a > b ? a : b;
}

int z__avl_get_height(const Z_Avl_Node *node)
{
  return node ? node->height : 0;
}

void z__avl_update_height(Z_Avl_Node *node)
{
  node->height = 1 + z__max(z__avl_get_height(node->right), z__avl_get_height(node->left));
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

Z_Avl_Node *z__avl_new(Z_Heap *heap, void *key, void *value)
{
  Z_Avl_Node *n = z_heap_malloc(heap, sizeof(Z_Avl_Node));
  n->key = key;
  n->value = value;
  n->height = 1;
  n->left = NULL;
  n->right = NULL;

  return n;
}

Z_Avl_Node *z__avl_get_min(Z_Avl_Node *root)
{
  return root->left ? z__avl_get_min(root->left) : root;
}

Z_Avl_Node *z__avl_get_node(Z_Avl_Node *root, const void *key, Z_Compare_Fn compare_keys)
{
  if (root == NULL) {
    return NULL;
  }

  int compare_result = compare_keys(key, root->key);

  if (compare_result > 0) {
    return z__avl_get_node(root->right, key, compare_keys);
  }

  if (compare_result < 0) {
    return z__avl_get_node(root->left, key, compare_keys);
  }

  return root;
}

bool z__avl_has(Z_Avl_Node *root, void *key, Z_Compare_Fn compare_keys)
{
  return z__avl_get_node(root, key, compare_keys) != NULL;
}

void *z__avl_try_get(Z_Avl_Node *root, const void *key, Z_Compare_Fn compare_keys, const void *fallback)
{
  Z_Avl_Node *node = z__avl_get_node(root, key, compare_keys);
  return node ? node->value : (void*)fallback;
}

void *z__avl_get(Z_Avl_Node *root, const void *key, Z_Compare_Fn compare_keys)
{
  return z__avl_try_get(root, key, compare_keys, NULL);
}

Z_Key_Value z__avl_replace_key_value(Z_Avl_Node *node, void *new_key, void *new_value)
{
  Z_Key_Value old_key_value = {
    .key = node->key,
    .value = node->value,
  };

  node->key = new_key;
  node->value = new_value;

  return old_key_value;
}

Z_Key_Value z__avl_put(
    Z_Heap *heap,
    Z_Avl_Node **root,
    void *key,
    void *value,
    Z_Compare_Fn compare_keys
)
{
  if (*root == NULL) {
    *root = z__avl_new(heap, key, value);
    return (Z_Key_Value){ .key = NULL, .value = NULL };
  }

  int compare_result = compare_keys(key, (*root)->key);

  if (compare_result == 0) {
    return z__avl_replace_key_value(*root, key, value);
  }

  Z_Avl_Node **next_node = compare_result > 0 ? &(*root)->right : &(*root)->left;
  Z_Key_Value old_key_value = z__avl_put(heap, next_node, key, value, compare_keys);
  z__avl_rebalance_node(root);

  return old_key_value;
}

Z_Key_Value z__avl_remove(
    Z_Heap *heap,
    Z_Avl_Node **root,
    void *key,
    Z_Compare_Fn compare_keys
)
{
  if (*root == NULL) {
    return (Z_Key_Value){ .key = NULL, .value = NULL };
  } else if (compare_keys(key, (*root)->key) > 0) {
     return z__avl_remove(heap, &(*root)->right, key, compare_keys);
  } else if (compare_keys(key, (*root)->key) < 0) {
    return z__avl_remove(heap, &(*root)->left, key, compare_keys);
  }

  Z_Key_Value old_key_value = { .key = (*root)->key, .value = (*root)->value };

  if ((*root)->left == NULL || (*root)->right == NULL) {
    Z_Avl_Node *tmp = (*root)->left ? (*root)->left : (*root)->right;
    z_heap_free_pointer(heap, *root);
    *root = tmp;
    return old_key_value;
  }

  Z_Avl_Node *succesor = z__avl_get_min((*root)->right);
  z__avl_replace_key_value(*root, succesor->key, succesor->value);
  z__avl_remove(heap, &(*root)->right, succesor->key, compare_keys);
  z__avl_rebalance_node(root);
  return old_key_value;
}

void z__avl_to_array_implementation(
  Z_Heap *heap, 
  const Z_Avl_Node *root,
  Z_Key_Value_Array *output_array
)
{
  if (root == NULL) {
    return;
  }

  z__avl_to_array_implementation(heap, root->left, output_array);

  Z_Key_Value pair = {
    .key = root->key,
    .value = root->value,
  };

  z_array_push(output_array, pair);

  z__avl_to_array_implementation(heap, root->right, output_array);
}

Z_Key_Value_Array z__avl_to_array(
  Z_Heap *heap, 
  const Z_Avl_Node *root
)
{
  Z_Key_Value_Array array = z_array_new(heap, Z_Key_Value_Array);
  z__avl_to_array_implementation(heap, root, &array);
  return array;
}

void z__avl_print_hierarchy_implementation(
    Z_Avl_Node *root,
    Z_Print_Fn print_key,
    Z_Print_Fn print_value,
    const char *seperator,
    int padding
)
{
  if (root == NULL) {
    return;
  }

  printf("%*c", padding, ' ');
  print_key(root->key);
  printf("%s", seperator);
  print_value(root->value);
  printf("\n");

  z__avl_print_hierarchy_implementation(root->right, print_key, print_value, seperator, padding + 4);
  z__avl_print_hierarchy_implementation(root->left, print_key, print_value, seperator, padding + 4);
}

void z__avl_print_hierarchy(
    Z_Avl_Node *root,
    Z_Print_Fn print_key,
    Z_Print_Fn print_value,
    const char *seperator
)
{
  z__avl_print_hierarchy_implementation(root, print_key, print_value, seperator, 0);
}

void z__avl_print(
    Z_Avl_Node *root,
    Z_Print_Fn print_key,
    Z_Print_Fn print_value
)
{
  Z_Heap_Auto heap = {0};
  Z_Key_Value_Array pairs = z__avl_to_array(&heap, root);

  printf("{\n");

  for (size_t i = 0; i < pairs.length; i++) {
    print_key(pairs.ptr[i].key);
    printf(": ");
    print_value(pairs.ptr[i].value);
    printf(",\n");
  }

  printf("}\n");
}

void z__avl_free(Z_Heap *heap, Z_Avl_Node *root, Z_Free_Fn free_key, Z_Free_Fn free_value)
{
  if (root == NULL) {
    return;
  }

  if (free_key) free_key(heap, root->key);
  if (free_value) free_value(heap, root->value);

  z__avl_free(heap, root->left, free_key, free_value);
  z__avl_free(heap, root->right, free_key, free_value);
  z_heap_free_pointer(heap, root);
}