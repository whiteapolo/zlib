#ifndef MAP_H
#define MAP_H

#include "heap.h"

typedef Z_Avl_Node Z_Avl_Node;

typedef struct {
  void *key;
  void *value;
} Z_Key_Value;

Z_DEFINE_ARRAY(Z_Key_Value_Array, Z_Key_Value);

typedef struct {
  Z_Heap *heap;
  Z_Avl_Node *root;
  size_t size;
  Z_Compare_Fn compare_keys;
} Z_Map;

Z_Map z_map_new(Z_Heap *heap, Z_Compare_Fn compare_keys);
size_t z_map_size(const Z_Map *map);
Z_Key_Value z_map_put(Z_Map *map, void *key, void *value);
void *z_map_get(const Z_Map *map, const void *key);
void *z_map_try_get(const Z_Map *map, const void *key, const void *fallback);
bool z_map_has(const Z_Map *map, void *key);
Z_Key_Value z_map_delete(Z_Map *map, void *key);
Z_Key_Value_Array z_map_to_array(Z_Heap *heap, Z_Map *map);
void z_map_print(const Z_Map *map, Z_Print_Fn print_key, Z_Print_Fn print_value);

#endif