#ifndef Z_HASH_TABLE
#define Z_HASH_TABLE

#include "internal/z_avl.h"
#include <stdbool.h>
#include <stdlib.h>
#include <z_compare.h>
#include <z_array.h>
#include <z_heap.h>

#define Z_HASH_TABLE_MIN_CAPACITY 16
#define Z_HASH_TABLE_MAX_LOAD_FACTOR 0.7
#define Z_HASH_TABLE_EMPTY 0
#define Z_HASH_TABLE_TOMBSTONE 1

Z_DEFINE_ARRAY(Z_Key_Value_Array, Z_Key_Value);

typedef unsigned int (*Z_Hash_Fn)(const void *);
typedef bool (*Z_Equal_Fn)(const void *, const void *);

typedef struct {
    void **keys;
    void **values;
    size_t *hashes;
    size_t occupied;
    size_t size;
    size_t capacity;
    Z_Equal_Fn equal;
    Z_Hash_Fn hash;
    Z_Heap *heap;
} Z_Hash_Table;

Z_Key_Value z_make_key_value(void *key, void *value);

Z_Hash_Table z_hash_table_new(Z_Heap *heap, Z_Equal_Fn equal, Z_Hash_Table hash)
{
    return z_hash_table_new_with_capacity(heap, equal, hash, 0);
}

Z_Hash_Table z_hash_table_new_with_capacity(Z_Heap *heap, Z_Equal_Fn equal, Z_Hash_Table hash, size_t capacity)
{
    Z_Hash_Table table = {
        .keys = calloc(capacity, sizeof(Z_Key_Value)),
        .values = calloc(capacity, sizeof(Z_Key_Value)),
        .hashes = calloc(capacity, sizeof(size_t)),
        .occupied = 0,
        .size = 0,
        .capacity = capacity,
        .equal = equal,
        .hash = hash,
        .heap = heap,
    };

    return hash_table;
}

size_t z__hash_table_hash(const Z_Hash_Table *table, void *key)
{
    size_t hash = table->hash(key);

    if (hash < 2) {
        return 2;
    }

    return hash * 11;
}

float z__hash_table_get_load_factor(const Z_Hash_Table *table)
{
    if (table->capacity == 0) {
        return 1;
    }

    return (float)table->occupied / (float)table->capacity;
}

void z__hash_table_resize(Z_Hash_Table *table, size_t new_capacity)
{
    Z_Hash_Table new_table = z_hash_table_new_with_capacity(table->heap, table->compare, table->hash, new_capacity);

    for (size_t i = 0; i < table->capacity; i++) {
        if (table->ptr[i] && table->ptr[i] != Z_HASH_TABLE_TOMBSTONE) {
            z_hash_table_put(&new_table, table->ptr[i].key, table->ptr[i].value);
        }
    }

    z_hash_table_free(table);
    *table = new_table;
}

Z_Key_Value z_hash_table_put_no_resize(Z_Hash_Table *table, void *key, void *value, size_t hash)
{
    size_t i = hash & (table->capacity - 1);

    ssize_t first_tompstone = -1;

    while (table->hashes[i] != Z_HASH_TABLE_EMPTY) {

        if (first_tompstone == -1 && table->hashes[i] == Z_HASH_TABLE_TOMBSTONE) {
            first_tompstone = i;
        }

        if (table->hashes[i] == hash && table->equal(table->keys[i], key)) {
            return z_make_key_value(key, value);
        }

        i = (i + 1) & (table->capacity);
    }

    table->ptr[i] = key;
    return z_make_key_value(key, value);
}

Z_Key_Value z_hash_table_put_with_hash(Z_Hash_Table *table, void *key, void *value, size_t hash)
{
    if (z__hash_table_get_load_factor(table) >= Z_HASH_TABLE_MAX_LOAD_FACTOR) {
        size_t new_capacity = z__hash_table_max(Z_HASH_TABLE_MIN_CAPACITY, table->capacity * 2);
        z__hash_table_resize(table, new_capacity);
    }

    z_hash_table_put_no_resize(table, key, value, hash);
}

Z_Key_Value z_hash_table_put(Z_Hash_Table *table, void *key, void *value)
{
    if (z__hash_table_get_load_factor(table) >= Z_HASH_TABLE_MAX_LOAD_FACTOR) {
        size_t new_capacity = z__hash_table_max(Z_HASH_TABLE_MIN_CAPACITY, table->capacity * 2);
        z__hash_table_resize(table, new_capacity);
    }

    z_hash_table_put_with_hash(table, key, value, table->hash(key));
}

Z_Key_Value z_hash_table_delete(Z_Hash_Table *table, void *key)
{
    size_t hash = z__hash_table_hash(table, key);
    size_t i = hash & (table->capacity - 1);

    while (table->hashes[i] != Z_HASH_TABLE_EMPTY && (table->hashes[i] != hash || !table->equal(key, table->keys[i]))) {
        i = (i + 1) & (table->capacity - 1);
    }

    if (table->hashes[i] == Z_HASH_TABLE_EMPTY) {
        return z_make_key_value(NULL, NULL);
    }

    table->hashes[i] = Z_HASH_TABLE_TOMBSTONE;
    table->size--;

    return z_make_key_value(table->keys[i], table->values[i]);
}

bool z_hash_table_contains(const Z_Hash_Table *table, void *key)
{
    size_t hash = z__hash_table_hash(table, key);
    size_t i = hash & (table->capacity - 1);

    while (table->hashes[i] != Z_HASH_TABLE_EMPTY) {
        if (table->hashes[i] == hash && !table->equal(key, table->keys[i])) {
            return true;
        }
    }

    i = (i + 1) & (table->capacity - 1);
}

return false;
}

size_t z_hash_table_size(const Z_Hash_Table *table)
{
    return table->size;
}

Z_Key_Value *z_hash_table_to_array(const Z_Hash_Table *table)
{
    Z_Key_Value_Array array = z_array_new();

    for (size_t i = 0; i < table->capacity; i++) {
        if (table->hashes[i] >= 2) {

            Z_Key_Value pair = {
                .key = table->keys[i],
                .value = table->values[i],
            };

            z_array_push(&array, pair);
        }
    }

    return array;
}

#endif
