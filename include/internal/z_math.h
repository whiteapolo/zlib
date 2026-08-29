#ifndef Z_MATH
#define Z_MATH

#define Z_DEFINE_MAX(type, identifier)        \
static inline type identifier(type a, type b) \
{                                             \
    return a > b ? a : b;                     \
}

#define Z_DEFINE_MIN(type, identifier)        \
static inline type identifier(type a, type b) \
{                                             \
    return a < b ? a : b;                     \
}

Z_DEFINE_MAX(char, z__max_char);
Z_DEFINE_MAX(int, z__max_int);
Z_DEFINE_MAX(size_t, z__max_size_t);
Z_DEFINE_MAX(float, z__max_float);
Z_DEFINE_MAX(double, z__max_double);

Z_DEFINE_MIN(char, z__min_char);
Z_DEFINE_MIN(int, z__min_int);
Z_DEFINE_MIN(size_t, z__min_size_t);
Z_DEFINE_MIN(float, z__min_float);
Z_DEFINE_MIN(double, z__min_double);

#endif
