#ifndef SHADOW_H
#define SHADOW_H

#include <glad/glad.h>
#include <cglm/cglm.h>

#define MAX_CASCADES 4

typedef struct {
    GLuint fbo;
    GLuint depth_texture_array;
    GLuint shader_program;
    int resolution;
    int num_cascades;
    float cascade_splits[MAX_CASCADES];
    mat4 light_space_matrices[MAX_CASCADES];
} ShadowMap;

/**
 * @brief Inicializa o sistema de Cascaded Shadow Maps.
 */
bool shadow_init(ShadowMap* shadow, int resolution, int num_cascades);

/**
 * @brief Calcula as matrizes de projecao para cada cascata baseadas no frustum da camera.
 */
void shadow_calculate_cascades(ShadowMap* shadow, mat4 view, mat4 projection, vec3 light_dir, float near, float far);

/**
 * @brief Inicia o passe de renderizacao de sombras para uma cascata especifica.
 */
void shadow_begin_pass(ShadowMap* shadow, int cascade_index);

/**
 * @brief Finaliza o passe de sombras e restaura o viewport.
 */
void shadow_end_pass(int screen_width, int screen_height);

/**
 * @brief Faz o bind do array de texturas de sombra para uso no shader PBR.
 */
void shadow_bind_texture(ShadowMap* shadow, int texture_unit);

/**
 * @brief Destroi os recursos de sombra.
 */
void shadow_destroy(ShadowMap* shadow);

#endif // SHADOW_H
