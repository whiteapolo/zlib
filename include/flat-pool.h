#ifndef FLAT_POOL_H
#define FLAT_POOL_H

#define Z_FLAT_POOL_DEFAULT_VIRTUAL_SIZE (256*1024*1024)

typedef struct {
    char *buffer;
    size_t offset;
    size_t capacity;
} Z_Flat_Pool;

Z_Flat_Pool z_flat_pool_new(size_t capacity);
void *z_flat_pool_allocate(Z_Flat_Pool *pool, size_t size);
void z_flat_pool_clear(Z_Flat_Pool *pool);
void z_flat_pool_free(Z_Flat_Pool *pool);

#endif
