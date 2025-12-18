#ifndef ZLIB_H
#define ZLIB_H

#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define Z_Heap_Auto __attribute__((cleanup(z_heap_free))) Z_Heap

typedef struct {
  void **pointers;
  size_t occupied;
  size_t capacity;
} Z_Pointer_Table;

typedef struct {
  Z_Pointer_Table table;
} Z_Heap;

typedef int (*Z_Compare_Fn)(const void *, const void *);
typedef void (*Z_Free_Fn)(Z_Heap *, void *);
typedef void *(*Z_Clone_Fn)(Z_Heap *, void *);
typedef void (*Z_Print_Fn)(const void *);

typedef struct Z_Avl_Node {
  struct Z_Avl_Node *left;
  struct Z_Avl_Node *right;
  void *key;
  void *value;
  char height;
} Z_Avl_Node;

typedef struct {
  void *key;
  void *value;
} Z_Key_Value;

typedef struct {
  Z_Heap *heap;
  Z_Key_Value *ptr;
  size_t length;
  size_t capacity;
} Z_Key_Value_Array;

typedef struct {
  Z_Heap *heap;
  Z_Avl_Node *root;
  size_t size;
  Z_Compare_Fn compare_keys;
} Z_Map;

typedef Z_Map Z_Set;

typedef struct {
  Z_Heap *heap;
  char *ptr;
  size_t length;
  size_t capacity;
} Z_String;

typedef struct {
  Z_Heap *heap;
  Z_String *ptr;
  size_t length;
  size_t capacity;
} Z_String_Array;

typedef struct {
  const char *ptr;
  size_t length;
} Z_String_View;

typedef clock_t Z_Clock;

#define Z_DEFAULT_GROWTH_RATE 2

// ============================================================
//                      ARRAY API
// ============================================================

#define z_array_new(heap_ptr, type) ((type){ .heap = heap_ptr, .ptr = NULL, .length = 0, .capacity = 0 })

#define z_array_ensure_capacity(array_ptr, needed)                                                                        \
  do {                                                                                                                    \
    if ((array_ptr)->capacity < (needed)) {                                                                               \
      size_t new_capacity = z__max_size_t((needed), (array_ptr)->capacity * Z_DEFAULT_GROWTH_RATE);                       \
      (array_ptr)->ptr = z_heap_realloc((array_ptr)->heap, (array_ptr)->ptr, sizeof(*(array_ptr)->ptr) * new_capacity);   \
      (array_ptr)->capacity = new_capacity;                                                                               \
    }                                                                                                                     \
  } while (0)

#define z_array_push(array_ptr, element)                           \
  do {                                                             \
    z_array_ensure_capacity(array_ptr, (array_ptr)->length + 1);   \
    (array_ptr)->ptr[(array_ptr)->length++] = element;             \
  } while (0)

#define z_array_push_array(dest_ptr, source_ptr)          \
  do {                                                    \
    for (size_t i = 0; i < (source_ptr)->length; i++) {   \
      z_array_push(dest_ptr, (source_ptr)->ptr[i]);       \
    }                                                     \
  } while (0)

#define z_array_peek(array_ptr)   ((array_ptr)->ptr[(array_ptr)->length - 1])
#define z_array_pop(array_ptr)    ((array_ptr)->ptr[--(array_ptr)->length])

#define z_array_zero_terminate(array_ptr)                                        \
  do {                                                                           \
    z_array_ensure_capacity(array_ptr, (array_ptr)->length + 1);                    \
    memset(&(array_ptr)->ptr[(array_ptr)->length], 0, sizeof(*(array_ptr)->ptr));   \
  } while (0)

// ============================================================
//                        STRING API
// ============================================================

Z_String z_str_new(Z_Heap *heap, const char *format, ...);
Z_String z_str_new_args(Z_Heap *heap, const char *format, va_list args);
Z_String z_str_new_from(Z_Heap *heap, Z_String_View s);

void z_str_append(Z_String *s, const char *format, ...);
void z_str_append_args(Z_String *s, const char *format, va_list args);
void z_str_append_str(Z_String *target, Z_String_View source);
void z_str_append_char(Z_String *s, char c);

void z_str_prepend(Z_String *s, const char *format, ...);
void z_str_prepend_args(Z_String *s, const char *format, va_list args);
void z_str_prepend_str(Z_String *target, Z_String_View source);
void z_str_prepend_char(Z_String *s, char c);

char z_str_pop_char(Z_String *s);
void z_str_replace(Z_String *s, Z_String_View target, Z_String_View replacement);
void z_str_clear(Z_String *s);

