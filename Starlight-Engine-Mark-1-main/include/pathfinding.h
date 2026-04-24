// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <cglm/cglm.h>
#include <stdbool.h>

typedef struct PathNode {
    vec3 position;
    struct PathNode* parent;
    float g_score;
    float h_score;
    float f_score;
    bool is_walkable;
} PathNode;

/**
 * @brief Encontra o caminho mais curto entre dois pontos usando A*.
 */
bool pathfinding_find_path(vec3 start, vec3 target, PathNode* grid, int width, int height, vec3* out_path, int* out_count);

#endif
