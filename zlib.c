#include "zlib.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>

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

size_t z__get_format_length(const char *format, va_list arguments)
{
  va_list arguments_copy;
  va_copy(arguments_copy, arguments);

  size_t size = vsnprintf(NULL, 0, format, arguments_copy);
  va_end(arguments_copy);

  return size;
}

Z_String *z_str_new(const char *format, ...)
{
  va_list arguments;
  va_start(arguments, format);
  Z_String *s = z_str_new_variadic(format, arguments);
  va_end(arguments);
  return s;
}

Z_String *z_str_new_variadic(const char *format, va_list arguments)
{
  Z_String *s = NULL;
  z_str_append_variadic(&s, format, arguments);
  return s;
}

Z_String *z_str_new_from(Z_String_View s)
{
  return z_str_new("%.*s", z__size_t_to_int(s.length), s.ptr);
}

void z_str_append(Z_String **s, const char *format, ...)
{
  va_list arguments;
  va_start(arguments, format);
  z_str_append_variadic(s, format, arguments);
  va_end(arguments);
}

void z_str_append_variadic(Z_String **s, const char *format, va_list arguments)
{
  size_t format_length = z__get_format_length(format, arguments);
  z__array_ensure_capacity((void **)s, z_array_length(*s) + format_length + 1, sizeof(char));

  va_list arguments_copy;
  va_copy(arguments_copy, arguments);
  vsnprintf(*s + z_array_length(*s), format_length + 1, format, arguments_copy);
  va_end(arguments_copy);

  z__array_header(*s)->length = z_array_length(*s) + format_length;
}

void z_str_append_str(Z_String **target, Z_String_View source)
{
  z_str_append(target, "%.*s", z__size_t_to_int(source.length), source.ptr);
}

void z_str_append_char(Z_String **s, char c)
{
  z_str_append(s, "%c", c);
}

char z_str_pop_char(Z_String **s)
{
  char c = z_array_pop(s);
  z_array_null_terminate(s);
  return c;
}

void z_str_set(Z_String **s, const char *format, ...)
{
  z_str_clear(s);
  va_list arguments;
  va_start(arguments, format);
  z_str_append_variadic(s, format, arguments);
  va_end(arguments);
}

void z_str_set_str(Z_String **s, Z_String_View str)
{
  z_str_set(s, "%.*s", z__size_t_to_int(str.length), str.ptr);
}

void z_str_replace(Z_String **s, Z_String_View target, Z_String_View replacement)
{
  Z_String *tmp = z_str_new("");

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

Z_String *z_str_join(Z_String **s, Z_String_View delimiter)
{
  if (z_array_length(s) == 0) {
    return z_str_new("");
  }

  Z_String *result = z_str_new("");

  for (size_t i = 0; i < z_array_length(s) - 1; i++) {
    z_str_append_str(&result, z_sv(s[i]));
    z_str_append_str(&result, delimiter);
  }

  z_str_append_str(&result, z_sv(s[z_array_length(s) - 1]));
  return result;
}

Z_String **z_str_split(Z_String_View s, Z_String_View delimiter)
{
  
}

size_t z_str_length(Z_String *s)
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
  if (needle.length == 0) {
    return true;
  }

  if (needle.length > haystack.length) {
    return false;
  }

  for (size_t i = 0; i < haystack.length - needle.length + 1; i++) {
    if (memcmp(haystack.ptr + i, needle.ptr, needle.length) == 0) {
      return true;
    }
  }

  return false;
}

bool z_sv_contain_char(Z_String_View s, char c)
{
  return memchr(s.ptr, c, s.length);
}

void z_str_trim(Z_String **s)
{
  z_str_trim_right(s);
  z_str_trim_left(s);
}

void z_str_trim_cset(Z_String **s, Z_String_View cset)
{
  z_str_trim_right_cset(s, cset);
  z_str_trim_left_cset(s, cset);
}

void z_str_trim_right(Z_String **s)
{
  z_str_trim_right_cset(s, z_sv(Z__WHITE_SPACE));
}

void z_str_trim_left(Z_String **s)
{
  z_str_trim_left_cset(s, z_sv(Z__WHITE_SPACE));
}

void z_str_trim_right_cset(Z_String **s, Z_String_View cset)
{
  Z_String_View trimmed = z_sv_trim_right_cset(z_sv(*s), cset);
  z_str_set_str(s, trimmed);
}

void z_str_trim_left_cset(Z_String **s, Z_String_View cset)
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

void z_str_free(Z_String **s)
{
  z_array_free(s);
}

void z_str_array_free(Z_String ***s)
{
  for (size_t i = 0; i < z_array_length(*s); i++) {
    z_str_free(*s + i);
  }

  z_array_free(s);
}

void z_str_clear(Z_String **s)
{
  z__array_header(*s)->length = 0;
  z_array_null_terminate(s);
}
