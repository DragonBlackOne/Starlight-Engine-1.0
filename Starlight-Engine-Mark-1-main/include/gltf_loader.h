#ifndef GLTF_LOADER_H
#define GLTF_LOADER_H

#include <glad/glad.h>
#include <cglm/cglm.h>
#include <stdbool.h>

// Mesh primitiva de GLTF
typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    int vertex_count;
    int index_count;
    bool has_indices;
    bool has_skin;
} GLTFMesh;

// Animacao Keyframe
typedef struct {
    char name[64];
    float duration;
} GLTFAnimation;

// Modelo GLTF completo (multiplas meshes e esqueleto)
typedef struct {
    GLTFMesh* meshes;
    int mesh_count;
    GLuint* textures;
    int texture_count;
    
    // Skeletal info
    mat4* joint_matrices;
    int joint_count;
    GLTFAnimation* animations;
    int animation_count;
} GLTFModel;

// API
bool gltf_load(const char* filepath, GLTFModel* out_model);
void gltf_draw(GLTFModel* model);
void gltf_destroy(GLTFModel* model);

#endif
