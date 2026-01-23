#ifndef Z_ALLOCATOR_H
#define Z_ALLOCATOR_H

#include <internal/z_heap.h>
#include <internal/z_arena.h>
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

#define Z_Allocator_Auto __attribute__((cleanup(z_allocator_free))) Z_Allocator

Z_Allocator z_allocator_new(Z_Allocator_Mode mode);

void *z_allocator_malloc(Z_Allocator *allocator, size_t size);
void *z_allocator_realloc(Z_Allocator *allocator, void *ptr, size_t size);
void z_allocator_free_pointer(Z_Allocator *allocator, void *ptr);

void z_allocator_reset(Z_Allocator *allocator);
void z_allocator_free(Z_Allocator *allocator);

#endif
