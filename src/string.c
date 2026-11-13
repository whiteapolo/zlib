#include "../include/string.h"
#include "../include/array.h"
#include "../include/min-max.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#define Z__WHITE_SPACE " \f\n\r\t\v"

static inline int z__size_t_to_int(size_t a);

static inline int z__size_t_to_int(size_t a)
{
    if (a > INT_MAX) {
        return INT_MAX;
    }

    return (int)a;
}

static inline int z__vsnprintf(size_t size; char str[restrict size], size_t size, const char *restrict format, va_list ap)
{
    // exists since vsnprintf override the va_list.
    va_list ap_tmp;
    va_copy(ap_tmp, ap);
    int ret = vsnprintf(str, size, format, ap_tmp);
    va_end(ap_tmp);

    return ret;
}

int z_get_format_length(const char *format, va_list ap)
{
    int size = z__vsnprintf(NULL, 0, format, ap);
    assert(size >= 0);

    return size;
}

Z_String z_str_new(Z_Pool *pool, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    Z_String s = z_str_new_va(pool, format, ap);
    va_end(ap);

    return s;
}

Z_String z_str_new_va(Z_Pool *pool, const char *format, va_list ap)
{
    Z_String s = z_array_new(pool, Z_String);
    z_str_push_va(&s, format, ap);
    return s;
}

Z_String z_str_new_from_sv(Z_Pool *pool, Z_String_View s)
{
    Z_String str = z_array_new(pool, Z_String);
    z_str_push_sv(&str, s);
    return str;
}

Z_String z_str_new_from_cstr(Z_Pool *pool, const char *cstr)
{
   return z_str_new_from_sv(pool, z_sv(cstr));
}

char *z_sv_to_cstr(Z_Pool *pool, Z_String_View s)
{
    char *ret = z_pool_malloc(pool, sizeof(char) * (s.length + 1));
    memcpy(ret, s.ptr, sizeof(char) * s.length);
    ret[s.length] = 0;
    return ret;
}

char *z_cstr_dup(Z_Pool *pool, const char *s)
{
    return z_sv_to_cstr(pool, z_sv(s));
}

Z_String z_str_take_ownership(Z_Pool *pool, char *s)
{
    size_t length = strlen(s);

    Z_String str = {
        .pool = pool,
        .ptr = s,
        .length = length,
        .capacity = length + 1,
    };

    return str;
}

void z_str_push(Z_String *s, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    z_str_push_va(s, format, ap);
    va_end(ap);
}

void z_str_push_va(Z_String *s, const char *format, va_list ap)
{
    size_t format_length = (size_t)z__vsnprintf(NULL, 0, format, ap);
    z_array_reserve(s, s->length + format_length + 1);
    z__vsnprintf(s->ptr + s->length, format_length + 1, format, ap);
    s->length += format_length;
}

void z_str_push_sv(Z_String *target, Z_String_View source)
{
    z_array_push_array(target, source);
    z_array_zero_terminate(target);
}

void z_str_push_char(Z_String *s, char c)
{
    z_array_push(s, c);
    z_array_zero_terminate(s);
}

void z_str_push_cstr(Z_String *s, const char *cstr)
{
    z_str_push_sv(s, z_sv(cstr));
}

char z_str_pop_char(Z_String *s)
{
    assert(s->length > 0);
    char c = z_array_pop(s);
    z_array_zero_terminate(s);
    return c;
}

void z_str_replace(Z_String *s, Z_String_View target, Z_String_View replacement)
{
    Z_Pool_Auto pool = {0};
    Z_String tmp = z_str_new(&pool, "");

    size_t i = 0;

    while (i < s->length) {
        if (z_sv_equal_n(z_sv_advance(z_sv(s), i), target, target.length)) {
            z_str_push_sv(&tmp, replacement);
            i += target.length;
        } else {
            z_str_push_char(&tmp, s->ptr[i]);
            i++;
        }
    }

    z_str_clear(s);
    z_str_push(s, "%s", tmp.ptr);
}

Z_Sv_Split_Iter z_sv_split_iter(Z_String_View s, Z_String_View delimeter)
{
    Z_Sv_Split_Iter iter = {
        .s = s,
        .delimeter = delimeter,
        .current = 0,
    };

    return iter;
}

bool z_sv_split_iter_next(Z_Sv_Split_Iter *iter, Z_String_View *slice)
{
    if (iter->current > iter->s.length) {
        return false;
    }

    Z_String_View small_world = z_sv_advance(iter->s, iter->current);
    ssize_t next = z_sv_find_index(small_world, iter->delimeter);

    if (next < 0) {
        iter->current += small_world.length + 1;
        *slice = small_world;
        return true;
    }

    iter->current += (size_t)next + iter->delimeter.length;
    *slice = z_sv_substring(small_world, 0, (size_t)next);
    return true;
}

void z_str_split(Z_String_View s, Z_String_View delimiter, Z_String_Array *out)
{
    Z_Sv_Split_Iter iter = z_sv_split_iter(s, delimiter);
    Z_String_View curr;

    while (z_sv_split_iter_next(&iter, &curr)) {
        z_array_push(out, z_str_new_from_sv(out->pool, curr));
    }
}

