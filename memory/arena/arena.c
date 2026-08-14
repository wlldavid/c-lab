#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/mman.h>

typedef struct {
  void *mem;
  size_t offset;
  size_t size;
} Arena;

Arena arena_create(size_t size) {
  Arena arena = {0};

  if (size <= 0)
    return arena;

  void *mem = mmap(NULL, size, PROT_WRITE | PROT_READ,
                   MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

  if (mem == MAP_FAILED)
    return arena;

  arena.mem = mem;
  arena.size = size;
  arena.offset = 0;

  return arena;
}

void *arena_alloc(Arena *arena, size_t size) {
  if (arena == NULL || arena->mem == NULL || arena->size <= 0) {
    return NULL;
  }

  if (arena->size < size || arena->size - arena->offset < size) {
    return NULL;
  }

  void *pointer = arena->mem + arena->offset;
  arena->offset += size;

  return pointer;
}

bool arena_reset(Arena *arena) {
  if (arena == NULL || arena->mem == NULL || arena->size <= 0)
    return false;

  arena->offset = 0;

  return true;
}

bool arena_destroy(Arena *arena) {
  if (arena == NULL || arena->mem == NULL || arena->size <= 0)
    return false;

  if (munmap(arena->mem, arena->size)) {
    return false;
  }

  *arena = (Arena){0};

  return true;
}
