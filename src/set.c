#include <zatar/set.h>

Z_Set z_set_new(Z_Heap *heap, Z_Compare_Fn compare_elements)
{
  return z_map_new(heap, compare_elements);
}

size_t z_set_size(const Z_Set *set)
{
  return z_map_size(set);
}

void *z_set_add(Z_Set *set, void *element)
{
  Z_Key_Value old_key_value = z_map_put(set, element, NULL);
  return old_key_value.key;
}

bool z_set_has(const Z_Set *set, void *element)
{
  return z_map_has(set, element);
}

void *z_set_remove(Z_Set *set, void *element)
{
  Z_Key_Value deleted = z_map_delete(set, element);
  return deleted.key;
}

void z__set_print_nothing(const void *)
{ }

void z_set_print(const Z_Set *set, Z_Print_Fn print_element)
{
  z_map_print(set, print_element, z__set_print_nothing);
}