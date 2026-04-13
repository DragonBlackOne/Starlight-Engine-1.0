#ifndef STARLIGHT_H
#define STARLIGHT_H

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <cglm/cglm.h>
#include <flecs.h>
#include <stdio.h>
#include <stdbool.h>
#include "slf_events.h"
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "engine.h"
#include "shader.h"
#include "camera.h"
#include "loader.h"
#include "gltf_loader.h"
#include "physics.h"
#include "audio.h"
#include "shadow.h"
#include "skybox.h"
#include "post_process.h"
#include "ssao.h"
#include "terrain.h"
#include "behavior_tree.h"
#include "gizmo.h"
#include "job_system.h"
#include "debug_console.h"
#include "asset_manager.h"
#include "tween.h"
#include "memory_pool.h"

// --- Framework Overhaul: System Logging ---
typedef enum {
    SLF_LOG_LEVEL_INFO,
    SLF_LOG_LEVEL_WARN,
    SLF_LOG_LEVEL_ERROR,
    SLF_LOG_LEVEL_FATAL
} SLF_LogLevel;

void starlight_log(SLF_LogLevel level, const char* file, int line, const char* fmt, ...);

#define SLF_LOG_INFO(...)  starlight_log(SLF_LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define SLF_LOG_WARN(...)  starlight_log(SLF_LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define SLF_LOG_ERROR(...) starlight_log(SLF_LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define SLF_LOG_FATAL(...) starlight_log(SLF_LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

// --- Framework Overhaul: Time Manager ---
typedef struct {
    float frame_delta_time; // Variable dt
    float fixed_delta_time; // Fixed dt for ECS/Physics
    float total_time;
    float time_scale;       // Matrix effect
} SLF_Time;

#define SLF_MAX_ACTIONS 32

struct SLF_Action {
    char name[32];
    int  keys[4];       // Up to 4 keyboard binds
    int  buttons[4];    // Up to 4 gamepad binds
    int  mouse_buttons[4]; // Up to 4 mouse binds
    int  key_count;
    int  button_count;
    int  mouse_button_count;
};
typedef struct SLF_Action SLF_Action;

struct SLF_App {
    Engine engine;
    bool running;
    int width, height;
    float delta_time; // Active legacy delta time (either fixed or scaled frame dt)
    float total_time;
    SLF_Time time;    // New time manager specifics

    // --- UI Batching System ---
    struct {
        float positions[2000];
        float sizes[2000];
        vec4 colors[1000];
        int count;
    } rect_batcher;

    Camera        camera;
    PostProcess   post;
    ShadowMap     shadow;
    GizmoRenderer gizmo;
    ecs_world_t*  world;
    DebugConsole  debug_console;
    
    struct SLF_Font* default_font;
    
    const uint8_t* keys;
    uint8_t prev_keys[512];
    
    // NEW Mouse Input State
    uint32_t mouse_state;
    uint32_t prev_mouse_state;
    int mouse_x, mouse_y;
    int mouse_dx, mouse_dy;
    
    SDL_GameController* controller;
    uint8_t buttons[SDL_CONTROLLER_BUTTON_MAX];
    uint8_t prev_buttons[SDL_CONTROLLER_BUTTON_MAX];
    float axes[SDL_CONTROLLER_AXIS_MAX];

    float view2d_x, view2d_y;
    bool view2d_active;
    
    SLF_Action actions[SLF_MAX_ACTIONS];
    int action_count;

    // --- Framework Overhaul: Scene Stack ---
    struct SLF_Scene* scene_stack[16];
    int scene_count;

    GLuint ui_shader;
    GLuint mode7_shader; // --- Retro Genesis: Mode 7 SNES ---
    GLuint ui_vao, ui_vbo;
    GLuint inst_vbo;
    GLuint text_vao, text_vbo;

    bool post_processing_enabled;
    bool headless; // Dedicated Server Mode sem video
    
    // --- Memory Management ---
    SLF_Arena frame_arena;
    
    // --- Asset Cache ---
    SLF_AssetPool asset_pool;
};
typedef struct SLF_App SLF_App;

typedef void (*SLF_Callback)(SLF_App* app);

struct SLF_Config {
    bool is_headless;
    SLF_Callback on_start;
    SLF_Callback on_update;
    SLF_Callback on_render;
    SLF_Callback on_ui;
    SLF_Callback on_shutdown;
};
typedef struct SLF_Config SLF_Config;

int starlight_framework_init(const char* title, int width, int height, SLF_Config* config);

void starlight_bind_action(SLF_App* app, const char* name, int scancode, int button);
void starlight_bind_action_key(SLF_App* app, const char* name, int scancode);
void starlight_bind_action_button(SLF_App* app, const char* name, int button);
void starlight_bind_action_mouse(SLF_App* app, const char* name, int mousebtn);
bool starlight_is_action_pressed(SLF_App* app, const char* name);
bool starlight_is_action_just_pressed(SLF_App* app, const char* name);
bool starlight_is_mouse_pressed(SLF_App* app, int mousebtn);
bool starlight_is_mouse_just_pressed(SLF_App* app, int mousebtn);

