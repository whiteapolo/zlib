#include <z_memory_manager.h>
#include <z_arena.h>
#include <z_heap.h>

Z_Memory_Manager z_memory_manager_new(Z_Memory_Manager_Mode mode)
{
  Z_Memory_Manager manager = {
    .mode = mode,
  };

  if (mode == Z_MEMORY_MANAGER_MODE_SUPER_HEAVY) {
    manager.as.arena = z_arena_new();
  }
}

void *z_memory_manager_malloc(Z_Memory_Manager *manager, size_t bytes)
{
  if (manager->mode == Z_MEMORY_MANAGER_MODE_SUPER_HEAVY) return z_arena_alocate(&manager->as.arena, bytes);
  return z_heap_malloc(&manager->as.heap, bytes);
}

void *z_memory_manager_realloc(Z_Memory_Manager *manager, void *ptr, size_t bytes)
{
  if (manager->mode == Z_MEMORY_MANAGER_MODE_SUPER_HEAVY) return z_arena_alocate(&manager->as.arena, bytes);
  return z_heap_realloc(&manager->as.heap, ptr, bytes);
}

void z_memory_manager_free_pointer(Z_Memory_Manager *manager, void *ptr)
{

}

void z_memory_manager_reset(Z_Memory_Manager *manager);
void z_memory_manager_free_all(Z_Memory_Manager *manager);
