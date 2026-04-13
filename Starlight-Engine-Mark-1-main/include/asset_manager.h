#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <glad/glad.h>
#include <stdbool.h>

// --- Framework Overhaul: Intelligent Asset Cache ---
// Prevents duplicate VRAM/RAM allocations by caching loaded resources by path.

#define SLF_ASSET_MAX 256
#define SLF_ASSET_PATH_LEN 128

typedef enum {
    SLF_ASSET_TEXTURE,
    SLF_ASSET_FONT,
    SLF_ASSET_SOUND
} SLF_AssetType;

typedef struct {
    char path[SLF_ASSET_PATH_LEN];
    SLF_AssetType type;
    GLuint gl_handle;   // For textures/shaders
    void* data;         // For fonts/sounds (generic pointer)
    int ref_count;
    bool active;
} SLF_AssetEntry;

typedef struct {
    SLF_AssetEntry entries[SLF_ASSET_MAX];
    int count;
} SLF_AssetPool;

void     slf_asset_pool_init(SLF_AssetPool* pool);
GLuint   slf_asset_pool_load_texture(SLF_AssetPool* pool, const char* path);
void*    slf_asset_pool_load_font(SLF_AssetPool* pool, const char* path, float size);
void     slf_asset_pool_release(SLF_AssetPool* pool, const char* path);
void     slf_asset_pool_destroy(SLF_AssetPool* pool);
int      slf_asset_pool_count(SLF_AssetPool* pool);

#endif
