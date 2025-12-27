#ifndef Z_ARENA_H
#define Z_ARENA_H

#include <stdlib.h>

#define Z_Arena_Auto __attribute__((cleanup(z_arena_free))) Z_Arena

typedef struct {
  char *ptr;
  size_t offset;
  size_t capacity;
} Z_Arena;

Z_Arena z_arena_new();
void *z_arena_alocate(Z_Arena *arena, size_t bytes);
void z_arena_reset(Z_Arena *arena);
void z_arena_free(Z_Arena *arena);

#endif
