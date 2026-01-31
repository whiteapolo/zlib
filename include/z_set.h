#ifndef SET_H
#define SET_H

#include <z_heap.h>
#include <z_map.h>

typedef Z_Map Z_Set;

Z_Set z_set_new(Z_Heap *heap, Z_Compare_Fn compare_elements);
size_t z_set_size(const Z_Set *set);
void *z_set_add(Z_Set *set, void *element);
bool z_set_has(const Z_Set *set, void *element);
void *z_set_remove(Z_Set *set, void *element);
void z_set_print(const Z_Set *set, Z_Print_Fn print_element);

#endif