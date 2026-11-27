#ifndef Z_MAP_H
#define Z_MAP_H

#include "internal/avl.h"

typedef Z_Avl_Tree Z_Map;

Z_Map z_map_new(Z_Pool *pool, Z_Compare_Fn compare_keys);

void *z_map_get(const Z_Map *map, const void *key);
bool z_map_contains(const Z_Map *map, const void *key);

Z_Maybe_Pair z_map_put(Z_Map *map, void *key, void *value);
Z_Maybe_Pair z_map_delete(Z_Map *map, void *key);

size_t z_map_size(const Z_Map *map);
Z_Pair_Array z_map_to_array(Z_Pool *pool, const Z_Map *map);

void *z_map_get_min(const Z_Map *map);
void *z_map_get_max(const Z_Map *map);

void z_map_foreach(const Z_Map *map,
                   void callback(void *key, void *value, void *arg),
                   void *arg);

#endif
