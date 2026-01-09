#ifndef Z_MEMORY_MANAGER_H
#define Z_MEMORY_MANAGER_H

#include <z_heap.h>
#include <z_arena.h>

typedef enum {
  Z_MEMORY_MANAGER_MODE_SUPER_LIGHT,
  Z_MEMORY_MANAGER_MODE_SUPER_HEAVY,
} Z_Memory_Manager_Mode;

typedef struct {
  void *(*malloc)(Z_Memory_Manager *manager, size_t size);
  void (*realloc)(Z_Memory_Manager *manager, void *ptr, size_t size);
  void (*free_pointer)(Z_Memory_Manager *manager, void *ptr);
  void (*reset)(Z_Memory_Manager *manager);
  void (*free_all)(Z_Memory_Manager *manager);
  union {
    Z_Heap heap;
    Z_Arena arena;
  } as;
} Z_Memory_Manager;

Z_Memory_Manager z_memory_manager_new(Z_Memory_Manager_Mode mode);
void *z_memory_manager_malloc(Z_Memory_Manager *manager, size_t bytes);
void *z_memory_manager_realloc(Z_Memory_Manager *manager, void *ptr, size_t bytes);
void z_memory_manager_free_pointer(Z_Memory_Manager *manager, void *ptr);
void z_memory_manager_reset(Z_Memory_Manager *manager);
void z_memory_manager_free_all(Z_Memory_Manager *manager);

#endif
