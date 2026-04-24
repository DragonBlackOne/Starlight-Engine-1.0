// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "memory_pool.h"
#include <stdlib.h>
#include <string.h>

// --- Linear Frame Arena ---
bool starlight_arena_init(SLF_Arena* arena, size_t size) {
    if (!arena || size == 0) return false;
    arena->buffer = (uint8_t*)malloc(size);
    if (!arena->buffer) return false;
    arena->capacity = size;
    arena->offset = 0;
    return true;
}

void* starlight_arena_alloc(SLF_Arena* arena, size_t size) {
    if (!arena || !arena->buffer) return NULL;
    // Align to 8 bytes
    size_t aligned_size = (size + 7) & ~7;
    
    if (arena->offset + aligned_size > arena->capacity) {
        return NULL; // OOM in arena
    }
    
    void* ptr = arena->buffer + arena->offset;
    arena->offset += aligned_size;
    return ptr;
}

void starlight_arena_reset(SLF_Arena* arena) {
    if (arena) arena->offset = 0;
}

void starlight_arena_destroy(SLF_Arena* arena) {
    if (arena && arena->buffer) {
        free(arena->buffer);
        arena->buffer = NULL;
        arena->capacity = 0;
        arena->offset = 0;
    }
}

// --- Object Pool ---
bool starlight_pool_init(SLF_Pool* pool, size_t object_size, size_t capacity) {
    if (!pool || object_size == 0 || capacity == 0) return false;
    
    // Ensure object_size is large enough to hold a pointer for the free list
    if (object_size < sizeof(void*)) {
        object_size = sizeof(void*);
    }
    // Align to 8 bytes
    object_size = (object_size + 7) & ~7; 
    
    pool->buffer = (uint8_t*)malloc(object_size * capacity);
    pool->free_list = (void**)malloc(sizeof(void*) * capacity);
    
    if (!pool->buffer || !pool->free_list) {
        if (pool->buffer) free(pool->buffer);
        if (pool->free_list) free(pool->free_list);
        return false;
    }
    
    pool->object_size = object_size;
    pool->capacity = capacity;
    pool->free_count = capacity;
    
    // Populate the free list with pointers to all blocks
    for (size_t i = 0; i < capacity; i++) {
        pool->free_list[i] = pool->buffer + (i * object_size);
    }
    
    return true;
}

void* starlight_pool_alloc(SLF_Pool* pool) {
    if (!pool || pool->free_count == 0) return NULL;
    
    // Pop a free block from the list
    pool->free_count--;
    return pool->free_list[pool->free_count];
}

void starlight_pool_free(SLF_Pool* pool, void* ptr) {
    if (!pool || !ptr) return;
    
    // Boundary check 
    if ((uint8_t*)ptr < pool->buffer || (uint8_t*)ptr >= pool->buffer + (pool->capacity * pool->object_size)) {
        return; // Pointer is outside the pool
    }
    
    if (pool->free_count < pool->capacity) {
        // Push the block back onto the free list
        pool->free_list[pool->free_count] = ptr;
        pool->free_count++;
    }
}

void starlight_pool_destroy(SLF_Pool* pool) {
    if (pool) {
        if (pool->buffer) free(pool->buffer);
        if (pool->free_list) free(pool->free_list);
        pool->buffer = NULL;
        pool->free_list = NULL;
        pool->capacity = 0;
        pool->free_count = 0;
    }
}
