#include "zlib.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <assert.h>

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

void *z_memory_duplicate(const void *memory, size_t size)
{
  void *new_memory = malloc(size);
  memcpy(new_memory, memory, size);
  return new_memory;
}

Z_Array_Header *z__array_header(void *array)
{
  return array ? ((Z_Array_Header *)array) - 1 : NULL;
}

void *z__array_from_header(Z_Array_Header *header)
{
  return header + 1;
}

size_t z__array_length(void *array)
{
  return array ? z__array_header(array)->length : 0;
}

size_t z__array_capacity(void *array)
{
  return array ? z__array_header(array)->capacity : 0;
}

void z__array_ensure_capacity(void **array, size_t capacity, size_t element_size)
{
  if (*array == NULL) {
    Z_Array_Header *header = malloc(sizeof(Z_Array_Header) + capacity * element_size);
    header->capacity = capacity;
    header->length = 0;
    *array = z__array_from_header(header);
    return;
  }

  Z_Array_Header *header = z__array_header(*array);

  if (header->capacity < capacity) {
    size_t new_capacity = z__max_size_t(capacity, header->capacity * Z_DEFAULT_GROWTH_RATE);
    header = realloc(header, sizeof(Z_Array_Header) + new_capacity * element_size);
    header->capacity = new_capacity;
    *array = z__array_from_header(header);
  }
}

void z__array_free(void **array)
{
  if (*array == NULL) {
    return;
  }

  free(z__array_header(*array));
  *array = NULL;
}

void *z__array_end(void *array, size_t element_size)
{
  return ((char *)array) + z__array_length(array) * element_size;
}

void z__array_null_terminate(void **array, size_t element_size)
{
  z__array_ensure_capacity(array, z__array_length(*array) + 1, element_size);
  memset(z__array_end(*array, element_size), 0, element_size);
}

void z__array_push(void **array, const void *element, size_t element_size)
{
   z__array_ensure_capacity(array, z__array_length(*array) + 1, element_size);
   memcpy(z__array_end(*array, element_size), element, element_size);
   z__array_header(*array)->length++;
}

size_t z__get_format_length(const char *format, va_list args)
{
  va_list args_copy;
  va_copy(args_copy, args);

  size_t size = vsnprintf(NULL, 0, format, args_copy);
  va_end(args_copy);

  return size;
}

Z_Char *z_str_new(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  Z_Char *s = z_str_new_args(format, args);
  va_end(args);
  return s;
}

Z_Char *z_str_new_args(const char *format, va_list args)
{
  Z_Char *s = NULL;
  z_str_append_args(&s, format, args);
  return s;
}

Z_Char *z_str_new_from(Z_String_View s)
{
  return z_str_new("%.*s", z__size_t_to_int(s.length), s.ptr);
}

void z_str_append(Z_Char **s, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  z_str_append_args(s, format, args);
  va_end(args);
}

void z_str_append_args(Z_Char **s, const char *format, va_list args)
{
  size_t format_length = z__get_format_length(format, args);
  z__array_ensure_capacity((void **)s, z_array_length(*s) + format_length + 1, sizeof(char));

  va_list args_copy;
  va_copy(args_copy, args);
  vsnprintf(*s + z_array_length(*s), format_length + 1, format, args_copy);
  va_end(args_copy);

  z__array_header(*s)->length = z_array_length(*s) + format_length;
}

void z_str_append_str(Z_Char **target, Z_String_View source)
{
  z_str_append(target, "%.*s", z__size_t_to_int(source.length), source.ptr);
}

void z_str_append_char(Z_Char **s, char c)
{
  z_str_append(s, "%c", c);
}

void z_str_prepend(Z_Char **s, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  z_str_prepend_args(s, format, args);
  va_end(args);
}

void z_str_prepend_args(Z_Char **s, const char *format, va_list args)
{
  Z_Char *tmp = z_str_new_args(format, args);
  z_str_append(&tmp, "%s", *s);
  z_str_free(s);
  *s = tmp;
}

