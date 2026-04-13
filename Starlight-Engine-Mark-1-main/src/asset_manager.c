#include "asset_manager.h"
#include "loader.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// --- Framework Overhaul: Asset Pool Implementation ---

void slf_asset_pool_init(SLF_AssetPool* pool) {
    memset(pool, 0, sizeof(SLF_AssetPool));
    printf("[ASSET_POOL] Initialized (capacity: %d slots)\n", SLF_ASSET_MAX);
}

// Internal: find existing entry by path
static SLF_AssetEntry* find_entry(SLF_AssetPool* pool, const char* path) {
    for (int i = 0; i < pool->count; i++) {
        if (pool->entries[i].active && strcmp(pool->entries[i].path, path) == 0) {
            return &pool->entries[i];
        }
    }
    return NULL;
}

// Internal: allocate a new slot
static SLF_AssetEntry* alloc_entry(SLF_AssetPool* pool, const char* path, SLF_AssetType type) {
    if (pool->count >= SLF_ASSET_MAX) {
        printf("[ASSET_POOL] WARNING: Pool full! Cannot cache '%s'\n", path);
        return NULL;
    }
    SLF_AssetEntry* e = &pool->entries[pool->count++];
    memset(e, 0, sizeof(SLF_AssetEntry));
    snprintf(e->path, SLF_ASSET_PATH_LEN, "%s", path);
    e->type = type;
    e->ref_count = 1;
    e->active = true;
    return e;
}

GLuint slf_asset_pool_load_texture(SLF_AssetPool* pool, const char* path) {
    // Check cache first
    SLF_AssetEntry* existing = find_entry(pool, path);
    if (existing) {
        existing->ref_count++;
        printf("[ASSET_POOL] Cache HIT '%s' (refs: %d)\n", path, existing->ref_count);
        return existing->gl_handle;
    }

    // Cache MISS: Load from disk
    GLuint tex = loader_load_texture(path);
    if (tex == 0) {
        printf("[ASSET_POOL] FAILED to load texture '%s'\n", path);
        return 0;
    }

    SLF_AssetEntry* entry = alloc_entry(pool, path, SLF_ASSET_TEXTURE);
    if (entry) {
        entry->gl_handle = tex;
        printf("[ASSET_POOL] Loaded & cached texture '%s' (GL:%u, slot:%d)\n", path, tex, pool->count - 1);
    }
    return tex;
}

void* slf_asset_pool_load_font(SLF_AssetPool* pool, const char* path, float size) {
    SLF_AssetEntry* existing = find_entry(pool, path);
    if (existing) {
        existing->ref_count++;
        printf("[ASSET_POOL] Cache HIT font '%s' (refs: %d)\n", path, existing->ref_count);
        return existing->data;
    }

    // Fonts use the starlight_load_font from slf.c (declared in starlight.h)
    // We store the pointer generically
    extern void* starlight_load_font(const char* path, float size);
    void* font = starlight_load_font(path, size);
    if (!font) {
        printf("[ASSET_POOL] FAILED to load font '%s'\n", path);
        return NULL;
    }

    SLF_AssetEntry* entry = alloc_entry(pool, path, SLF_ASSET_FONT);
    if (entry) {
        entry->data = font;
        printf("[ASSET_POOL] Loaded & cached font '%s' (slot:%d)\n", path, pool->count - 1);
    }
    return font;
}

void slf_asset_pool_release(SLF_AssetPool* pool, const char* path) {
    SLF_AssetEntry* e = find_entry(pool, path);
    if (!e) return;
    e->ref_count--;
    if (e->ref_count <= 0) {
        if (e->type == SLF_ASSET_TEXTURE && e->gl_handle) {
            glDeleteTextures(1, &e->gl_handle);
        }
        // Fonts/Sounds freed by their own destructors
        e->active = false;
        printf("[ASSET_POOL] Released '%s'\n", path);
    }
}

void slf_asset_pool_destroy(SLF_AssetPool* pool) {
    for (int i = 0; i < pool->count; i++) {
        if (pool->entries[i].active && pool->entries[i].type == SLF_ASSET_TEXTURE) {
            glDeleteTextures(1, &pool->entries[i].gl_handle);
        }
        pool->entries[i].active = false;
    }
    pool->count = 0;
    printf("[ASSET_POOL] Destroyed all cached assets.\n");
}

int slf_asset_pool_count(SLF_AssetPool* pool) {
    int active = 0;
    for (int i = 0; i < pool->count; i++) {
        if (pool->entries[i].active) active++;
    }
    return active;
}
