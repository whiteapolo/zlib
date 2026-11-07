#include "../include/flat-pool.h"
#include "../include/error.h"
#include <assert.h>
#include <stdlib.h>
#include <sys/mman.h>

Z_Flat_Pool z_flat_pool_new(size_t capacity)
{
    void *buffer = mmap(NULL, capacity, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (buffer == NULL) {
        z_perror("flat_pool_new(%zub) Failed", capacity);
        exit(1);
    }

    Z_Flat_Pool pool = {
        .buffer = buffer,
        .offset = 0,
        .capacity = capacity,
    };

    return pool;
}

void *z_flat_pool_allocate(Z_Flat_Pool *pool, size_t size)
{
    assert(pool->offset + size < pool->capacity);
    void *ptr = pool->buffer + pool->offset;
    pool->offset += size;
    return ptr;
}

void z_flat_pool_free(Z_Flat_Pool *pool)
{
    munmap(pool->buffer, pool->capacity);
}

void z_flat_pool_clear(Z_Flat_Pool *pool)
{
    pool->offset = 0;
}
