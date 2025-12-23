#ifndef Z_ARENA_H
#define Z_ARENA_H

#include <stdlib.h>

typedef struct {
  char *ptr;
  size_t offset;
  size_t capacity;
} Z_Arena;

Z_Arena z_arena_new(size_t capacity);
void *z_arena_alocate(Z_Arena *arena, size_t bytes, size_t align);
void z_arena_free(Z_Arena *arena);

#endif
