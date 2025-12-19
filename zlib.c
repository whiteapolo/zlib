#include "zlib.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <assert.h>
#include <stdint.h>

#define Z__WHITE_SPACE " \f\n\r\t\v"

int z__size_t_to_int(size_t a)
{
  return a > INT_MAX ? INT_MAX : (int)a;
}

size_t z__min_size_t(size_t a, size_t b)
{
  return a > b ? b : a;
}

size_t z__max_size_t(size_t a, size_t b)
{
  return a > b ? a : b;
}

int z__max(int a, int b)
{
  return a > b ? a : b;
}

int z__min(int a, int b)
{
  return a < b ? a : b;
}

size_t z__get_file_size(FILE *fp)
{
  size_t curr = ftell(fp);
  fseek(fp, 0, SEEK_END);

  size_t size = ftell(fp);
  fseek(fp, curr, SEEK_SET);

  return size;
}

size_t z__get_format_length(const char *format, va_list args)
{
  va_list args_copy;
  va_copy(args_copy, args);

  size_t size = vsnprintf(NULL, 0, format, args_copy);
  va_end(args_copy);

  return size;
}

Z_String z_str_new(Z_Heap *heap, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  Z_String s = z_str_new_args(heap, format, args);
  va_end(args);
  return s;
}

Z_String z_str_new_args(Z_Heap *heap, const char *format, va_list args)
{
  Z_String s = z_array_new(heap, Z_String);
  z_str_append_args(&s, format, args);
  return s;
}

Z_String z_str_new_from(Z_Heap *heap, Z_String_View s)
{
  return z_str_new(heap, "%.*s", z__size_t_to_int(s.length), s.ptr);
}

void z_str_append(Z_String *s, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  z_str_append_args(s, format, args);
  va_end(args);
}

void z_str_append_args(Z_String *s, const char *format, va_list args)
{
  size_t format_length = z__get_format_length(format, args);
  z_array_ensure_capacity(s, s->length + format_length + 1);

  va_list args_copy;
  va_copy(args_copy, args);
  vsnprintf(s->ptr + s->length, format_length + 1, format, args_copy);
  va_end(args_copy);

  s->length += format_length;
}

void z_str_append_str(Z_String *target, Z_String_View source)
{
  z_str_append(target, "%.*s", z__size_t_to_int(source.length), source.ptr);
}

void z_str_append_char(Z_String *s, char c)
{
  z_str_append(s, "%c", c);
}

void z_str_prepend(Z_String *s, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  z_str_prepend_args(s, format, args);
  va_end(args);
}

void z_str_prepend_args(Z_String *s, const char *format, va_list args)
{
  Z_Heap_Auto heap = {0};
  Z_String tmp = z_str_new_args(&heap, format, args);
  z_str_append(&tmp, "%s", s->ptr);
  z_str_clear(s);
  z_str_append(s, "%s", tmp.ptr);
}

void z_str_prepend_str(Z_String *target, Z_String_View source)
{
  z_str_prepend(target, "%.*s", z__size_t_to_int(source.length), source.ptr);
}

void z_str_prepend_char(Z_String *s, char c)
{
  z_str_prepend(s, "%c", c);
}

char z_str_pop_char(Z_String *s)
{
  char c = z_array_pop(s);
  z_array_zero_terminate(s);
  return c;
}

void z_str_replace(Z_String *s, Z_String_View target, Z_String_View replacement)
{
  Z_Heap_Auto heap = {0};
  Z_String tmp = z_str_new(&heap, "");

  size_t i = 0;

  while (i < s->length) {
    if (z_sv_equal(z_sv_offset(z_sv_from_str(s), i), target)) {
      z_str_append_str(&tmp, replacement);
      i += replacement.length;
    } else {
      z_str_append_char(&tmp, s->ptr[i]);
      i++;
    }
  }

  z_str_clear(s);
  z_str_append(s, "%s", tmp.ptr);
}

Z_String z_str_join(Z_Heap *heap, const Z_String_Array *array, Z_String_View delimiter)
{
  if (array->length == 0) {
    return z_str_new(heap, "");
  }

  Z_String result = z_str_new(heap, "");

  for (size_t i = 0; i < array->length - 1; i++) {
    z_str_append_str(&result, z_sv_from_str(array->ptr + i));
    z_str_append_str(&result, delimiter);
  }

  z_str_append_str(&result, z_sv_from_str(&z_array_peek(array)));
  return result;
}

