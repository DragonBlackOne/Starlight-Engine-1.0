#ifndef SSAO_H
#define SSAO_H

#include <glad/glad.h>
#include <cglm/cglm.h>

typedef struct {
    GLuint fbo;
    GLuint ssao_color_buffer;
    
    GLuint blur_fbo;
    GLuint blur_color_buffer;

    GLuint noise_texture;
    GLuint ssao_shader;
    GLuint blur_shader;
    
    vec3 ssao_kernel[64];
} SSAO;

/**
 * @brief Inicializa o sistema de SSAO com buffer de blur.
 */
bool ssao_init(SSAO* ssao, int width, int height);

/**
 * @brief Renderiza a oclusao de ambiente baseada no depth buffer.
 */
void ssao_render(SSAO* ssao, GLuint depth_texture, mat4 projection, mat4 view, int width, int height);

/**
 * @brief Libera os recursos do SSAO.
 */
void ssao_destroy(SSAO* ssao);

#endif
