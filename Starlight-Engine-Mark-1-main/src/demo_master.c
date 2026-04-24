// Este projeto é feito por IA e só o prompt é feito por um humano.
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <cglm/cglm.h>
#include <flecs.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "starlight.h"
#include "pbr.h"
#include "components.h"
#include "physics.h"

// --- TYPES & GLOBALS ---
typedef struct {
    vec3 position;
    vec3 velocity;
    float timer;
    AudioSource* sfx;
} MasterAgent;

static SLF_Scene g_scene_menu;
static SLF_Scene g_scene_world;

// Assets
static GLuint g_pbr_shader = 0;
static PBR_IBL g_ibl = {0};
static Terrain g_terrain;
static GizmoRenderer g_gizmo;
static SLF_Font* g_font_main = NULL;
static MasterAgent g_master_agent = {0};

// --- BEHAVIOR TREE ---
BTStatus action_bot_move(void* target) {
    MasterAgent* agent = (MasterAgent*)target;
    agent->timer += 0.016f;
    agent->position[0] = sinf(agent->timer) * 15.0f;
    agent->position[1] = 15.0f;
    agent->position[2] = cosf(agent->timer) * 15.0f;
    
    vec3 velocity = { cosf(agent->timer) * 15.0f, 0.0f, -sinf(agent->timer) * 15.0f };
    glm_vec3_copy(velocity, agent->velocity);

    if (agent->sfx) {
        audio_source_set_position(agent->sfx, agent->position);
        audio_source_set_velocity(agent->sfx, agent->velocity);
    }
    return BT_RUNNING;
}

// --- MENU SCENE ---
static void Menu_OnEnter(SLF_App* app) {
    (void)app;
    printf("[MASTER] Entered Main Menu\n");
}

static void Menu_OnUI(SLF_App* app) {
    vec4 col_white = {1.0f, 1.0f, 1.0f, 1.0f};
    vec4 col_dim = {0.7f, 0.7f, 0.9f, 1.0f};
    vec4 col_bg = {0.05f, 0.05f, 0.15f, 1.0f};
    
    starlight_ui_draw_rect(app, 0, 0, (float)app->width, (float)app->height, col_bg);
    
    if (g_font_main) {
        starlight_ui_draw_text(app, g_font_main, "STARLIGHT ENGINE", (float)app->width/2.0f - 150.0f, 200.0f, col_white);
        starlight_ui_draw_text(app, g_font_main, "ULTIMATE MASTER SHOWCASE", (float)app->width/2.0f - 180.0f, 250.0f, col_dim);
        starlight_ui_draw_text(app, g_font_main, "[ PRESS SPACE TO START ]", (float)app->width/2.0f - 140.0f, 450.0f, col_white);
    } else {
        // Fallback if font missing
        starlight_ui_draw_rect(app, 100, 100, 200, 50, col_white);
    }
    
    if (starlight_is_action_just_pressed(app, "confirm")) {
        starlight_scene_change(app, &g_scene_world);
    }
}

// --- WORLD SCENE ---
static void World_OnEnter(SLF_App* app) {
    vec3 cam_pos = {0.0f, 30.0f, 60.0f};
    vec3 cam_up = {0.0f, 1.0f, 0.0f};
    printf("[MASTER] Entering World Showcase. Res: %dx%d\n", app->width, app->height);
    camera_init(&app->camera, cam_pos, cam_up, -90.0f, -20.0f);

    g_master_agent.sfx = audio_create_source("assets/audio/announcer_go.wav");
    audio_source_play(g_master_agent.sfx, true, 1.0f);

    // Spawn 1000 entities para demonstrar o Next-Gen Frustum Culling e Audio Occlusion
    for(int i = 0; i < 1000; i++) {
        ecs_entity_t e = ecs_new_id(app->world);
        float x = (float)(rand() % 800) - 400.0f;
        float z = (float)(rand() % 800) - 400.0f;
        ecs_set(app->world, e, Transform, {{x, 15.0f, z}, {0,0,0}, {1,1,1}});
        ecs_set(app->world, e, AABB, {{-2,-2,-2}, {2,2,2}});
    }
}

