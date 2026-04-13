#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/glad.h>
#include <cglm/cglm.h>
#include <stdbool.h>

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint texture_id;
    GLuint shader_program;
} Skybox;

// Inicializa o skybox com 6 faces (diretorio base + nomes das faces)
bool skybox_init(Skybox* skybox, const char* faces[6]);

// Renderiza o skybox usando as matrizes de projecao e view (view deve ter translacao zerada)
void skybox_draw(Skybox* skybox, mat4 projection, mat4 view);

// Limpa recursos
void skybox_destroy(Skybox* skybox);

#endif
