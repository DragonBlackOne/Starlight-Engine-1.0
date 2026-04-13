#ifndef STARLIGHT_MEMORY_H
#define STARLIGHT_MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    void*  base;
    size_t size;
    size_t offset;
} MemoryArena;

/**
 * @brief Inicializa uma arena de memoria com um tamanho fixo.
 */
bool arena_init(MemoryArena* arena, size_t size);

/**
 * @brief Aloca um bloco de memoria na arena.
 */
void* arena_alloc(MemoryArena* arena, size_t size);

/**
 * @brief Reseta o offset da arena (liberacao instantanea de toda a memoria alocada).
 */
void arena_reset(MemoryArena* arena);

/**
 * @brief Destroi a arena e libera o bloco base.
 */
void arena_destroy(MemoryArena* arena);

#endif // STARLIGHT_MEMORY_H
