#ifndef SCENE_GRAPH_H
#define SCENE_GRAPH_H

#include <cglm/cglm.h>
#include <stdint.h>

typedef struct SceneNode {
    mat4 local_matrix;
    mat4 world_matrix;
    
    struct SceneNode* parent;
    struct SceneNode** children;
    int child_count;
    int child_capacity;

    uint32_t entity_id; // Link com ECS se necessário
    bool dirty;
} SceneNode;

/**
 * @brief Cria um novo nodo na cena.
 */
SceneNode* scene_node_create(void);

/**
 * @brief Adiciona um filho a um nodo pai.
 */
void scene_node_add_child(SceneNode* parent, SceneNode* child);

/**
 * @brief Atualiza as matrizes de mundo recursivamente.
 */
void scene_node_update(SceneNode* node, mat4 parent_world_matrix);

/**
 * @brief Define a transformacao local do nodo.
 */
void scene_node_set_transform(SceneNode* node, vec3 pos, vec3 rot, vec3 scale);

#endif
