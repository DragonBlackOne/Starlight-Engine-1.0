// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef LOD_H
#define LOD_H

#include <cglm/cglm.h>
#include <stdint.h>

#define MAX_LOD_LEVELS 3

typedef struct {
    uint32_t vao;
    uint32_t index_count;
    float distance_threshold; // Distancia para trocar para este nivel
} LODLevel;

typedef struct {
    LODLevel levels[MAX_LOD_LEVELS];
    int count;
    int current_lod;
} LODGroup;

/**
 * @brief Seleciona o nivel de detalhe correto baseado na distancia da camera.
 */
int lod_select_level(LODGroup* group, float distance);

#endif