Z_String_Array z_str_split(Z_Heap *heap, Z_String_View s, Z_String_View delimiter)
{
  if (delimiter.length == 0) {
    return z_array_new(heap, Z_String_Array);
  }

  Z_String_Array result = z_array_new(heap, Z_String_Array);

  ssize_t offset = 0;
  ssize_t length = 0;

  while ((length = z_sv_find_index(z_sv_offset(s, offset), delimiter)) != -1) {
    Z_String_View slice = z_sv_substring(s, offset, offset + length);
    z_array_push(&result, z_str_new_from(heap, slice));
    offset += length + delimiter.length;
  }

  z_array_push(&result, z_str_new_from(heap, z_sv_substring(s, offset, s.length)));

  return result;
}

Z_String_View z_sv_from_str(const Z_String *s)
{
  Z_String_View view = {
    .ptr = s->ptr,
    .length = s->length
  };

  return view;
}

Z_String_View z_sv_from_cstr(const char *s)
{
  Z_String_View view = {
    .ptr = s,
    .length = strlen(s)
  };

  return view;
}

Z_String_View z_sv_offset(Z_String_View s, size_t offset)
{
  Z_String_View view = {
    .ptr = s.ptr + offset,
    .length = z__size_t_to_int(s.length) - z__size_t_to_int(offset) < 0 ? 0 : s.length - offset,
  };

  return view;
}

Z_String_View z_sv_substring(Z_String_View s, int start, int end)
{
  Z_String_View view = {
    .ptr = s.ptr + start,
    .length = end - start,
  };

  return view;
}

char z_sv_peek(Z_String_View s)
{
  return s.ptr[s.length - 1];
}

int z_sv_compare(Z_String_View a, Z_String_View b)
{
  int result = memcmp(a.ptr, b.ptr, z__min_size_t(a.length, b.length));
  return result == 0 ? result : z__size_t_to_int(a.length - b.length);
}

bool z_sv_equal(Z_String_View a, Z_String_View b)
{
  return z_sv_compare(a, b) == 0;
}

int z_sv_compare_n(Z_String_View a, Z_String_View b, size_t n)
{
  if (a.length < n) return -1;
  if (b.length < n) return 1;
  return memcmp(a.ptr, b.ptr, n);
}

bool z_sv_equal_n(Z_String_View a, Z_String_View b, size_t n)
{
  return z_sv_compare_n(a, b, n) == 0;
}

bool z_sv_ends_with(Z_String_View s, Z_String_View end)
{
  if (s.length < end.length) {
    return false;
  }

  Z_String_View endings = {
      .ptr = s.ptr + s.length - end.length,
      .length = end.length,
  };

  return z_sv_equal(endings, end);
}

bool z_sv_starts_with(Z_String_View s, Z_String_View start)
{
  if (start.length > s.length) {
    return false;
  }

  return z_sv_equal_n(s, start, start.length);
}

bool z_sv_contains(Z_String_View haystack, Z_String_View needle)
{
  return z_sv_find_index(haystack, needle) != -1;
}

bool z_sv_contain_char(Z_String_View s, char c)
{
  return memchr(s.ptr, c, s.length);
}

ssize_t z_sv_find_index(Z_String_View haystack, Z_String_View needle)
{
  if (needle.length == 0) {
    return 0;
  }

  if (needle.length > haystack.length) {
    return -1;
  }

  for (size_t i = 0; i < haystack.length - needle.length + 1; i++) {
    if (memcmp(haystack.ptr + i, needle.ptr, needle.length) == 0) {
      return i;
    }
  }

  return -1;
}

void z_str_trim(Z_String *s)
{
  z_str_trim_right(s);
  z_str_trim_left(s);
}

void z_str_trim_cset(Z_String *s, Z_String_View cset)
{
  z_str_trim_right_cset(s, cset);
  z_str_trim_left_cset(s, cset);
}

void z_str_trim_right(Z_String *s)
{
  z_str_trim_right_cset(s, z_sv_from_cstr(Z__WHITE_SPACE));
}

void z_str_trim_left(Z_String *s)
{
  z_str_trim_left_cset(s, z_sv_from_cstr(Z__WHITE_SPACE));
}

