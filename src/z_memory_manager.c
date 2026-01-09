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
  if (manager->mode == Z_MEMORY_MANAGER_MODE_SUPER_HEAVY) {
    return z_arena_alocate(&manager->as.arena, bytes);
  } else {
    return z_heap_malloc(&manager->as.heap, bytes);
  }
}

void *z_memory_manager_realloc(Z_Memory_Manager *manager, void *ptr, size_t bytes)
{
  if (manager->mode == Z_MEMORY_MANAGER_MODE_SUPER_HEAVY) {
    return z_arena_alocate(&manager->as.arena, bytes);
  } else {
    return z_heap_realloc(&manager->as.heap, ptr, bytes);
  }
}

void z_memory_manager_free_pointer(Z_Memory_Manager *manager, void *ptr)
{
  if (manager->mode == Z_MEMORY_MANAGER_MODE_SUPER_HEAVY) {
    // nothing
  } else {
    z_heap_free_pointer(&manager->as.heap, ptr);
  }
}

void z_memory_manager_reset(Z_Memory_Manager *manager)
{
  if (manager->mode == Z_MEMORY_MANAGER_MODE_SUPER_HEAVY) {
    z_arena_reset(&manager->as.arena);
  } else {
    z_heap_reset(&manager->as.heap);
  }
}

void z_memory_manager_free_all(Z_Memory_Manager *manager)
{
  if (manager->mode == Z_MEMORY_MANAGER_MODE_SUPER_HEAVY) {
    z_arena_free(&manager->as.arena);
  } else {
    z_heap_free(&manager->as.heap);
  }
}
