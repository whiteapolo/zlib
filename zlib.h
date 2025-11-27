#ifndef ZLIB_H
#define ZLIB_H

#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

//----------------------------------------------------
//
// Internal
//
//----------------------------------------------------

typedef int (*Z_Compare_Fn)(const void *, const void *);
typedef void (*Z_Free_Fn)(void *);
typedef void *(*Z_Clone_Fn)(void *);
typedef void (*Z_Print_Fn)(const void *);

typedef struct {
  size_t capacity;
  size_t length;
} Z_Array_Header;

typedef struct Z_Avl_Node {
  struct Z_Avl_Node *left;
  struct Z_Avl_Node *right;
  void *key;
  void *value;
  char height;
} Z_Avl_Node;

typedef enum {
  Z_PUT_RESULT_INSERTED,
  Z_PUT_RESULT_REPLACED,
} Z_Put_Result;

typedef struct {
  Z_Compare_Fn compare_keys;
  Z_Clone_Fn clone_key;
  Z_Clone_Fn clone_value;
  Z_Free_Fn free_key;
  Z_Free_Fn free_value;
} Z_Map_Handlers;

typedef struct {
  Z_Compare_Fn compare_elements;
  Z_Clone_Fn clone_element;
  Z_Free_Fn free_element;
} Z_Set_Handlers;

typedef struct {
  Z_Avl_Node *root;
  Z_Map_Handlers handlers;
} Z_Map;

typedef struct {
  Z_Avl_Node *root;
  Z_Set_Handlers handlers;
} Z_Set;

typedef char Z_Char;

typedef struct {
  const char *ptr;
  size_t length;
} Z_String_View;

typedef struct {
  void **ptr;
  size_t front;
  size_t rear;
  size_t capacity;
} Z_Deque;

Z_Array_Header *z__array_header(void *array);
size_t z__array_length(void *array);
void z__array_push(void **array, const void *element, size_t element_size);
void z__array_null_terminate(void **array, size_t element_size);
void z__array_free(void **array);

//----------------------------------------------------
//
// API
//
//----------------------------------------------------

void *z_memory_duplicate(const void *memory, size_t size);
#define Z_HEAP_ALLOC(value, Type) z_memory_duplicate(&(Type){value}, sizeof(Type))
#define Z_DEFAULT_GROWTH_RATE 2

#define z_array_push(array_ptr, element) z__array_push((void **)(array_ptr), &(typeof(**(array_ptr))){element}, sizeof(**(array_ptr)))
#define z_array_pop(array_ptr) ((*(array_ptr))[--z__array_header(*(array_ptr))->length])
#define z_array_null_terminate(array_ptr) z__array_null_terminate((void **)(array_ptr), sizeof(**(array_ptr)))
#define z_array_length(array) z__array_length(array)
#define z_array_free(array_ptr) z__array_free((void **)array_ptr)
#define z_array_foreach(array, callback) for (size_t i = 0; i < z_array_length(array); i++) callback((array)[i])
#define z_array_foreach_ptr(array, callback) for (size_t i = 0; i < z_array_length(array); i++) callback(&(array)[i])
#define z_array_sort(array_ptr, compare) qsort(*(array_ptr), z_array_length(*(array_ptr)), sizeof(**(array_ptr)), compare)

Z_Deque *z_deque_new();
void z_deque_push_back(Z_Deque *deque, void *element);
void z_deque_push_front(Z_Deque *deque, void *element);
void *z_deque_pop_back(Z_Deque *deque);
void *z_deque_pop_front(Z_Deque *deque);
size_t z_deque_length(const Z_Deque *deque);
void **z_deque_at(const Z_Deque *deque, size_t i);

Z_Char *z_str_new(const char *format, ...);
Z_Char *z_str_new_args(const char *format, va_list args);
Z_Char *z_str_new_from(Z_String_View s);
void z_str_append(Z_Char **s, const char *format, ...);
void z_str_append_args(Z_Char **s, const char *format, va_list args);
void z_str_append_str(Z_Char **target, Z_String_View source);
void z_str_append_char(Z_Char **s, char c);
void z_str_prepend(Z_Char **s, const char *format, ...);
void z_str_prepend_args(Z_Char **s, const char *format, va_list args);
void z_str_prepend_str(Z_Char **target, Z_String_View source);
void z_str_prepend_char(Z_Char **s, char c);
char z_str_pop_char(Z_Char **s);