void z_str_trim_right_cset(Z_String *s, Z_String_View cset)
{
  Z_String_View trimmed = z_sv_trim_right_cset(z_sv_from_str(s), cset);
  memmove(s->ptr, trimmed.ptr, trimmed.length);
  s->length = trimmed.length;
  z_array_zero_terminate(s);
}

void z_str_trim_left_cset(Z_String *s, Z_String_View cset)
{
  Z_String_View trimmed = z_sv_trim_left_cset(z_sv_from_str(s), cset);
  memmove(s->ptr, trimmed.ptr, trimmed.length);
  s->length = trimmed.length;
  z_array_zero_terminate(s);
}

Z_String_View z_sv_trim(Z_String_View s)
{
  return z_sv_trim_cset(s, z_sv_from_cstr(Z__WHITE_SPACE));
}

Z_String_View z_sv_trim_cset(Z_String_View s, Z_String_View cset)
{
  Z_String_View trimmed = z_sv_trim_right_cset(s, cset);
  return z_sv_trim_left_cset(trimmed, cset);
}

Z_String_View z_sv_trim_right(Z_String_View s)
{
  return z_sv_trim_right_cset(s, z_sv_from_cstr(Z__WHITE_SPACE));
}

Z_String_View z_sv_trim_right_cset(Z_String_View s, Z_String_View cset)
{
  Z_String_View trimmed = s;

  while (trimmed.length > 0 && z_sv_contain_char(cset, z_sv_peek(trimmed))) {
    trimmed.length--;
  }

  return trimmed;
}

Z_String_View z_sv_trim_left(Z_String_View s)
{
  return z_sv_trim_left_cset(s, z_sv_from_cstr(Z__WHITE_SPACE));
}

Z_String_View z_sv_trim_left_cset(Z_String_View s, Z_String_View cset)
{
  Z_String_View trimmed = s;

  while (trimmed.length > 0 && z_sv_contain_char(cset, s.ptr[0])) {
    trimmed = z_sv_offset(trimmed, 1);
  }

  return trimmed;
}

void z_sv_print(Z_String_View s)
{
  printf("%.*s", z__size_t_to_int(s.length), s.ptr);
}

void z_sv_println(Z_String_View s)
{
  printf("%.*s\n", z__size_t_to_int(s.length), s.ptr);
}

void z_str_clear(Z_String *s)
{
  s->length = 0;
  z_array_zero_terminate(s);
}

bool z_write_file(const char *pathname, const char *format, ...)
{
  FILE *fp = fopen(pathname, "w");

  if (fp == NULL) {
    return false;
  }

  va_list args;
  va_start(args, format);
  vfprintf(fp, format, args);
  va_end(args);

  return true;
}

bool z_append_file(const char *pathname, const char *format, ...)
{
  FILE *fp = fopen(pathname, "a");

  if (fp == NULL) {
    return false;
  }

  va_list args;
  va_start(args, format);
  vfprintf(fp, format, args);
  va_end(args);

  return true;
}

bool z_scanf_file(const char *pathname, const char *format, ...)
{
  FILE *fp = fopen(pathname, "r");

  if (fp == NULL) {
    return false;
  }

  va_list args;
  va_start(args, format);

  if (vfscanf(fp, format, args) == EOF) {
    va_end(args);
    fclose(fp);
    return false;
  }

  va_end(args);
  fclose(fp);

  return true;
}

Z_Maybe_String z_read_file(Z_Heap *heap, const char *pathname)
{
  FILE *fp = fopen(pathname, "r");

  if (fp == NULL) {
    return (Z_Maybe_String){ .ok = false };
  }

  Z_String content = z_str_new(heap, "");
  size_t file_size = z__get_file_size(fp);

  z_array_ensure_capacity(&content, file_size);
  content.length = fread(content.ptr, sizeof(char), file_size, fp);
  z_array_zero_terminate(&content);
  fclose(fp);

  return (Z_Maybe_String){ .ok = true, .value = content };
}