// --- Framework Overhaul: Shader Hot-Reload ---
GLuint starlight_shader_load_watched(SLF_App* app, const char* vert_path, const char* frag_path, GLuint* target_ptr);
void   starlight_shader_check_reload(SLF_App* app);

// --- Framework Overhaul: UI Anchor Layout ---
typedef enum {
    SLF_ANCHOR_TOP_LEFT,
    SLF_ANCHOR_TOP_CENTER,
    SLF_ANCHOR_TOP_RIGHT,
    SLF_ANCHOR_CENTER_LEFT,
    SLF_ANCHOR_CENTER,
    SLF_ANCHOR_CENTER_RIGHT,
    SLF_ANCHOR_BOTTOM_LEFT,
    SLF_ANCHOR_BOTTOM_CENTER,
    SLF_ANCHOR_BOTTOM_RIGHT
} SLF_Anchor;

void starlight_ui_anchor_pos(SLF_App* app, SLF_Anchor anchor, float margin_x, float margin_y, float w, float h, float* out_x, float* out_y);

// --- Framework Overhaul: Scene Management API ---
typedef struct SLF_Scene {
    void (*on_enter)(SLF_App* app);
    void (*on_update)(SLF_App* app, float dt);
    void (*on_draw)(SLF_App* app);
    void (*on_ui)(SLF_App* app);
    void (*on_exit)(SLF_App* app);
    void* user_data;
} SLF_Scene;

void starlight_scene_push(SLF_App* app, SLF_Scene* scene);
void starlight_scene_pop(SLF_App* app);
void starlight_scene_change(SLF_App* app, SLF_Scene* scene);

void starlight_ui_flush_rects(SLF_App* app);
void starlight_ui_draw_rect(SLF_App* app, float x, float y, float w, float h, vec4 color);
void starlight_ui_draw_rects_instanced(SLF_App* app, float* positions, float* sizes, vec4* colors, int count);
void starlight_ui_draw_texture(SLF_App* app, GLuint tex, float x, float y, float w, float h, vec4 color);
void starlight_ui_draw_mode7(SLF_App* app, GLuint tex, float map_x, float map_y, float map_z, float horizon, float angle, float pitch);
#define starlight_preload_texture(app_ptr, path) slf_asset_pool_load_texture(&(app_ptr)->asset_pool, path)

typedef struct {
    GLuint texture_id;
    void* cdata; 
    float size;
} SLF_Font;

SLF_Font* starlight_load_font(const char* path, float size);
void starlight_unload_font(SLF_Font* font);
void starlight_ui_draw_text(SLF_App* app, SLF_Font* font, const char* text, float x, float y, vec4 color);

// --- Retro Genesis: Sprite Sheet Hardware & Animation ---
typedef struct {
    GLuint texture_id;
    int cols, rows;
    int total_frames;
    vec2 frame_size_uv; // normalized 0.0 to 1.0 (1.0/cols, 1.0/rows)
} SLF_SpriteSheet;

typedef struct {
    SLF_SpriteSheet* sheet;
    int start_frame;
    int end_frame;
    float frame_duration;
    float current_time;
    int current_frame;
    bool loop;
    bool finished;
} SLF_SpriteAnimator;

void starlight_sprite_sheet_init(SLF_SpriteSheet* sheet, GLuint tex, int cols, int rows);
void starlight_sprite_animator_init(SLF_SpriteAnimator* anim, SLF_SpriteSheet* sheet, int start, int end, float duration, bool loop);
void starlight_sprite_animator_update(SLF_SpriteAnimator* anim, float dt);
void starlight_ui_draw_sprite(SLF_App* app, SLF_SpriteSheet* sheet, int frame_index, float x, float y, float w, float h, vec4 color, bool flip_x);

// --- Retro Genesis: Parallax Background Engine ---
typedef struct {
    GLuint texture_id;
    float scroll_speed_x;
    float scroll_speed_y;
    float offset_x;
    float offset_y;
    float width, height;
} SLF_ParallaxLayer;

void starlight_parallax_layer_init(SLF_ParallaxLayer* layer, GLuint tex, float spd_x, float spd_y, float w, float h);
void starlight_parallax_layer_update(SLF_ParallaxLayer* layer, float delta_cam_x, float delta_cam_y);
void starlight_ui_draw_parallax(SLF_App* app, SLF_ParallaxLayer* layer, float screen_x, float screen_y, vec4 color);

// --- Retro Genesis: Tilemap Engine & AABB Physics ---
typedef struct {
    int* data;
    int cols, rows;
    float tile_size;
    SLF_SpriteSheet* sheet;
} SLF_Tilemap;

void starlight_tilemap_init(SLF_Tilemap* map, int cols, int rows, float tile_size, SLF_SpriteSheet* sheet);
void starlight_tilemap_destroy(SLF_Tilemap* map);
void starlight_tilemap_set_data(SLF_Tilemap* map, int* map_data);
void starlight_ui_draw_tilemap(SLF_App* app, SLF_Tilemap* map, float offset_x, float offset_y);
// Basic Kinematic AABB resolution against Solid Tiles (indices > 0)
void starlight_physics_tilemap_collide(SLF_Tilemap* map, float* px, float* py, float p_width, float p_height, float* p_vx, float* p_vy);

#endif
