// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "octree.h"
#include "memory.h"
#include <stdlib.h>
#include <string.h>

// Arena global para a Octree (resetada por frame)
static MemoryArena octree_arena;
static bool arena_initialized = false;

Octree* octree_create(AABB bounds) {
    if (!arena_initialized) {
        arena_init(&octree_arena, 1024 * 1024); 
        arena_initialized = true;
    }
    arena_reset(&octree_arena);

    Octree* tree = (Octree*)arena_alloc(&octree_arena, sizeof(Octree));
    tree->world_bounds = bounds;
    
    OctreeNode* root = (OctreeNode*)arena_alloc(&octree_arena, sizeof(OctreeNode));
    root->bounds = bounds;
    root->is_leaf = true;
    root->entity_count = 0;
    root->entities = (uint32_t*)arena_alloc(&octree_arena, sizeof(uint32_t) * OCTREE_MAX_ENTITIES);
    memset(root->children, 0, sizeof(root->children));
    
    tree->root = root;
    return tree;
}

static bool aabb_contains(AABB parent, AABB child) {
    return child.min[0] >= parent.min[0] && child.max[0] <= parent.max[0] &&
           child.min[1] >= parent.min[1] && child.max[1] <= parent.max[1] &&
           child.min[2] >= parent.min[2] && child.max[2] <= parent.max[2];
}

static void octree_split(OctreeNode* node) {
    node->is_leaf = false;
    vec3 center;
    glm_vec3_add(node->bounds.min, node->bounds.max, center);
    glm_vec3_scale(center, 0.5f, center);

    for (int i = 0; i < 8; i++) {
        OctreeNode* child = (OctreeNode*)arena_alloc(&octree_arena, sizeof(OctreeNode));
        child->is_leaf = true;
        child->entity_count = 0;
        child->entities = (uint32_t*)arena_alloc(&octree_arena, sizeof(uint32_t) * OCTREE_MAX_ENTITIES);
        memset(child->children, 0, sizeof(child->children));
        
        AABB b;
        b.min[0] = (i & 1) ? center[0] : node->bounds.min[0];
        b.max[0] = (i & 1) ? node->bounds.max[0] : center[0];
        b.min[1] = (i & 2) ? center[1] : node->bounds.min[1];
        b.max[1] = (i & 2) ? node->bounds.max[1] : center[1];
        b.min[2] = (i & 4) ? center[2] : node->bounds.min[2];
        b.max[2] = (i & 4) ? node->bounds.max[2] : center[2];
        child->bounds = b;
        
        node->children[i] = child;
    }
}

void octree_insert(Octree* tree, uint32_t entity_id, AABB entity_aabb) {
    OctreeNode* current = tree->root;
    if (current->is_leaf && current->entity_count < OCTREE_MAX_ENTITIES) {
        current->entities[current->entity_count++] = entity_id;
    } else if (current->is_leaf) {
        octree_split(current);
    }
    
    if (!current->is_leaf) {
        for (int i = 0; i < 8; i++) {
            if (aabb_contains(current->children[i]->bounds, entity_aabb)) {
                current->children[i]->entities[current->children[i]->entity_count++] = entity_id;
                break;
            }
        }
    }
}

static void query_recursive(OctreeNode* node, Frustum* frustum, uint32_t* out_entities, int* out_count) {
    if (!camera_check_aabb(frustum, &node->bounds)) return;

    if (node->is_leaf) {
        for (int i = 0; i < node->entity_count; i++) {
            out_entities[(*out_count)++] = node->entities[i];
        }
    } else {
        for (int i = 0; i < 8; i++) {
            if (node->children[i]) {
                query_recursive(node->children[i], frustum, out_entities, out_count);
            }
        }
    }
}

void octree_query_frustum(Octree* tree, Frustum* frustum, uint32_t* out_entities, int* out_count) {
    *out_count = 0;
    query_recursive(tree->root, frustum, out_entities, out_count);
}

void octree_destroy(Octree* tree) {}