void z_str_prepend_str(Z_Char **target, Z_String_View source)
{
  z_str_prepend(target, "%.*s", z__size_t_to_int(source.length), source.ptr);
}

void z_str_prepend_char(Z_Char **s, char c)
{
  z_str_prepend(s, "%c", c);
}

char z_str_pop_char(Z_Char **s)
{
  char c = z_array_pop(s);
  z_array_null_terminate(s);
  return c;
}

void z_str_set(Z_Char **s, const char *format, ...)
{
  z_str_clear(s);
  va_list args;
  va_start(args, format);
  z_str_append_args(s, format, args);
  va_end(args);
}

void z_str_set_str(Z_Char **s, Z_String_View str)
{
  z_str_set(s, "%.*s", z__size_t_to_int(str.length), str.ptr);
}

void z_str_replace(Z_Char **s, Z_String_View target, Z_String_View replacement)
{
  Z_Char *tmp = z_str_new("");

  size_t i = 0;

  while (i < z_str_length(*s)) {
    if (z_sv_equal(z_sv_offset(z_sv(*s), i), target)) {
      z_str_append_str(&tmp, replacement);
      i += replacement.length;
    } else {
      z_str_append_char(&tmp, (*s)[i]);
      i++;
    }
  }

  z_str_free(s);
  *s = tmp;
}

Z_Char *z_str_join(Z_Char **s, Z_String_View delimiter)
{
  if (z_array_length(s) == 0) {
    return z_str_new("");
  }

  Z_Char *result = z_str_new("");

  for (size_t i = 0; i < z_array_length(s) - 1; i++) {
    z_str_append_str(&result, z_sv(s[i]));
    z_str_append_str(&result, delimiter);
  }

  z_str_append_str(&result, z_sv(s[z_array_length(s) - 1]));
  return result;
}

Z_Char **z_str_split(Z_String_View s, Z_String_View delimiter)
{
  if (delimiter.length == 0) {
    return NULL;
  }

  Z_Char **result = NULL;

  ssize_t offset = 0;
  ssize_t length = 0;

  while ((length = z_sv_find_index(z_sv_offset(s, offset), delimiter)) != -1) {
    Z_String_View slice = z_sv_substring(s, offset, offset + length);
    z_array_push(&result, z_str_new_from(slice));
    offset += length + delimiter.length;
  }

  z_array_push(&result, z_str_new_from(z_sv_substring(s, offset, s.length)));

  return result;
}

size_t z_str_length(Z_Char *s)
{
  return z_array_length(s);
}

size_t z_sv_length(Z_String_View s)
{
  return s.length;
}

Z_String_View z_sv(const char *s)
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

void z_str_trim(Z_Char **s)
{
  z_str_trim_right(s);
  z_str_trim_left(s);
}

void z_str_trim_cset(Z_Char **s, Z_String_View cset)
{
  z_str_trim_right_cset(s, cset);
  z_str_trim_left_cset(s, cset);
}

void z_str_trim_right(Z_Char **s)
{
  z_str_trim_right_cset(s, z_sv(Z__WHITE_SPACE));
}

void z_str_trim_left(Z_Char **s)
{
  z_str_trim_left_cset(s, z_sv(Z__WHITE_SPACE));
}

void z_str_trim_right_cset(Z_Char **s, Z_String_View cset)
{
  Z_String_View trimmed = z_sv_trim_right_cset(z_sv(*s), cset);
  z_str_set_str(s, trimmed);
}

void z_str_trim_left_cset(Z_Char **s, Z_String_View cset)
{
  Z_String_View trimmed = z_sv_trim_left_cset(z_sv(*s), cset);
  z_str_set_str(s, trimmed);
}

Z_String_View z_sv_trim(Z_String_View s)
{
  return z_sv_trim_cset(s, z_sv(Z__WHITE_SPACE));
}

Z_String_View z_sv_trim_cset(Z_String_View s, Z_String_View cset)
{
  Z_String_View trimmed = z_sv_trim_right_cset(s, cset);
  return z_sv_trim_left_cset(trimmed, cset);
}