Z_String_View z_sv_split_part(Z_String_View s, Z_String_View delimiter, size_t index)
{
    Z_Sv_Split_Iter iter = z_sv_split_iter(s, delimiter);
    Z_String_View slice;

    for (size_t i = 0; i < index; i++) {
        z_sv_split_iter_next(&iter, &slice);
    }

    assert(z_sv_split_iter_next(&iter, &slice) && "index of split is overreaching");
    return slice;
}

Z_String_View z_sv_from_str_ptr(const Z_String *s)
{
    Z_String_View view = {
        .ptr = s->ptr,
        .length = s->length
    };

    return view;
}

Z_String_View z_sv_from_str(Z_String s)
{
    Z_String_View view = {
        .ptr = s.ptr,
        .length = s.length
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

Z_String_View z_sv_advance(Z_String_View s, size_t offset)
{
    Z_String_View view = {
        .ptr = s.ptr + offset,
        .length = z__size_t_to_int(s.length) - z__size_t_to_int(offset) < 0 ? 0 : s.length - offset,
    };

    return view;
}

Z_String_View z_sv_substring(Z_String_View s, size_t start, size_t length)
{
    Z_String_View view = {
        .ptr = s.ptr + start,
        .length = length,
    };

    return view;
}

char z_sv_top(Z_String_View s)
{
    return s.ptr[s.length - 1];
}

int z_sv_compare(Z_String_View a, Z_String_View b)
{
    int result = memcmp(a.ptr, b.ptr, Z_MIN(a.length, b.length));

    if (result == 0) {
        return z__size_t_to_int(a.length) - z__size_t_to_int(b.length);
    }

    return result;
}

bool z_sv_equal(Z_String_View a, Z_String_View b)
{
    return z_sv_compare(a, b) == 0;
}

bool z_sv_like(Z_String_View str, Z_String_View pattern)
{
    ssize_t last_wildcard_match = -1;
    ssize_t last_pattern_wildcard = -1;
    size_t i = 0;
    size_t j = 0;

    while (i < str.length && j < pattern.length) {
        if (pattern.ptr[j] == '%') {
            last_pattern_wildcard = (ssize_t)j;
            last_wildcard_match = (ssize_t)i;
            j++;
        } else if (str.ptr[i] == pattern.ptr[j] || pattern.ptr[j] == '_') {
            i++;
            j++;
        } else if (last_pattern_wildcard != -1) {
            j = (size_t)last_pattern_wildcard + 1;
            i = (size_t)++last_wildcard_match;
        } else {
            return false;
        }
    }

    if (i == str.length && j == pattern.length) {
        return true;
    }

    if ((j == pattern.length || j + 1 == pattern.length) && z_sv_top(pattern) == '%') {
        return true;
    }

    return false;
}

int z_sv_compare_n(Z_String_View a, Z_String_View b, size_t n)
{
    size_t length = Z_MIN(n, Z_MIN(a.length, b.length));
    int result = memcmp(a.ptr, b.ptr, length);

    if (result != 0) return result;
    if (n <= length) return 0;
    if (a.length < b.length) return -1;
    if (a.length > b.length) return 1;
    return 0;
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

bool z_sv_contains_char(Z_String_View s, char c)
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
            return (ssize_t)i;
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
    z_str_trim_right_cset(s, z_sv(Z__WHITE_SPACE));
}

void z_str_trim_left(Z_String *s)
{
    z_str_trim_left_cset(s, z_sv(Z__WHITE_SPACE));
}

void z_str_trim_right_cset(Z_String *s, Z_String_View cset)
{
    Z_String_View trimmed = z_sv_trim_right_cset(z_sv(s), cset);
    memmove(s->ptr, trimmed.ptr, trimmed.length);
    s->length = trimmed.length;
    z_array_zero_terminate(s);
}

void z_str_trim_left_cset(Z_String *s, Z_String_View cset)
{
    Z_String_View trimmed = z_sv_trim_left_cset(z_sv(s), cset);
    memmove(s->ptr, trimmed.ptr, trimmed.length);
    s->length = trimmed.length;
    z_array_zero_terminate(s);
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

    while (trimmed.length > 0 && z_sv_contains_char(cset, z_sv_top(trimmed))) {
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

    while (trimmed.length > 0 && z_sv_contains_char(cset, s.ptr[0])) {
        trimmed = z_sv_advance(trimmed, 1);
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

void z_str_override(Z_String *s, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    z_str_override_va(s, format, ap);
    va_end(ap);
}

void z_str_override_va(Z_String *s, const char *format, va_list ap)
{
    z_str_clear(s);
    z_str_push_va(s, format, ap);
}

size_t z_sv_get_edit_distance(Z_String_View a, Z_String_View b)
{
	size_t row[a.length + 1];

	for (size_t j = 0; j <= a.length; j++) {
		row[j] = j;
    }

	for (size_t i = 1; i <= b.length; i++) {
        size_t topleft = i - 1;
		row[0] = i;

		for (size_t j = 1; j <= a.length; j++) {
			size_t curr;

			if (a.ptr[j-1] == b.ptr[i-1]) {
				curr = topleft;
            } else {
				curr = 1 + Z_MIN3(topleft, row[j], row[j-1]);
            }

			topleft = row[j];
			row[j] = curr;
		}
	}

	return row[a.length];
}
