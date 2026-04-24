// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool arena_init(MemoryArena* arena, size_t size) {
    arena->base = malloc(size);
    if (!arena->base) {
        printf("[MEMORY] Falha ao alocar %zu bytes para a Memory Arena!\n", size);
        return false;
    }
    arena->size = size;
    arena->offset = 0;
    memset(arena->base, 0, size);
    return true;
}

void* arena_alloc(MemoryArena* arena, size_t size) {
    // Alinhamento para performance (8 bytes)
    size_t align = 8;
    size_t aligned_size = (size + align - 1) & ~(align - 1);

    if (arena->offset + aligned_size > arena->size) {
        printf("[MEMORY] Arena out of memory! (Alloc %zu, Total %zu, Used %zu)\n", 
               aligned_size, arena->size, arena->offset);
        return NULL;
    }

    void* ptr = (uint8_t*)arena->base + (uintptr_t)arena->offset;
    arena->offset += aligned_size;
    return ptr;
}

void arena_reset(MemoryArena* arena) {
    arena->offset = 0;
}

void arena_destroy(MemoryArena* arena) {
    if (arena->base) {
        free(arena->base);
        arena->base = NULL;
    }
    arena->size = 0;
    arena->offset = 0;
}
