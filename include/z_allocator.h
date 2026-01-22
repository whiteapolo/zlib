#ifndef Z_ALLOCATOR_H
#define Z_ALLOCATOR_H

#include <z_heap.h>
#include <z_arena.h>
#include <stddef.h>

typedef enum {
  Z_ALLOCATOR_MODE_HEAP,
  Z_ALLOCATOR_MODE_ARENA,
} Z_Allocator_Mode;

typedef struct {
  Z_Allocator_Mode mode;
  union {
    Z_Heap heap;
    Z_Arena arena;
  };
} Z_Allocator;

Z_Allocator z_allocator_new(Z_Allocator_Mode mode);

void *z_allocator_malloc(Z_Allocator *allocator, size_t bytes);
void *z_allocator_realloc(Z_Allocator *allocator, void *ptr, size_t bytes);
void  z_allocator_free(Z_Allocator *allocator, void *ptr);

void z_allocator_reset(Z_Allocator *allocator);
void z_allocator_destroy(Z_Allocator *allocator);

#endif
