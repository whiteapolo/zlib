#ifndef ZLIB_H
#define ZLIB_H

#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

//----------------------------------------------------
//
// API
//
//----------------------------------------------------

#define Z_DEFAULT_GROWTH_RATE 2

#define z_array_push(array_ptr, element) z__array_push((void **)(array_ptr), &(typeof(**(array_ptr))){element}, sizeof(**(array_ptr)))
#define z_array_pop(array_ptr) ((*(array_ptr))[--z__array_header(*(array_ptr))->length])
#define z_array_null_terminate(array_ptr) z__array_null_terminate((void **)(array_ptr), sizeof(**(array_ptr)))
#define z_array_length(array) z__array_length(array)
#define z_array_free(array_ptr) z__array_free((void **)array_ptr)

typedef char Z_String;

typedef struct {
  const char *ptr;
  size_t length;
} Z_String_View;

Z_String *z_str_new(const char *format, ...);
Z_String *z_str_new_variadic(const char *format, va_list arguments);
Z_String *z_str_new_from(Z_String_View s);

void z_str_append(Z_String **s, const char *format, ...);
void z_str_append_variadic(Z_String **s, const char *format, va_list arguments);
void z_str_append_str(Z_String **target, Z_String_View source);
void z_str_append_char(Z_String **s, char c);
char z_str_pop_char(Z_String **s);

void z_str_set(Z_String **s, const char *format, ...);
void z_str_replace(Z_String **s, Z_String_View target, Z_String_View replacement);
Z_String *z_str_join(char **s, Z_String_View delimiter);
Z_String **z_str_split(Z_String_View s, Z_String_View delimiter);
Z_String_View *z_sv_split(Z_String_View s, Z_String_View delimiter);

size_t z_str_length(Z_String *s);
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

void z_str_trim(Z_String **s);
void z_str_trim_cset(Z_String **s, Z_String_View cset);
void z_str_trim_right(Z_String **s);
void z_str_trim_left(Z_String **s);
void z_str_trim_right_cset(Z_String **s, Z_String_View cset);
void z_str_trim_left_cset(Z_String **s, Z_String_View cset);
Z_String_View z_sv_trim(Z_String_View s);
Z_String_View z_sv_trim_cset(Z_String_View s, Z_String_View cset);
Z_String_View z_sv_trim_right(Z_String_View s);
Z_String_View z_sv_trim_right_cset(Z_String_View s, Z_String_View cset);
Z_String_View z_sv_trim_left(Z_String_View s);
Z_String_View z_sv_trim_left_cset(Z_String_View s, Z_String_View cset);

void z_sv_print(Z_String_View s);
void z_sv_println(Z_String_View s);
void z_str_free(Z_String **s);
void z_str_clear(Z_String **s);

//----------------------------------------------------
//
// Internal
//
//----------------------------------------------------

typedef struct {
  size_t capacity;
  size_t length;
} Z_Array_Header;

Z_Array_Header *z__array_header(void *array);
void *z__array_from_header(Z_Array_Header *header);
size_t z__array_length(void *array);
size_t z__array_capacity(void *array);
void z__array_ensure_capacity(void **array, size_t capacity, size_t element_size);
void z__array_free(void **array);
void *z__array_end(void *array, size_t element_size);
void z__array_null_terminate(void **array, size_t element_size);
void z__array_push(void **array, const void *element, size_t element_size);

#endif