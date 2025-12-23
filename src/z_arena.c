#include <z_arena.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>

Z_Arena z_arena_new(size_t capacity)
{
  void *ptr = mmap(
      NULL,
      capacity,
      PROT_READ | PROT_WRITE,
      MAP_PRIVATE | MAP_ANONYMOUS,
      -1,
      0
  );

  assert(ptr != MAP_FAILED);

  Z_Arena arena = {
    .ptr = ptr,
    .offset = 0,
    .capacity = capacity,
  };

  return arena;
}

static size_t align_up(size_t n, size_t align)
{
    return (n + align - 1) & ~(align - 1);
}

void *z_arena_alocate(Z_Arena *arena, size_t bytes, size_t align)
{
  size_t aligned_size = align_up(arena->offset, align);

  if (arena->offset + aligned_size > arena->capacity) {
    return NULL;
  }

  void *ret = arena->ptr + arena->offset;
  arena->offset += aligned_size;
  return ret;
}

void z_arena_free(Z_Arena *arena)
{
  munmap(arena->ptr, arena->capacity);
}
