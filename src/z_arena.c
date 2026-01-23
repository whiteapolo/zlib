#include <stdio.h>
#include <internal/z_arena.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <internal/z_config.h>
#include <stdalign.h>
#include <stddef.h>
#include <string.h>

typedef struct {
  size_t size;
  size_t padding;
} Z_Block_Header;

Z_Arena z_arena_new()
{
  char *begin = mmap(
      NULL,
      Z_ARENA_VIRTUAL_SPACE,
      PROT_READ | PROT_WRITE,
      MAP_PRIVATE | MAP_ANONYMOUS,
      -1,
      0
  );

  assert(begin != MAP_FAILED);

  Z_Arena arena = {
    .begin = begin,
    .current = begin,
    .end = begin + Z_ARENA_VIRTUAL_SPACE,
  };

  return arena;
}

void *z_arena_malloc(Z_Arena *arena, size_t size)
{
  Z_Block_Header *block = (Z_Block_Header*)arena->current;
  char *ptr = arena->current + sizeof(Z_Block_Header);

  block->size = size;
  arena->current = ptr + size + (size % 16);
  assert(arena->current < arena->end);

  return ptr;
}

void *z_arena_realloc(Z_Arena *arena, void *ptr, size_t size)
{
  if (ptr == NULL) {
    return z_arena_malloc(arena, size);
  }

  Z_Block_Header *block = ((Z_Block_Header*)ptr) - 1;
  size_t old_size = block->size;

  if (old_size >= size) {
    return ptr;
  }

  void *new_ptr = z_arena_malloc(arena, size * 2);
  memcpy(new_ptr, ptr, old_size);

  return new_ptr;
}

void z_arena_reset(Z_Arena *arena)
{
  arena->current = arena->begin;
}

void z_arena_free(Z_Arena *arena)
{
  munmap(arena->begin, arena->end - arena->begin);
}