static void World_OnRender(SLF_App* app) {
    // 0. Explicit Clear for World
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    vec3 light_dir = {0.5f, -1.0f, -0.3f};
    glm_vec3_normalize(light_dir);

    // Camera processing
    vec3 cam_movement = {0, 0, 0};
    audio_set_listener(app->camera.position, app->camera.front, app->camera.up, cam_movement);
    action_bot_move(&g_master_agent);

    mat4 view, proj;
    camera_get_view_matrix(&app->camera, view);
    glm_perspective(glm_rad(60.0f), (float)app->width/(float)app->height, 0.1f, 1000.0f, proj);

    // 1. Shadow Pass
    shadow_calculate_cascades(&app->shadow, view, proj, light_dir, 0.1f, 1000.0f);
    for (int i = 0; i < app->shadow.num_cascades; i++) {
        shadow_begin_pass(&app->shadow, i);
        terrain_draw(&g_terrain, app->shadow.shader_program);
    }
    shadow_end_pass(app->width, app->height);

    // 2. Main Render Pass
    if (g_pbr_shader) {
        glUseProgram(g_pbr_shader);
        glUniformMatrix4fv(glGetUniformLocation(g_pbr_shader, "u_view"), 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(glGetUniformLocation(g_pbr_shader, "u_projection"), 1, GL_FALSE, (float*)proj);
        glUniform3fv(glGetUniformLocation(g_pbr_shader, "u_view_pos"), 1, (float*)app->camera.position);
        glUniform3fv(glGetUniformLocation(g_pbr_shader, "u_light_dir"), 1, light_dir);
        glUniform3f(glGetUniformLocation(g_pbr_shader, "u_light_color"), 1.5f, 1.4f, 1.2f); // Bolder light
        
        shadow_bind_texture(&app->shadow, 8);
        glUniform1i(glGetUniformLocation(g_pbr_shader, "u_shadow_map"), 8);
        glUniformMatrix4fv(glGetUniformLocation(g_pbr_shader, "u_light_matrices"), 4, GL_FALSE, (float*)app->shadow.light_space_matrices);
        glUniform1fv(glGetUniformLocation(g_pbr_shader, "u_cascade_splits"), 4, app->shadow.cascade_splits);

        // Bind IBL Maps
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_CUBE_MAP, g_ibl.irradiance_map);
        glUniform1i(glGetUniformLocation(g_pbr_shader, "u_irradiance_map"), 0);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_CUBE_MAP, g_ibl.prefilter_map);
        glUniform1i(glGetUniformLocation(g_pbr_shader, "u_prefilter_map"), 1);
        glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, g_ibl.brdf_lut);
        glUniform1i(glGetUniformLocation(g_pbr_shader, "u_brdf_lut"), 2);
        
        glUniform1i(glGetUniformLocation(g_pbr_shader, "u_use_ibl"), 1);
        glUniform1i(glGetUniformLocation(g_pbr_shader, "u_is_textured"), 0);
        glUniform1i(glGetUniformLocation(g_pbr_shader, "u_planet_count"), 0);

        // Draw Terrain (as a big plane)
        mat4 model_t = GLM_MAT4_IDENTITY_INIT;
        glUniformMatrix4fv(glGetUniformLocation(g_pbr_shader, "u_model"), 1, GL_FALSE, (float*)model_t);
        glUniform4f(glGetUniformLocation(g_pbr_shader, "u_base_color"), 0.3f, 0.5f, 0.3f, 1.0f);
        glUniform1f(glGetUniformLocation(g_pbr_shader, "u_metallic"), 0.0f);
        glUniform1f(glGetUniformLocation(g_pbr_shader, "u_roughness"), 0.8f);
        terrain_draw(&g_terrain, g_pbr_shader);
        
        // Frustum Culling / Audio Occlusion Extraction
        mat4 view_proj; glm_mat4_mul(proj, view, view_proj);
        camera_extract_frustum(&app->camera, view_proj);

        vec3 ray_dir;
        glm_vec3_sub(g_master_agent.position, app->camera.position, ray_dir);
        float dist_to_agent = glm_vec3_norm(ray_dir);
        glm_vec3_normalize(ray_dir);
        bool agent_occluded = false;

        ecs_query_t* q = ecs_query(app->world, {
            .filter.terms = {
                { .id = ecs_id(Transform) },
                { .id = ecs_id(AABB), .oper = EcsOptional }
            }
        });

        int rendered_count = 0;
        ecs_iter_t it = ecs_query_iter(app->world, q);
        while (ecs_query_next(&it)) {
            Transform* t = ecs_field(&it, Transform, 1);
            AABB* aabb = ecs_field(&it, AABB, 2);

            for (int i = 0; i < it.count; ++i) {
                if (aabb) {
                    AABB world_aabb;
                    glm_vec3_add(aabb[i].min, t[i].position, world_aabb.min);
                    glm_vec3_add(aabb[i].max, t[i].position, world_aabb.max);
                    
                    // Audio Occlusion Raycast
                    float hit_dist = 0.0f;
                    if (physics_ray_vs_aabb(app->camera.position, ray_dir, world_aabb.min, world_aabb.max, &hit_dist)) {
                        if (hit_dist > 0.01f && hit_dist < dist_to_agent) {
                            agent_occluded = true;
                        }
                    }

                    // OpenGL Frustum Culling
                    if (!camera_check_aabb(&app->camera.frustum, &world_aabb)) continue; 
                }

                mat4 model_s = GLM_MAT4_IDENTITY_INIT;
                glm_translate(model_s, t[i].position);
                glUniformMatrix4fv(glGetUniformLocation(g_pbr_shader, "u_model"), 1, GL_FALSE, (float*)model_s);
                glUniform4f(glGetUniformLocation(g_pbr_shader, "u_base_color"), 1.0f, 0.2f, 0.2f, 1.0f); 
                glUniform1f(glGetUniformLocation(g_pbr_shader, "u_metallic"), 0.8f);
                glUniform1f(glGetUniformLocation(g_pbr_shader, "u_roughness"), 0.2f);
                terrain_draw(&g_terrain, g_pbr_shader); 
                rendered_count++;
            }
        }
        ecs_query_fini(q);
        
        audio_source_set_occlusion(g_master_agent.sfx, agent_occluded ? 1.0f : 0.0f);
        
        // Draw the Audio Source Agent
        mat4 model_a = GLM_MAT4_IDENTITY_INIT;
        glm_translate(model_a, g_master_agent.position);
        glm_scale(model_a, (vec3){2,2,2}); // Make it visibly bigger
        glUniformMatrix4fv(glGetUniformLocation(g_pbr_shader, "u_model"), 1, GL_FALSE, (float*)model_a);
        glUniform4f(glGetUniformLocation(g_pbr_shader, "u_base_color"), 0.0f, 1.0f, 1.0f, 1.0f); // Cyan
        terrain_draw(&g_terrain, g_pbr_shader); 
    }
}