Z_Maybe_String_Array z_read_directory(Z_Heap *heap, const char *pathname)
{
  DIR *directory = opendir(pathname);

  if (directory == NULL) {
    return (Z_Maybe_String_Array){ .ok = false };
  }

  Z_String_Array entries = z_array_new(heap, Z_String_Array);
  struct dirent *directory_entry;

  while ((directory_entry = readdir(directory))) {
    z_array_push(&entries, z_str_new(heap, "%s", directory_entry->d_name));
  }

  closedir(directory);

  return (Z_Maybe_String_Array){ .ok = true, .value = entries };
}

Z_String z_expand_tilde(Z_Heap *heap, Z_String_View pathname)
{
  if (z_sv_starts_with(pathname, z_sv_from_cstr("~"))) {
    Z_String expanded = z_str_new(heap, "%s", z_try_get_env("HOME", "."));
    z_str_append_str(&expanded, z_sv_offset(pathname, 1));
    return expanded;
  }

  return z_str_new_from(heap, pathname);
}

Z_String z_compress_tilde(Z_Heap *heap, Z_String_View pathname)
{
  const char *home = z_try_get_env("HOME", NULL);

  if (home && z_sv_starts_with(pathname, z_sv_from_cstr(home))) {
    Z_String compressed = z_str_new(heap, "~");
    z_str_append_str(&compressed, z_sv_offset(pathname, strlen(home)));
    return compressed;
  }

  return z_str_new_from(heap, pathname);
}

