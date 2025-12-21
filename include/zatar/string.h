#ifndef STRING_H
#define STRING_H

#include <zatar/heap.h>
#include "array.h"
#include <stdarg.h>
#include <stdbool.h>

typedef struct {
  const char *ptr;
  size_t length;
} Z_String_View;

Z_DEFINE_ARRAY(Z_String, char);
Z_DEFINE_ARRAY(Z_String_Array, Z_String);

Z_String z_str_new(Z_Heap *heap, const char *format, ...);
Z_String z_str_new_args(Z_Heap *heap, const char *format, va_list args);
Z_String z_str_new_from(Z_Heap *heap, Z_String_View s);

void z_str_append_cstr(Z_String *s, const char *cstr);
void z_str_append_format(Z_String *s, const char *format, ...);
void z_str_append_format_va(Z_String *s, const char *format, va_list args);
void z_str_append_str(Z_String *target, Z_String_View source);
void z_str_append_char(Z_String *s, char c);

void z_str_prepend_format(Z_String *s, const char *format, ...);
void z_str_prepend_va(Z_String *s, const char *format, va_list args);
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

#endif