static void World_OnUI(SLF_App* app) {
    char buf[128];
    vec4 col_fps = {0, 1, 0, 1};
    vec4 col_label = {1, 1, 0, 1};
    snprintf(buf, 128, "FPS: %.1f", (app->delta_time > 0) ? 1.0f / app->delta_time : 0.0f);
    starlight_ui_draw_text(app, g_font_main, buf, 20.0f, 20.0f, col_fps);
    starlight_ui_draw_text(app, g_font_main, "ULTIMATE SHOWCASE - WORLD ACTIVE", (float)app->width/2.0f - 200.0f, 30.0f, col_label);
    starlight_ui_draw_text(app, g_font_main, "[ BACKSPACE ] RETURN TO MENU", 20.0f, (float)app->height - 40.0f, col_fps);
    
    if (app->keys[SDL_SCANCODE_BACKSPACE]) {
        starlight_scene_change(app, &g_scene_menu);
    }
}

static void World_OnUpdate(SLF_App* app, float dt) {
    (void)dt;
    // Em modo servidor-dedicado (headless), processamos a logica do bot no Update Tick
    // (Na UI normal, estamos processando isso no OnRender por simplificacao da demo, 
    // idealmente sempre devera ser no OnUpdate)
    if (app->headless) {
        action_bot_move(&g_master_agent);
    }
}

static void Master_OnStart(SLF_App* app) {
    printf("[MASTER] Starting Initialization (Headless? %d)...\n", app->headless);
    
    memset(&g_scene_world, 0, sizeof(SLF_Scene));
    g_scene_world.on_enter = World_OnEnter;
    g_scene_world.on_update = World_OnUpdate;
    if (!app->headless) {
        g_scene_world.on_draw = World_OnRender;
        g_scene_world.on_ui = World_OnUI;
    }
    
    if (!app->headless) {
        app->post_processing_enabled = true;
        g_pbr_shader = shader_load_program("assets/shaders/pbr.vert", "assets/shaders/pbr.frag");
        
        terrain_init(&g_terrain, 128, 128, 2.0f);
        pbr_init_ibl("assets/textures/environment.hdr", &g_ibl);
        g_font_main = starlight_load_font("assets/fonts/Inconsolata-Regular.ttf", 24.0f);
        
        memset(&g_scene_menu, 0, sizeof(SLF_Scene));
        g_scene_menu.on_enter = Menu_OnEnter;
        g_scene_menu.on_ui = Menu_OnUI;
        
        starlight_bind_action_key(app, "confirm", SDL_SCANCODE_SPACE);
        starlight_scene_push(app, &g_scene_menu);
    } else {
        printf("[SERVER] Dedicated Server Bootstrapping...\n");
        // Push diretos para World
        starlight_scene_push(app, &g_scene_world);
    }
    
    printf("[MASTER] Ready.\n");
    fflush(stdout);
}

static void Master_OnShutdown(SLF_App* app) {
    (void)app;
    terrain_destroy(&g_terrain);
    starlight_unload_font(g_font_main);
    pbr_destroy_ibl(&g_ibl);
    if (g_pbr_shader) glDeleteProgram(g_pbr_shader);
    if (g_master_agent.sfx) audio_source_destroy(g_master_agent.sfx);
}

int main(int argc, char* argv[]) {
    SLF_Config config;
    memset(&config, 0, sizeof(SLF_Config));
    
    // Parse CLI Arguments for Headless Dedicated Server Mode
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--headless") == 0 || strcmp(argv[i], "-h") == 0) {
            config.is_headless = true;
        }
    }
    
    config.on_start = Master_OnStart;
    config.on_shutdown = Master_OnShutdown;
    
    return starlight_framework_init("Starlight Engine Ultimate Master Showcase", 1280, 720, &config);
}