Z_String_View z_sv_trim_right(Z_String_View s)
{
  return z_sv_trim_right_cset(s, z_sv(Z__WHITE_SPACE));
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
  return z_sv_trim_left_cset(s, z_sv(Z__WHITE_SPACE));
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

void z_str_free(Z_Char **s)
{
  z_array_free(s);
}

void z_str_array_free(Z_Char ***s)
{
  z_array_foreach_ptr(*s, z_str_free);
  z_array_free(s);
}

void z_str_clear(Z_Char **s)
{
  z__array_header(*s)->length = 0;
  z_array_null_terminate(s);
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

Z_Char *z_read_file(const char *pathname)
{
  FILE *fp = fopen(pathname, "r");

  if (fp == NULL) {
    return NULL;
  }

  Z_Char *content = NULL;
  int file_size = z__get_file_size(fp);

  z__array_ensure_capacity((void **)&content, file_size, sizeof(char));
  z__array_header(content)->length = fread(content, sizeof(char), file_size, fp);
  z_array_null_terminate(&content);
  fclose(fp);

  return content;
}

Z_Char **z_read_directory(const char *pathname)
{
  DIR *directory = opendir(pathname);

  if (directory == NULL) {
    return NULL;
  }

  Z_Char **entries = NULL;
  struct dirent *directory_entry;

  while ((directory_entry = readdir(directory))) {
    z_array_push(&entries, z_str_new("%s", directory_entry->d_name));
  }

  closedir(directory);

  return entries;
}

Z_Char *z_expand_tilde(Z_String_View pathname)
{
  if (z_sv_starts_with(pathname, z_sv("~"))) {
    Z_Char *expanded = z_str_new("%s", z_get_env("HOME", "."));
    z_str_append_str(&expanded, z_sv_offset(pathname, 1));
    return expanded;
  }

  return z_str_new_from(pathname);
}

Z_Char *z_compress_tilde(Z_String_View pathname)
{
  const char *home = z_get_env("HOME", NULL);

  if (home && z_sv_starts_with(pathname, z_sv(home))) {
    Z_Char *compressed = z_str_new("~");
    z_str_append_str(&compressed, z_sv_offset(pathname, strlen(home)));
    return compressed;
  }

  return z_str_new_from(pathname);
}

const char *z_get_env(const char *name, const char *fallback)
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

void z__avl_rebalance_node(Z_Avl_Node **node, Z_Compare_Fn compare_keys)
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

Z_Avl_Node *z__avl_new(void *key, void *value)
{
  Z_Avl_Node *n = malloc(sizeof(Z_Avl_Node));
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

void *z__avl_get(Z_Avl_Node *root, const void *key, Z_Compare_Fn compare_keys)
{
  Z_Avl_Node *node = z__avl_get_node(root, key, compare_keys);
  return node ? node->value : NULL;
}

void z__avl_set_node_key_value(Z_Avl_Node *node, void *key, void *value,
                     void free_key(void *), void free_value(void *))
{
  if (free_value) {
    free_value((*root)->value);
  }

  if (free_key) {
    free_key(key);
  }

  node->value = value;
  node->key = key;
}

void z__avl_free_node(Z_Avl_Node *node, void free_key(void *), void free_value(void *))
{
  if (free_value) {
    free_value((*root)->value);
  }

  if (free_key) {
    free_key(key);
  }

  free(node);
}

void z__avl_put(Z_Avl_Node **root, void *key, void *value,
                Z_Compare_Fn compare_keys, void free_key(void *),
                void free_value(void *))
{
  if (*root == NULL) {
    *root = z__avl_new(key, value);
    return;
  }

  int compare_result = compare_keys(key, (*root)->key);

  if (compare_result == 0) {
    z__avl_set_node_key_value(*root, key, value);
    return;
  }

  if (compare_result > 0) {
    z__avl_put(&(*root)->right, key, value, compare_keys, free_key, free_value);
  } else {
    z__avl_put(&(*root)->left, key, value, compare_keys, free_key, free_value);
  }

  z__avl_rebalance_node(root, compare_keys);
}

void z__avl_remove(Z_Avl_Node **root, void *key, Z_Compare_Fn compare_keys,
                   void free_key(void *), void free_value(void *))
{
  if (*root == NULL) {
    return;
  }

  int compare_result = compare_keys(key, (*root)->key);

  if (compare_result > 0) {
    z__avl_remove(&((*root)->right), key, compare_keys, free_key, free_value);
    return;
  }

  if (compare_result < 0) {
    z__avl_remove(&((*root)->left), key, compare_keys, free_key, free_value);
    return;
  }

  if ((*root)->left == NULL || (*root)->right == NULL) {
    Z_Avl_Node *tmp = (*root)->left ? (*root)->left : (*root)->right;
    z__avl_free_node(*root, free_key, free_value);
    *root = tmp;
    return;
  }

  Z_Avl_Node *succesor = z__avl_get_min((*root)->right);
  z__avl_set_node_key_value(*root, succesor->key, succesor->value, free_key, free_value);
  z__avl_remove(&((*root)->right), succesor->key, compare_keys, NULL, NULL);
  z__avl_rebalance_node(root, compare_keys);
}

void z__avl_foreach(Z_Avl_Node *root,
                    void callback(void *key, void *value, void *context),
                    void *context)
{
  if (root == NULL) {
    return;
  }

  z__avl_foreach(root->left, callback, context);
  callback(root->key, root->value, context);
  z__avl_foreach(root->right, callback, context);
}

void z__avl_print(Z_Avl_Node *root,
                 void print_entry(void *key, void *value, void *context), void *context,
                 int padding)
{
  if (root == NULL) {
    return;
  }

  printf("%*c", padding, ' ');
  print_entry(root->key, root->value, context);
  z__avl_print(root->right, print_entry, context, padding + 4);
  z__avl_print(root->left, print_entry, context, padding + 4);
}

void z__avl_free(Z_Avl_Node *root, void free_key(void *), void free_value(void *))
{
  if (root == NULL) {
    return;
  }

  if (free_key) free_key(root->key);
  if (free_value) free_value(root->value);

  z__avl_free(root->left, free_key, free_value);
  z__avl_free(root->right, free_key, free_value);
  free(root);
}

Z_Map *z_map_new(Z_Compare_Fn compare_keys)
{
  Z_Map *map = malloc(sizeof(Z_Map));
  map->root = NULL;
  map->compare_keys = compare_keys;

  return map;
}

void z_map_put(Z_Map *map, void *key, void *value, void free_key(void *), void free_value(void *))
{
  z__avl_put(&map->root, key, value, map->compare_keys, free_key, free_value);
}

void *z_map_get(const Z_Map *map, const void *key)
{
  return z__avl_get(map->root, key, map->compare_keys);
}

bool z_map_has(const Z_Map *map, void *key)
{
  return z__avl_has(map->root, key, map->compare_keys);
}

void z_map_delete(Z_Map *map, void *key, void free_key(void *), void free_value(void *))
{
  z__avl_remove(&map->root, key, map->compare_keys, free_key, free_value);
}

void z_map_foreach(const Z_Map *map, void callback(void *key, void *value, void *context), void *context)
{
  z__avl_foreach(map->root, callback, context);
}

void z_map_free(Z_Map *map, void free_key(void *), void free_value(void *))
{
  z__avl_free(map->root, free_key, free_value);
  free(map);
}

Z_Dictionary *z_dictionary_new()
{
  return z_map_new((Z_Compare_Fn)strcmp);
}

void z_dictionary_put(Z_Dictionary *dictionary, const char *key, void *value, void free_value(void *))
{
  z_map_put(dictionary, strdup(key), value, free, free_value);
}

void *z_dictionary_get(const Z_Dictionary *dictionary, const char *key)
{
  return z_map_get(dictionary, key);
}

bool z_dictionary_has(const Z_Dictionary *dictionary, const char *key)
{
  z_map_has(dictionary, (void*)key);
}

void z_dictionary_delete(Z_Dictionary *dictionary, const char *key, void free_value(void *))
{
  z_map_delete(dictionary, (void*)key, free, free_value);
}

void z_dictionary_foreach(const Z_Dictionary *dictionary, void callback(const char *key, void *value, void *context), void *context)
{
  z_map_foreach(dictionary, (void (*)(void *, void *, void *))callback, context);
}

void z_dictionary_free(Z_Dictionary *dictionary, void free_value(void *))
{
  z_map_free(dictionary, free, free_value);
}

Z_Set *z_set_new(Z_Compare_Fn compare)
{
  return z_map_new(compare);
}

void z_set_put(Z_Set *set, void *element, void free_element(void *))
{
  z_map_put(set, element, NULL, free_element, NULL);
}

bool z_set_has(const Z_Set *set, const void *element)
{
  z_map_has(set, (void*)element);
}

void z_set_delete(Z_Set *set, const void *element, void free_element(void *))
{
  z_map_delete(set, (void*)element, free_element, NULL);
}

void z_set_free(Z_Set *set, void free_element(void *))
{
  z_map_free(set, free_element, NULL);
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

size_t z__deque_next_index(Z_Deque *deque, size_t i)
{
  return (i + 1) % deque->capacity;
}

size_t z__deque_previous_index(Z_Deque *deque, size_t i)
{
  return (i + deque->capacity - 1) % deque->capacity;
}

void z__deque_copy_to_array(Z_Deque *deque, void **ptr)
{
  for (size_t i = 0; i < z_deque_length(deque); i++) {
    ptr[i] = *z_deque_at(deque, i);
  }
}

void z__deque_ensure_capacity(Z_Deque *deque, size_t capacity)
{
  if (deque->capacity >= capacity + 1) {
    return;
  }

  size_t length = z_deque_length(deque);
  size_t new_capacity = z__max_size_t(capacity + 1, deque->capacity * Z_DEFAULT_GROWTH_RATE);
  void **new_ptr = malloc(sizeof(void *) * new_capacity);

  z__deque_copy_to_array(deque, new_ptr);

  free(deque->ptr);
  deque->ptr = new_ptr;
  deque->capacity = new_capacity;
  deque->front = 0;
  deque->rear = length;
}

Z_Deque *z_deque_new()
{
  Z_Deque *deque = malloc(sizeof(Z_Deque));
  deque->ptr = NULL;
  deque->capacity = 0;
  deque->front = 0;
  deque->rear = 0;

  return deque;
}

void z_deque_push_back(Z_Deque *deque, void *element)
{
  z__deque_ensure_capacity(deque, z_deque_length(deque) + 1);
  deque->ptr[deque->rear] = element;
  deque->rear = z__deque_next_index(deque, deque->rear);
}

void z_deque_push_front(Z_Deque *deque, void *element)
{
  z__deque_ensure_capacity(deque, z_deque_length(deque) + 1);
  deque->front = z__deque_previous_index(deque, deque->front);
  deque->ptr[deque->front] = element;
}

void *z_deque_pop_back(Z_Deque *deque)
{
  assert(z_deque_length(deque) > 0);
  deque->rear = z__deque_previous_index(deque, deque->rear);
  return deque->ptr[deque->rear];
}

void *z_deque_pop_front(Z_Deque *deque)
{
  assert(z_deque_length(deque) > 0);
  deque->front = z__deque_next_index(deque, deque->front);
  return deque->ptr[z__deque_previous_index(deque, deque->front)];
}

size_t z_deque_length(const Z_Deque *deque)
{
  if (deque->capacity == 0) {
    return 0;
  }

  return (deque->rear + deque->capacity - deque->front) % deque->capacity;
}

void **z_deque_at(const Z_Deque *deque, size_t i)
{
  assert(i < z_deque_length(deque));
  return &deque->ptr[(i + deque->front) % deque->capacity];
}