const char *z_try_get_env(const char *name, const char *fallback)
{
  const char *value = getenv(name);
  return value ? value : fallback;
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

Z_Map z_map_new(Z_Heap *heap, Z_Compare_Fn compare_keys)
{
  Z_Map map = {
    .heap = heap,
    .root = NULL,
    .compare_keys = compare_keys,
    .size = 0,
  };

  return map;
}

size_t z_map_size(const Z_Map *map)
{
  return map->size;
}

Z_Key_Value z_map_put(Z_Map *map, void *key, void *value)
{
  Z_Key_Value old_key_value = z__avl_put(map->heap, &map->root, key, value, map->compare_keys);

  if (old_key_value.key) {
    map->size++;
  }

  return old_key_value;
}

void *z_map_get(const Z_Map *map, const void *key)
{
  return z__avl_get(map->root, key, map->compare_keys);
}

void *z_map_try_get(const Z_Map *map, const void *key, const void *fallback)
{
  return z__avl_try_get(map->root, key, map->compare_keys, fallback);
}

bool z_map_has(const Z_Map *map, void *key)
{
  return z__avl_has(map->root, key, map->compare_keys);
}

Z_Key_Value z_map_delete(Z_Map *map, void *key)
{
  Z_Key_Value old_key_value = z__avl_remove(map->heap, &map->root, key, map->compare_keys);

  if (old_key_value.key) {
    map->size--;
  }

  return old_key_value;
}

Z_Key_Value_Array z_map_to_array(Z_Heap *heap, Z_Map *map)
{
  return z__avl_to_array(heap, map->root);
}

void z_map_print(const Z_Map *map, Z_Print_Fn print_key, Z_Print_Fn print_value)
{
  z__avl_print(map->root, print_key, print_value);
}

Z_Set z_set_new(Z_Heap *heap, Z_Compare_Fn compare_elements)
{
  return z_map_new(heap, compare_elements);
}

size_t z_set_size(const Z_Set *set)
{
  return z_map_size(set);
}

void *z_set_add(Z_Set *set, void *element)
{
  Z_Key_Value old_key_value = z_map_put(set, element, NULL);
  return old_key_value.key;
}

bool z_set_has(const Z_Set *set, void *element)
{
  return z_map_has(set, element);
}

void *z_set_remove(Z_Set *set, void *element)
{
  Z_Key_Value deleted = z_map_delete(set, element);
  return deleted.key;
}

void z__set_print_nothing(const void *)
{ }

void z_set_print(const Z_Set *set, Z_Print_Fn print_element)
{
  z_map_print(set, print_element, z__set_print_nothing);
}

int z_compare_int_pointers(const int *a, const int *b)
{
  return (*a > *b) - (*a < *b);
}

int z_compare_float_pointers(const float *a, const float *b)
{
  return (*a > *b) - (*a < *b);
}

int z_compare_double_pointers(const double *a, const double *b)
{
  return (*a > *b) - (*a < *b);
}

int z_compare_string_pointers(const char **a, const char **b)
{
  return strcmp(*a, *b);
}

void z_print_int_pointer(const int *a)
{
  printf("%d", *a);
}

void z_print_float_pointer(const float *a)
{
  printf("%f", *a);
}

void z_print_double_pointer(const double *a)
{
  printf("%lf", *a);
}

void z_print_string(const char *a)
{
  printf("%s", a);
}

void z_print_string_with_double_quotes(const char *a)
{
  printf("\"%s\"", a);
}

void z_print_string_pointer(const char **a)
{
  printf("%s", *a);
}

#define Z_PTR_TABLE_MIN_CAPACITY 16
#define Z_PTR_TABLE_GROWTH_RATE 2
#define Z_PTR_TABLE_MAX_LOAD_FACTOR 0.7
#define Z_PTR_TABLE_TOMBSTONE ((void*)1)

bool z__pointer_table_can_insert(void *slot)
{
  return slot == NULL || slot == Z_PTR_TABLE_TOMBSTONE;
}

void **z__pointer_table_find_slot_for_insert(const Z_Pointer_Table *table, const void *pointer)
{
  if (table->capacity == 0) {
    return NULL;
  }

  size_t i = (uintptr_t)pointer % table->capacity;
  
  while (!z__pointer_table_can_insert(table->pointers[i]) && table->pointers[i] != pointer) {
    i = (i + 1) % table->capacity;
  }

  return &table->pointers[i];
}

void **z__pointer_table_find_slot(const Z_Pointer_Table *table, const void *pointer)
{
  if (table->capacity == 0) {
    return NULL;
  }

  size_t i = (uintptr_t)pointer % table->capacity;
  
  while (table->pointers[i] && table->pointers[i] != pointer) {
    i = (i + 1) % table->capacity;
  }

  return &table->pointers[i];
}

float z_ptr_table_load_factor(const Z_Pointer_Table *table)
{
  return (float)table->occupied / (float)table->capacity;
}

void z__pointer_table_resize(Z_Pointer_Table *table, size_t new_capacity)
{
  Z_Pointer_Table new_table = {
    .pointers = calloc(new_capacity, sizeof(void *)),
    .capacity = new_capacity,
  };

  for (size_t i = 0; i < table->capacity; i++) {
    if (table->pointers[i] && table->pointers[i] != Z_PTR_TABLE_TOMBSTONE) {
      void **slot = z__pointer_table_find_slot_for_insert(&new_table, table->pointers[i]);
      *slot = table->pointers[i];
      new_table.occupied++;
    }
  }

  free(table->pointers);
  *table = new_table;
}

void z__pointer_table_insert(Z_Pointer_Table *table, void *pointer)
{
  if (table->capacity == 0 || z_ptr_table_load_factor(table) >= Z_PTR_TABLE_MAX_LOAD_FACTOR) {
    size_t new_capacity = z__max_size_t(Z_PTR_TABLE_MIN_CAPACITY, table->capacity * Z_PTR_TABLE_GROWTH_RATE);
    z__pointer_table_resize(table, new_capacity);
  }

  void **slot =  z__pointer_table_find_slot_for_insert(table, pointer);

  if (*slot == NULL) {
    table->occupied++;
  }

  *slot = pointer;
}

void z__pointer_table_delete(Z_Pointer_Table *table, const void *pointer)
{
  void **slot = z__pointer_table_find_slot(table, pointer);
  *slot = Z_PTR_TABLE_TOMBSTONE;
}

void z__pointer_table_foreach(const Z_Pointer_Table *table, void callback(void *))
{
  for (size_t i = 0; i < table->capacity; i++) {
    if (table->pointers[i] && table->pointers[i] != Z_PTR_TABLE_TOMBSTONE) {
      callback(table->pointers[i]);
    }
  }
}

void z__pointer_table_free(Z_Pointer_Table *table)
{
  free(table->pointers);
}

void *z_heap_malloc(Z_Heap *heap, size_t size)
{
  void *pointer = malloc(size);
  z__pointer_table_insert(&heap->table, pointer);
  return pointer;
}

void *z_heap_calloc(Z_Heap *heap, size_t size)
{
  void *pointer = calloc(size, sizeof(char));
  z__pointer_table_insert(&heap->table, pointer);
  return pointer;
}

void *z_heap_realloc(Z_Heap *heap, void *pointer, size_t new_size)
{
  if (pointer == NULL) {
    return z_heap_malloc(heap, new_size);
  }

  void *new_pointer = realloc(pointer, new_size);

  if (new_pointer != pointer) {
    z__pointer_table_delete(&heap->table, pointer);
    z__pointer_table_insert(&heap->table, new_pointer); 
  }

  return new_pointer;
}

void z_heap_free_pointer(Z_Heap *heap, void *pointer)
{
  if (pointer == NULL) {
    return;
  }

  free(pointer);
  z__pointer_table_delete(&heap->table, pointer);
}

void z_heap_free(Z_Heap *heap)
{
  z__pointer_table_foreach(&heap->table, free);
  z__pointer_table_free(&heap->table);
}

Z_Clock z_get_clock()
{
  return clock();
}

void z_print_elapsed_seconds(Z_Clock start)
{
  double elapsed_seconds = ((double)(z_get_clock() - start)) / CLOCKS_PER_SEC;
  printf("%lfs\n", elapsed_seconds);
}

Z_Deque z_deque_new()
{
  Z_Deque deque = {
    .capacity = 0,
    .length = 0,
    .front = 0,
    .rear = 0,
    .ptr = NULL,
  };

  return deque;
}

size_t z_deque_next_index(const Z_Deque *deque, size_t i)
{
  return (i + 1) % deque->capacity;
}

size_t z_deque_previous_index(const Z_Deque *deque, size_t i)
{
  return i == 0 ? deque->capacity - 1 : i - 1;
}

int z_deque_at(const Z_Deque *deque, size_t i)
{
  assert(i < deque->length);
  size_t wrapped_index = (deque->front + i) % deque->capacity;
  return deque->ptr[wrapped_index];
}

bool z_deque_is_index_inside(const Z_Deque *deque, size_t i)
{
  if (deque->front < deque->rear) {
    return deque->front <= i && deque->rear <= i;
  }

  return deque->front <= i || i <= deque->rear;
}

void z_deque_debug_print(const Z_Deque *deque)
{
  if (deque->capacity == 0) {
    printf("[]\n");
    return;
  }

  printf("[ ");
  for (size_t i = 0; i < deque->capacity - 1; i++) {
    if (z_deque_is_index_inside(deque, i)) {
      printf("%d, ", deque->ptr[i]);
    } else {
      printf("-, ");
    }
  }

  if (z_deque_is_index_inside(deque, deque->capacity - 1)) {
      printf("%d ", deque->ptr[deque->capacity - 1]);
  } else {
      printf("- ");
  }
  printf("]\n");
}

void z_deque_ensure_capacity(Z_Deque *deque, size_t needed)
{
  if (deque->capacity < needed) {
    size_t new_capacity = z__max_size_t(needed, deque->capacity * Z_GROWTH_RATE);
    deque->ptr = realloc(deque->ptr, sizeof(int) * new_capacity);
    if (deque->front > deque->rear) {

      int *buf = malloc(sizeof(int) * deque->length);
      memcpy(buf, deque->ptr + deque->front, sizeof(int) * (deque->capacity - deque->front));
      memcpy(buf + deque->capacity - deque->front, deque->ptr, sizeof(int) * (deque->rear + 1));
      memcpy(deque->ptr, buf, sizeof(int) * deque->length);
      free(buf);

      deque->front = 0;
      deque->rear = deque->length - 1;
    }

    deque->capacity = new_capacity;
  }
}

void z_deque_push_back(Z_Deque *deque, int element)
{
  z_deque_ensure_capacity(deque, deque->length + 1);
  deque->rear = z_deque_next_index(deque, deque->rear);
  deque->ptr[deque->rear] = element;
  deque->length++;
}

void z_deque_push_front(Z_Deque *deque, int element)
{
  z_deque_ensure_capacity(deque, deque->length + 1);
  deque->front = z_deque_previous_index(deque, deque->front);
  deque->ptr[deque->front] = element;
  deque->length++;
}

int z_deque_pop_back(Z_Deque *deque)
{
  int element = deque->ptr[deque->rear];
  deque->rear = z_deque_previous_index(deque, deque->rear);
  deque->length--;
  return element;
}

int z_deque_pop_front(Z_Deque *deque)
{
  int element = deque->ptr[deque->front];
  deque->front = z_deque_next_index(deque, deque->front);
  deque->length--;
  return element;
}