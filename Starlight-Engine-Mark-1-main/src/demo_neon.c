#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <cglm/cglm.h>
#include <flecs.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "starlight.h"

// --- DEMO COMPONENTS ---
typedef struct {
    vec3 position;
    float timer;
} AIAgent;

static GLuint g_shader = 0;
static GLuint g_white_tex = 0;
static AIAgent g_neon_bot = { .position = {50, 12, 50}, .timer = 0 };
static BTNode* g_bt_root = NULL;
static Terrain g_terrain;
static GizmoRenderer g_gr;

BTStatus action_wander(void* target) {
    AIAgent* agent = (AIAgent*)target;
    agent->timer += 0.016f;
    return BT_RUNNING;
}

static void Neon_OnStart(SLF_App* app) {
    shadow_init(&app->shadow, 2048, 4);
    terrain_init(&g_terrain, 100, 100, 1.0f);
    gizmo_init(&g_gr);

    g_bt_root = bt_create_sequence();
    bt_add_child(g_bt_root, bt_create_action(action_wander));

    g_shader = shader_load_program("assets/shaders/lighting.vert", "assets/shaders/lighting.frag");
    
    glGenTextures(1, &g_white_tex);
    glBindTexture(GL_TEXTURE_2D, g_white_tex);
    unsigned char white_data[] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white_data);

    camera_init(&app->camera, (vec3){100, 120, 200}, (vec3){0, 1, 0}, -135.0f, -30.0f);
}

static void Neon_OnUpdate(SLF_App* app) {
    bt_tick(g_bt_root, &g_neon_bot);
}

static void Neon_OnRender(SLF_App* app) {
    vec3 lightDir = {0.5f, -1.0f, 0.4f};
    glm_normalize(lightDir);

    mat4 view, proj;
    camera_get_view_matrix(&app->camera, view);
    glm_perspective(glm_rad(50.0f), (float)app->width/(float)app->height, 0.1f, 1500.0f, proj);

    // 1. Shadow Pass
    shadow_calculate_cascades(&app->shadow, view, proj, lightDir, 0.1f, 1500.0f);
    for (int i = 0; i < app->shadow.num_cascades; i++) {
        shadow_begin_pass(&app->shadow, i);
        terrain_draw(&g_terrain, app->shadow.shader_program); 
    }
    shadow_end_pass(app->width, app->height);

    // 2. Main Pass
    glUseProgram(g_shader);
    glUniformMatrix4fv(glGetUniformLocation(g_shader, "view"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(g_shader, "projection"), 1, GL_FALSE, (float*)proj);
    glUniform3fv(glGetUniformLocation(g_shader, "viewPos"), 1, (float*)app->camera.position);
    glUniform3fv(glGetUniformLocation(g_shader, "lightDir"), 1, lightDir);
    glUniformMatrix4fv(glGetUniformLocation(g_shader, "lightSpaceMatrix"), 1, GL_FALSE, (float*)app->shadow.light_space_matrices[0]);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_white_tex);
    glUniform1i(glGetUniformLocation(g_shader, "texture1"), 0);
    shadow_bind_texture(&app->shadow, 1);
    glUniform1i(glGetUniformLocation(g_shader, "shadowMap"), 1);
    
    mat4 model_terrain = GLM_MAT4_IDENTITY_INIT;
    glUniformMatrix4fv(glGetUniformLocation(g_shader, "model"), 1, GL_FALSE, (float*)model_terrain);
    terrain_draw(&g_terrain, g_shader);

    vec3 gizmo_color = {1.0f, 0.0f, 1.0f}; 
    vec3 gizmo_size = {5, 5, 5};
    gizmo_draw_cube(&g_gr, g_neon_bot.position, gizmo_size, gizmo_color, view, proj);
}

static void Neon_OnShutdown(SLF_App* app) {
    gizmo_destroy(&g_gr);
    if (g_shader) glDeleteProgram(g_shader);
    if (g_white_tex) glDeleteTextures(1, &g_white_tex);
}

int main(int argc, char* argv[]) {
    SLF_Config config = {
        .on_start = Neon_OnStart,
        .on_update = Neon_OnUpdate,
        .on_render = Neon_OnRender,
        .on_shutdown = Neon_OnShutdown
    };
    return starlight_framework_init("Starlight Mark-C: Project Neon", 1280, 720, &config);
}
