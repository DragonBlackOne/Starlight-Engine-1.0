// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef STARLIGHT_MEMORY_POOL_H
#define STARLIGHT_MEMORY_POOL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// --- Linear Frame Arena ---
// Resets completely every frame. Very fast allocation.
typedef struct {
    uint8_t* buffer;
    size_t capacity;
    size_t offset;
} SLF_Arena;

bool starlight_arena_init(SLF_Arena* arena, size_t size);
void* starlight_arena_alloc(SLF_Arena* arena, size_t size);
void starlight_arena_reset(SLF_Arena* arena);
void starlight_arena_destroy(SLF_Arena* arena);

// --- Object Pool ---
// Fixed size object allocator. Good for particles and spatial hash grids.
typedef struct {
    uint8_t* buffer;
    void** free_list;
    size_t object_size;
    size_t capacity;
    size_t free_count;
} SLF_Pool;

bool starlight_pool_init(SLF_Pool* pool, size_t object_size, size_t capacity);
void* starlight_pool_alloc(SLF_Pool* pool);
void starlight_pool_free(SLF_Pool* pool, void* ptr);
void starlight_pool_destroy(SLF_Pool* pool);

#endif
