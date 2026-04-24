// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "scene_graph.h"
#include "memory.h"
#include <stdlib.h>
#include <string.h>

static MemoryArena scene_arena;
static bool arena_init_done = false;

SceneNode* scene_node_create(void) {
    if (!arena_init_done) {
        arena_init(&scene_arena, 2 * 1024 * 1024); // 2MB para nodos de cena
        arena_init_done = true;
    }

    SceneNode* node = (SceneNode*)arena_alloc(&scene_arena, sizeof(SceneNode));
    glm_mat4_identity(node->local_matrix);
    glm_mat4_identity(node->world_matrix);
    node->parent = NULL;
    node->children = NULL;
    node->child_count = 0;
    node->child_capacity = 0;
    node->dirty = true;
    return node;
}

void scene_node_add_child(SceneNode* parent, SceneNode* child) {
    if (parent->child_count >= parent->child_capacity) {
        int new_cap = (parent->child_capacity == 0) ? 4 : parent->child_capacity * 2;
        SceneNode** new_children = (SceneNode**)arena_alloc(&scene_arena, sizeof(SceneNode*) * new_cap);
        if (parent->children) memcpy(new_children, parent->children, sizeof(SceneNode*) * parent->child_count);
        parent->children = new_children;
        parent->child_capacity = new_cap;
    }
    parent->children[parent->child_count++] = child;
    child->parent = parent;
}

void scene_node_update(SceneNode* node, mat4 parent_matrix) {
    if (node->dirty) {
        glm_mat4_mul(parent_matrix, node->local_matrix, node->world_matrix);
        // node->dirty = false; // Em um grafo real, propagamos o dirty flag
    }

    for (int i = 0; i < node->child_count; i++) {
        scene_node_update(node->children[i], node->world_matrix);
    }
}

void scene_node_set_transform(SceneNode* node, vec3 pos, vec3 rot, vec3 scale) {
    glm_mat4_identity(node->local_matrix);
    glm_translate(node->local_matrix, pos);
    glm_rotate_x(node->local_matrix, rot[0], node->local_matrix);
    glm_rotate_y(node->local_matrix, rot[1], node->local_matrix);
    glm_rotate_z(node->local_matrix, rot[2], node->local_matrix);
    glm_scale(node->local_matrix, scale);
    node->dirty = true;
}
