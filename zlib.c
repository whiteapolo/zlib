#include "zlib.h"
#include <string.h>
#include <limits.h>
#include <dirent.h>

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

size_t z__get_file_size(FILE *fp)
{
  size_t curr = ftell(fp);
  fseek(fp, 0, SEEK_END);

  size_t size = ftell(fp);
  fseek(fp, curr, SEEK_SET);

  return size;
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