void z_str_set(Z_Char **s, const char *format, ...);
void z_str_set_str(Z_Char **s, Z_String_View str);
void z_str_replace(Z_Char **s, Z_String_View target, Z_String_View replacement);
Z_Char *z_str_join(char **s, Z_String_View delimiter);
Z_Char **z_str_split(Z_String_View s, Z_String_View delimiter);
size_t z_str_length(Z_Char *s);
size_t z_sv_length(Z_String_View s);
Z_String_View z_sv(const char *s);
Z_String_View z_sv_offset(Z_String_View s, size_t offset);
Z_String_View z_sv_substring(Z_String_View s, int start, int end);
char z_sv_peek(Z_String_View s);
int z_sv_compare(Z_String_View a, Z_String_View b);
bool z_sv_equal(Z_String_View a, Z_String_View b);
int z_sv_compare_n(Z_String_View a, Z_String_View b, size_t n);
bool z_sv_equal_n(Z_String_View a, Z_String_View b, size_t n);
bool z_sv_ends_with(Z_String_View s, Z_String_View end);
bool z_sv_starts_with(Z_String_View s, Z_String_View start);
bool z_sv_contains(Z_String_View haystack, Z_String_View needle);
bool z_sv_contain_char(Z_String_View s, char c);
ssize_t z_sv_find_index(Z_String_View haystack, Z_String_View needle);
bool z_sv_is_number(Z_String_View s);
int z_sv_to_number(Z_String_View s, int fallback);

// trim hell
void z_str_trim(Z_Char **s);
void z_str_trim_cset(Z_Char **s, Z_String_View cset);
void z_str_trim_right(Z_Char **s);
void z_str_trim_left(Z_Char **s);
void z_str_trim_right_cset(Z_Char **s, Z_String_View cset);
void z_str_trim_left_cset(Z_Char **s, Z_String_View cset);
Z_String_View z_sv_trim(Z_String_View s);
Z_String_View z_sv_trim_cset(Z_String_View s, Z_String_View cset);
Z_String_View z_sv_trim_right(Z_String_View s);
Z_String_View z_sv_trim_right_cset(Z_String_View s, Z_String_View cset);
Z_String_View z_sv_trim_left(Z_String_View s);
Z_String_View z_sv_trim_left_cset(Z_String_View s, Z_String_View cset);

void z_sv_print(Z_String_View s);
void z_sv_println(Z_String_View s);
void z_str_free(Z_Char **s);
void z_str_array_free(Z_Char ***s);
void z_str_clear(Z_Char **s);

bool z_write_file(const char *pathname, const char *format, ...);
bool z_append_file(const char *pathname, const char *format, ...);
bool z_scanf_file(const char *pathname, const char *format, ...);
Z_Char *z_read_file(const char *pathname);
Z_Char **z_read_directory(const char *pathname);
Z_Char *z_expand_tilde(Z_String_View pathname);
Z_Char *z_compress_tilde(Z_String_View pathname);
const char *z_try_get_env(const char *name, const char *fallback);

Z_Map_Handlers z_map_create_handlers(
  Z_Compare_Fn compare_keys,
  Z_Clone_Fn clone_key,        // nullable
  Z_Clone_Fn clone_value,      // nullable
  Z_Free_Fn free_key,          // nullable
  Z_Free_Fn free_value         // nullable
);

// map - map key to value
Z_Map *z_map_new(Z_Map_Handlers handlers);
void z_map_put(Z_Map *map, void *key, void *value);
void *z_map_get(const Z_Map *map, const void *key);
void *z_map_try_get(const Z_Map *map, const void *key, const void *fallback);
bool z_map_has(const Z_Map *map, void *key);
void z_map_delete(Z_Map *map, void *key);
void z_map_foreach(const Z_Map *map, void callback(void *key, void *value, void *context), void *context);
void z_map_print(const Z_Map *map, Z_Print_Fn print_key, Z_Print_Fn print_value);
void z_map_free(Z_Map *map);

Z_Set_Handlers z_set_create_handlers(
  Z_Compare_Fn compare_keys,
  Z_Clone_Fn clone_element,     // nullable
  Z_Free_Fn free_element        // nullable
);

// set - set of elements
Z_Set *z_set_new(Z_Set_Handlers handlers);
void z_set_add(Z_Set *set, void *element);
bool z_set_has(const Z_Set *set, void *element);
void z_set_remove(Z_Set *set, void *element);
void z_set_print(const Z_Set *set, Z_Print_Fn print_element);
void z_set_free(Z_Set *set);

int z_compare_int_pointers(const int *a, const int *b);
int z_compare_float_pointers(const float *a, const float *b);
int z_compare_double_pointers(const double *a, const double *b);
int z_compare_string_pointers(const char **a, const char **b);

void z_print_int_pointer(const int *a);
void z_print_float_pointer(const float *a);
void z_print_double_pointer(const double *a);
void z_print_string(const char *a);
void z_print_string_with_double_quotes(const char *a);
void z_print_string_pointer(const char **a);


#endif
