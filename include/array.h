#ifndef Z_ARRAY_H
#define Z_ARRAY_H

#include <string.h>
#include <assert.h>
#include "internal/config.h"
#include "min-max.h"
#include "pool.h"

#define Z_DEFINE_ARRAY(identifier, element_type) \
typedef struct {                                 \
    Z_Pool *pool;                                \
    element_type *ptr;                           \
    size_t length;                               \
    size_t capacity;                             \
} identifier

#define Z_DEFINE_ARRAY_VIEW(identifier, element_type) \
typedef struct {                                      \
    const element_type *ptr;                          \
    size_t length;                                    \
} identifier

#define z_array_new(pool_ptr, type) ((type){ .pool = pool_ptr, .ptr = NULL, .length = 0, .capacity = 0 })

#define z_array_push_back(array_ptr, element)                        \
    do {                                                             \
        z_array_ensure_capacity(array_ptr, (array_ptr)->length + 1); \
        (array_ptr)->ptr[(array_ptr)->length++] = element;           \
    } while (0)

#define z_array_pop_back(array_ptr)         \
(                                           \
    assert((array_ptr)->length > 0),        \
    (array_ptr)->ptr[--(array_ptr)->length] \
)

#define z_array_peek(array_ptr)               \
(                                             \
    assert((array_ptr)->length > 0),          \
    (array_ptr)->ptr[(array_ptr)->length - 1] \
)

#define z_array_insert(array_ptr, element, index)     \
    do {                                               \
        z_array_reserve_space_at(array_ptr, index, 1); \
        (array_ptr)->ptr[index] = element;             \
    } while (0)

#define z_array_push_back_array(array_ptr, array_view)                                                                     \
    do {                                                                                                                   \
        z_array_ensure_capacity((array_ptr), (array_ptr)->length + (array_view).length);                                   \
        memcpy((array_ptr)->ptr + (array_ptr)->length, (array_view).ptr, sizeof(*(array_ptr)->ptr) * (array_view).length); \
        (array_ptr)->length += (array_view).length;                                                                        \
    } while (0)

#define z_array_insert_array(array_ptr, array_view, index)                                                  \
    do {                                                                                                     \
        z_array_reserve_space_at(array_ptr, index, (array_view).length);                                     \
        memcpy((array_ptr)->ptr + index, (array_view).ptr, sizeof(*(array_ptr)->ptr) * (array_view).length); \
    } while (0)

#define z_array_zero_terminate(array_ptr)                                             \
    do {                                                                              \
        z_array_ensure_capacity(array_ptr, (array_ptr)->length + 1);                  \
        memset(&(array_ptr)->ptr[(array_ptr)->length], 0, sizeof(*(array_ptr)->ptr)); \
    } while (0)

#define z_array_ensure_capacity(array_ptr, needed)                                                                            \
    do {                                                                                                                      \
        size_t _needed = needed;                                                                                              \
        if ((array_ptr)->capacity < (_needed)) {                                                                              \
            size_t new_capacity = Z_MAX(_needed, ((array_ptr)->capacity * Z_ARRAY_GROWTH_FACTOR));                            \
            (array_ptr)->ptr = z_pool_realloc((array_ptr)->pool, (array_ptr)->ptr, sizeof(*(array_ptr)->ptr) * new_capacity); \
            (array_ptr)->capacity = new_capacity;                                                                             \
        }                                                                                                                     \
    } while (0)

#define z_array_reserve_space_at(array_ptr, index, space)                                                            \
    do {                                                                                                             \
        size_t _index = index;                                                                                       \
        size_t _space = space;                                                                                       \
        assert(_space <= (array_ptr)->length);                                                                       \
        size_t _length = (array_ptr)->length - _index;                                                               \
        z_array_ensure_capacity(array_ptr, (array_ptr)->length + _space);                                            \
        memmove((array_ptr)->ptr + _index + _space, (array_ptr)->ptr + _index, sizeof(*(array_ptr)->ptr) * _length); \
        (array_ptr)->length += _space;                                                                               \
    } while (0)

#endif
