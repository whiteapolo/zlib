#include <stdbool.h>
#include "../include/hash_table.h"
#include "../include/min_max.h"

#define Z__PTR_TABLE_MIN_CAPACITY 16u
#define Z__PTR_TABLE_MAX_LOAD_FACTOR 0.7
#define Z__PTR_TABLE_TOMBSTONE ((uintptr_t)1)
#define Z__PTR_TABLE_EMPTY ((uintptr_t)0)

void z__ptr_table_insert_no_check(Z_Ptr_Table *table, uintptr_t ptr);
static inline size_t z__ptr_table_fast_mod(size_t value, size_t mod);
static inline uintptr_t z__ptr_table_hash(uintptr_t ptr);
void z__ptr_table_resize(Z_Ptr_Table *table, size_t new_capacity);
void z_ptr_table_insert(Z_Ptr_Table *table, uintptr_t ptr);
bool z_ptr_table_delete(Z_Ptr_Table *table, uintptr_t ptr);
void z_ptr_table_free(Z_Ptr_Table *table);
void z_ptr_table_reset(Z_Ptr_Table *table);

static inline size_t z__ptr_table_fast_mod(size_t value, size_t mod)
{
    return value & (mod - 1);
}

static inline uintptr_t z__ptr_table_hash(uintptr_t ptr)
{
    return ptr;
}

static inline float z__ptr_table_load_factor(const Z_Ptr_Table *table)
{
    if (table->capacity == 0) {
        return 1;
    }

    return (float)table->occupied / (float)table->capacity;
}

void z__ptr_table_insert_no_check(Z_Ptr_Table *table, uintptr_t ptr)
{
    size_t i = z__ptr_table_fast_mod(z__ptr_table_hash(ptr), table->capacity);

    while (table->ptr[i] > 1) {
        i = z__ptr_table_fast_mod(i + 1, table->capacity);
    }

    if (table->ptr[i] == Z__PTR_TABLE_EMPTY) {
        table->occupied++;
    }

    table->ptr[i] = ptr;
}

void z__ptr_table_resize(Z_Ptr_Table *table, size_t new_capacity)
{
    Z_Ptr_Table new = {
        .ptr = calloc(new_capacity, sizeof(uintptr_t)),
        .occupied = 0,
        .capacity = new_capacity,
    };

    for (size_t i = 0; i < table->capacity; i++) {
        if (table->ptr[i] > 1) {
            z__ptr_table_insert_no_check(&new, table->ptr[i]);
        }
    }

    free(table->ptr);
    *table = new;
}

void z_ptr_table_insert(Z_Ptr_Table *table, uintptr_t ptr)
{
    if (z__ptr_table_load_factor(table) >= Z__PTR_TABLE_MAX_LOAD_FACTOR) {
        size_t new_capacity = Z_MAX(Z__PTR_TABLE_MIN_CAPACITY, table->capacity * 2);
        z__ptr_table_resize(table, new_capacity);
    }

    z__ptr_table_insert_no_check(table, ptr);
}

bool z_ptr_table_delete(Z_Ptr_Table *table, uintptr_t ptr)
{
    size_t i = z__ptr_table_fast_mod(z__ptr_table_hash(ptr), table->capacity);

    while (table->ptr[i] != Z__PTR_TABLE_EMPTY) {
        if (table->ptr[i] == ptr) {
            table->ptr[i] = Z__PTR_TABLE_TOMBSTONE;
            return true;
        }

        i = z__ptr_table_fast_mod(i + 1, table->capacity);
    }

    return false;
}

void z_ptr_table_free(Z_Ptr_Table *table)
{
    for (size_t i = 0; i < table->capacity; i++) {
        if (table->ptr[i] > 1) {
            free((void*)table->ptr[i]);
        }
    }

    free(table->ptr);
}

void z_ptr_table_reset(Z_Ptr_Table *table)
{
    for (size_t i = 0; i < table->capacity; i++) {
        if (table->ptr[i] > 1) {
            free((void*)table->ptr[i]);
        }
    }

    memset(table->ptr, 0, sizeof(uintptr_t) * table->capacity);
    table->occupied = 0;
}

void *z_pool_malloc(Z_Pool *pool, size_t size)
{
    void *ptr = malloc(size);
    z_ptr_table_insert(pool, (uintptr_t)ptr);
    return ptr;
}

void *z_pool_calloc(Z_Pool *pool, size_t size)
{
    void *ptr = calloc(1, size);
    z_ptr_table_insert(pool, (uintptr_t)ptr);
    return ptr;
}

void *z_pool_realloc(Z_Pool *pool, void *ptr, size_t new_size)
{
    uintptr_t old_ptr = (uintptr_t)ptr;
    void *new_ptr = realloc(ptr, new_size);

    if ((void*)old_ptr != NULL) {
        z_ptr_table_delete(pool, old_ptr);
    }

    if (ptr != new_ptr) {
        z_ptr_table_insert(pool, (uintptr_t)new_ptr);
    }

    return new_ptr;
}

void z_pool_free(Z_Pool *pool, void *ptr)
{
    if (ptr == NULL) {
        return;
    }

    z_ptr_table_delete(pool, (uintptr_t)ptr);
    free(ptr);
}

void z_pool_free_all(Z_Pool *pool)
{
    z_ptr_table_free(pool);
}

void z_pool_reset(Z_Pool *pool)
{
    z_ptr_table_reset(pool);
}
