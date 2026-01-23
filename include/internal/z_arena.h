#ifndef Z_ARENA_H
#define Z_ARENA_H

#include <stdlib.h>
#include <stdint.h>

typedef struct {
  char *begin;
  char *current;
  char *end;
} Z_Arena;

Z_Arena z_arena_new();
void *z_arena_malloc(Z_Arena *arena, size_t size);
void *z_arena_realloc(Z_Arena *arena, void *ptr, size_t size);
void z_arena_reset(Z_Arena *arena);
void z_arena_free(Z_Arena *arena);

#endif
