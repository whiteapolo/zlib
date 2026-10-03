#ifndef Z_POOL_H
#define Z_POOL_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uintptr_t *ptr;
    size_t occupied;
    size_t capacity;
} Z_Ptr_Table;

typedef Z_Ptr_Table Z_Pool;

#define Z_Pool_Auto __attribute__((cleanup(z_pool_free_all))) Z_Pool

void *z_pool_malloc(Z_Pool *pool, size_t size);
void *z_pool_calloc(Z_Pool *pool, size_t size);
void *z_pool_realloc(Z_Pool *pool, void *ptr, size_t new_size);
void z_pool_free(Z_Pool *pool, void *ptr);
void z_pool_free_all(Z_Pool *pool);
void z_pool_reset(Z_Pool *pool);

#endif
