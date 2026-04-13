#ifndef LOADER_H
#define LOADER_H

#include <glad/glad.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    int vertex_count;
    int index_count;
    GLuint texture_id;
    bool has_indices;
} MeshRenderer;

typedef struct {
    const char* path;
    int width, height, channels;
    unsigned char* pixel_data;
    bool ready_for_gpu;
    GLuint target_gl_id; // For Async Streaming (Placeholder ID)
} TextureLoadJob;

/**
 * @brief Inicializa a arena de memoria do loader.
 */
void loader_init_arena(void);

void* loader_alloc_metadata(size_t size);

/**
 * @brief Carrega um modelo OBJ de forma sincrona.
 */
bool loader_load_obj(const char* filepath, MeshRenderer* out_mesh);

/**
 * @brief Carrega uma textura de forma sincrona.
 */
GLuint loader_load_texture(const char* filepath);

/**
 * @brief Carrega uma textura de forma streamada.
 */
GLuint loader_load_texture_streamed(const char* filepath, TextureLoadJob* out_job);

/**
 * @brief Inicia o carregamento de uma textura em background (Job System).
 */
void loader_async_load_texture(const char* filepath, TextureLoadJob* out_job);

/**
 * @brief Finaliza o carregamento enviando os dados para a GPU.
 */
bool loader_finalize_texture(TextureLoadJob* job);

/**/
void loader_shutdown_arena(void);

#endif
