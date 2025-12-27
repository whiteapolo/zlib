#include "z_config.h"
#include <stdio.h>
#include <z_arena.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>

Z_Arena z_arena_new()
{
  void *ptr = mmap(
      NULL,
      Z_ARENA_VIRTUAL_SPACE,
      PROT_READ | PROT_WRITE,
      MAP_PRIVATE | MAP_ANONYMOUS,
      -1,
      0
  );

  assert(ptr != MAP_FAILED);

  Z_Arena arena = {
    .ptr = ptr,
    .offset = 0,
    .capacity = Z_ARENA_VIRTUAL_SPACE,
  };

  return arena;
}

static size_t align_up(size_t n)
{
  return n + (n % Z_MEMORY_ALIGHMENT);
}

void *z_arena_alocate(Z_Arena *arena, size_t bytes)
{
  size_t new_offset = align_up(arena->offset);
  assert(arena->ptr + new_offset < arena->ptr + arena->capacity);
  void *ptr = arena->ptr + arena->offset;
  arena->offset = new_offset;
  return ptr;
}

void z_arena_reset(Z_Arena *arena)
{
  arena->offset = 0;
}

void z_arena_free(Z_Arena *arena)
{
  printf("hey\n");
  munmap(arena->ptr, arena->capacity);
}
