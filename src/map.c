#include "../include/map.h"

Z_Map z_map_new(Z_Pool *pool, Z_Compare_Fn compare_keys)
{
    return z_avl_new(pool, compare_keys);
}

void *z_map_get(const Z_Map *map, const void *key)
{
    return z_avl_get(map, key);
}

bool z_map_contains(const Z_Map *map, const void *key)
{
    return z_avl_contains(map, key);
}

Z_Maybe_Pair z_map_put(Z_Map *map, void *key, void *value)
{
    return z_avl_put(map, key, value);
}

Z_Maybe_Pair z_map_delete(Z_Map *map, void *key)
{
    return z_avl_delete(map, key);
}

size_t z_map_size(const Z_Map *map)
{
    return z_avl_size(map);
}

Z_Pair_Array z_map_to_array(Z_Pool *pool, const Z_Map *map)
{
    return z_avl_to_array(pool, map);
}

void *z_map_get_min(const Z_Map *map)
{
    return z_avl_get_min(map);
}

void *z_map_get_max(const Z_Map *map)
{
    return z_avl_get_max(map);
}

void z_map_foreach(const Z_Map *map,
                   void callback(void *key, void *value, void *arg),
                   void *arg)
{
    z_avl_foreach(map, callback, arg);
}