Z_String z_str_join(Z_Heap *heap, const Z_String_Array *array, Z_String_View delimiter);
Z_String_Array z_str_split(Z_Heap *heap, Z_String_View s, Z_String_View delimiter);

void z_str_trim(Z_String *s);
void z_str_trim_cset(Z_String *s, Z_String_View cset);
void z_str_trim_right(Z_String *s);
void z_str_trim_left(Z_String *s);
void z_str_trim_right_cset(Z_String *s, Z_String_View cset);
void z_str_trim_left_cset(Z_String *s, Z_String_View cset);

Z_String_View z_sv_from_str(const Z_String *s);
Z_String_View z_sv_from_cstr(const char *s);
Z_String_View z_sv_offset(Z_String_View s, size_t offset);
Z_String_View z_sv_substring(Z_String_View s, int start, int end);

char z_sv_peek(Z_String_View s);
int  z_sv_compare(Z_String_View a, Z_String_View b);
bool z_sv_equal(Z_String_View a, Z_String_View b);
int  z_sv_compare_n(Z_String_View a, Z_String_View b, size_t n);
bool z_sv_equal_n(Z_String_View a, Z_String_View b, size_t n);
bool z_sv_starts_with(Z_String_View s, Z_String_View start);
bool z_sv_ends_with(Z_String_View s, Z_String_View end);
bool z_sv_contains(Z_String_View haystack, Z_String_View needle);
bool z_sv_contain_char(Z_String_View s, char c);
ssize_t z_sv_find_index(Z_String_View haystack, Z_String_View needle);

Z_String_View z_sv_trim(Z_String_View s);
Z_String_View z_sv_trim_cset(Z_String_View s, Z_String_View cset);
Z_String_View z_sv_trim_right(Z_String_View s);
Z_String_View z_sv_trim_right_cset(Z_String_View s, Z_String_View cset);
Z_String_View z_sv_trim_left(Z_String_View s);
Z_String_View z_sv_trim_left_cset(Z_String_View s, Z_String_View cset);

void z_sv_print(Z_String_View s);
void z_sv_println(Z_String_View s);

bool z_write_file(const char *pathname, const char *format, ...);
bool z_append_file(const char *pathname, const char *format, ...);
bool z_scanf_file(const char *pathname, const char *format, ...);

Z_String z_read_file(Z_Heap *heap, const char *pathname);
Z_String_Array z_read_directory(Z_Heap *heap, const char *pathname);

Z_String z_expand_tilde(Z_Heap *heap, Z_String_View pathname);
Z_String z_compress_tilde(Z_Heap *heap, Z_String_View pathname);

const char *z_try_get_env(const char *name, const char *fallback);

// ============================================================
//                        MAP API
// ============================================================

Z_Map z_map_new(Z_Heap *heap, Z_Compare_Fn compare_keys);
size_t z_map_size(const Z_Map *map);
Z_Key_Value z_map_put(Z_Map *map, void *key, void *value);
void *z_map_get(const Z_Map *map, const void *key);
void *z_map_try_get(const Z_Map *map, const void *key, const void *fallback);
bool z_map_has(const Z_Map *map, void *key);
Z_Key_Value z_map_delete(Z_Map *map, void *key);
Z_Key_Value_Array z_map_to_array(Z_Heap *heap, Z_Map *map);
void z_map_print(const Z_Map *map, Z_Print_Fn print_key, Z_Print_Fn print_value);


// ============================================================
//                        SET API
// ============================================================

Z_Set z_set_new(Z_Heap *heap, Z_Compare_Fn compare_elements);
size_t z_set_size(const Z_Set *set);
void *z_set_add(Z_Set *set, void *element);
bool z_set_has(const Z_Set *set, void *element);
void *z_set_remove(Z_Set *set, void *element);
void z_set_print(const Z_Set *set, Z_Print_Fn print_element);


// ============================================================
//                        HEAP API
// ============================================================

void *z_heap_malloc(Z_Heap *heap, size_t size);
void *z_heap_calloc(Z_Heap *heap, size_t size);
void *z_heap_realloc(Z_Heap *heap, void *pointer, size_t new_size);
void z_heap_free_pointer(Z_Heap *heap, void *pointer);
void z_heap_free(Z_Heap *heap);


// ============================================================
//                       MESSURE TIME
// ============================================================

clock_t z_get_clock();
void z_print_elapsed_seconds(clock_t start);

// ============================================================
//                         HELPERS
// ============================================================

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