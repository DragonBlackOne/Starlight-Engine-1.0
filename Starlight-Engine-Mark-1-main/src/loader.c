#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "loader.h"
#include "job_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOADER_ARENA_SIZE (1024 * 1024 * 4) // 4MB for metadata and small jobs
static uint8_t* g_loader_arena_base = NULL;
static size_t   g_loader_arena_offset = 0;

void loader_init_arena(void) {
    if (!g_loader_arena_base) {
        g_loader_arena_base = malloc(LOADER_ARENA_SIZE);
        g_loader_arena_offset = 0;
        printf("[LOADER] Arena initialized: %d MB\n", LOADER_ARENA_SIZE / (1024*1024));
    }
}

void* loader_alloc_metadata(size_t size) {
    if (g_loader_arena_offset + size > LOADER_ARENA_SIZE) {
        printf("[LOADER] ARENA OVERFLOW!\n");
        return malloc(size); // Fallback to heap if arena is full
    }
    void* ptr = g_loader_arena_base + g_loader_arena_offset;
    g_loader_arena_offset += size;
    return ptr;
}

void loader_shutdown_arena(void) {
    if (g_loader_arena_base) {
        free(g_loader_arena_base);
        g_loader_arena_base = NULL;
    }
}

static void texture_decompress_job(void* data) {
    TextureLoadJob* job = (TextureLoadJob*)data;
    job->pixel_data = stbi_load(job->path, &job->width, &job->height, &job->channels, 0);
    
    if (job->pixel_data) {
        // Ryzen 5500 + RX 550: Textures are uploaded efficiently in the main thread
        job->ready_for_gpu = true;
    } else {
        printf("[LOADER] Async fail: %s\n", job->path);
    }
}

GLuint loader_load_texture(const char* filepath) {
    int width, height, channels;
    unsigned char* data = stbi_load(filepath, &width, &height, &channels, 0);
    if (!data) return 0;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return tex;
}

void loader_async_load_texture(const char* filepath, TextureLoadJob* out_job) {
    out_job->path = filepath;
    out_job->ready_for_gpu = false;
    out_job->pixel_data = NULL;
    
    job_system_push(texture_decompress_job, out_job, NULL);
}

GLuint loader_load_texture_streamed(const char* filepath, TextureLoadJob* out_job) {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    unsigned char dummy[3] = {255, 0, 255}; // Placeholder Magenta (1x1)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, dummy);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    out_job->target_gl_id = tex;
    loader_async_load_texture(filepath, out_job);
    return tex;
}

bool loader_finalize_texture(TextureLoadJob* job) {
    if (!job->ready_for_gpu || !job->pixel_data) return false;

    // Se já havia um texture ID reservado (Streaming mode), nós o sobrescrevemos.
    // Senão, geramos um novo id silenciosamente. (O retorno foi modificado no header, então este modo clássico estaria defasado,
    // mas deixaremos safe para evitar breakages).
    if (job->target_gl_id == 0) {
        glGenTextures(1, &job->target_gl_id);
    }
    
    glBindTexture(GL_TEXTURE_2D, job->target_gl_id);
    
    GLenum format = (job->channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, job->width, job->height, 0, format, GL_UNSIGNED_BYTE, job->pixel_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(job->pixel_data);
    job->pixel_data = NULL;
    job->ready_for_gpu = false;
    
    return true;
}
