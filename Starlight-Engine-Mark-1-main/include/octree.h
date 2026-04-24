// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef OCTREE_H
#define OCTREE_H

#include "camera.h"
#include <stdbool.h>

#define OCTREE_MAX_ENTITIES 8
#define OCTREE_MAX_DEPTH 5

typedef struct OctreeNode {
    AABB bounds;
    struct OctreeNode* children[8];
    uint32_t* entities;
    int entity_count;
    bool is_leaf;
} OctreeNode;

typedef struct {
    OctreeNode* root;
    AABB world_bounds;
} Octree;

/**
 * @brief Inicializa a Octree com limites globais.
 */
Octree* octree_create(AABB bounds);

/**
 * @brief Insere uma entidade na Octree.
 */
void octree_insert(Octree* tree, uint32_t entity_id, AABB entity_aabb);

/**
 * @brief Consulta entidades que estao dentro do Frustum.
 */
void octree_query_frustum(Octree* tree, Frustum* frustum, uint32_t* out_entities, int* out_count);

/**
 * @brief Destroi a Octree e libera memoria.
 */
void octree_destroy(Octree* tree);

#endif
