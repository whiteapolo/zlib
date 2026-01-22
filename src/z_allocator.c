#include <z_allocator.h>
#include <internal/z_arena.h>
#include <internal/z_heap.h>

Z_Allocator z_allocator_new(Z_Allocator_Mode mode)
{
  Z_Allocator allocator = {
    .mode = mode,
  };

  switch (mode) {
    case Z_ALLOCATOR_MODE_HEAP:
      allocator.heap = z_heap_new();
      break;

    case Z_ALLOCATOR_MODE_ARENA:
      allocator.arena = z_arena_new();
      break;
  }

  return allocator;
}

void *z_allocator_malloc(Z_Allocator *allocator, size_t bytes)
{
  switch (allocator->mode) {
    case Z_ALLOCATOR_MODE_HEAP:
      return z_heap_malloc(&allocator->heap, bytes);

    case Z_ALLOCATOR_MODE_ARENA:
      return z_arena_allocate(&allocator->arena, bytes);
  }

  return NULL;
}

void *z_allocator_realloc(Z_Allocator *allocator, void *ptr, size_t bytes)
{
  switch (allocator->mode) {
    case Z_ALLOCATOR_MODE_HEAP:
      return z_heap_realloc(&allocator->heap, ptr, bytes);

    case Z_ALLOCATOR_MODE_ARENA:
      return z_arena_allocate(&allocator->arena, bytes);
  }

  return NULL;
}

void z_allocator_free_pointer(Z_Allocator *allocator, void *ptr)
{
  switch (allocator->mode) {
    case Z_ALLOCATOR_MODE_HEAP:
      z_heap_free_pointer(&allocator->heap, ptr);
      break;

    case Z_ALLOCATOR_MODE_ARENA:
      break;
  }
}

void z_allocator_reset(Z_Allocator *allocator)
{
  switch (allocator->mode) {
    case Z_ALLOCATOR_MODE_HEAP:
      z_heap_reset(&allocator->heap);
      break;

    case Z_ALLOCATOR_MODE_ARENA:
      z_arena_reset(&allocator->arena);
      break;
  }
}

void z_allocator_free(Z_Allocator *allocator)
{
  switch (allocator->mode) {
    case Z_ALLOCATOR_MODE_HEAP:
      z_heap_free(&allocator->heap);
      break;

    case Z_ALLOCATOR_MODE_ARENA:
      z_arena_free(&allocator->arena);
      break;
  }
}
