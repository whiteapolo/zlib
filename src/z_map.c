#include <z_map.h>
#include <internal/z_avl.h>

Z_Map z_map_new(Z_Allocator *allocator, Z_Compare_Fn compare_keys)
{
  Z_Map map = {
    .allocator = allocator,
    .root = NULL,
    .compare_keys = compare_keys,
    .size = 0,
  };

  return map;
}

size_t z_map_size(const Z_Map *map)
{
  return map->size;
}

Z_Key_Value z_map_put(Z_Map *map, void *key, void *value)
{
  Z_Key_Value old_key_value = z__avl_put(map->allocator, &map->root, key, value, map->compare_keys);

  if (old_key_value.key) {
    map->size++;
  }

  return old_key_value;
}

void *z_map_get(const Z_Map *map, const void *key)
{
  return z__avl_get(map->root, key, map->compare_keys);
}

void *z_map_try_get(const Z_Map *map, const void *key, const void *fallback)
{
  return z__avl_try_get(map->root, key, map->compare_keys, fallback);
}

bool z_map_has(const Z_Map *map, void *key)
{
  return z__avl_has(map->root, key, map->compare_keys);
}

Z_Key_Value z_map_delete(Z_Map *map, void *key)
{
  Z_Key_Value old_key_value = z__avl_remove(map->allocator, &map->root, key, map->compare_keys);

  if (old_key_value.key) {
    map->size--;
  }

  return old_key_value;
}

Z_Key_Value_Array z_map_to_array(Z_Allocator *allocator, Z_Map *map)
{
  return z__avl_to_array(allocator, map->root);
}

void z_map_print(const Z_Map *map, Z_Print_Fn print_key, Z_Print_Fn print_value)
{
  z__avl_print(map->root, print_key, print_value);
}