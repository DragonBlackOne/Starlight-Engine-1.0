#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <stdbool.h>

// Compila um Vertex e Fragment Shader em memoria
GLuint shader_create_program(const char* vertex_source, const char* fragment_source);

// Carrega o shader do disco direto pra VRAM
GLuint shader_load_program(const char* vertex_path, const char* fragment_path);

// Deleta o programa de shader da GPU
void shader_delete(GLuint program);

#endif
