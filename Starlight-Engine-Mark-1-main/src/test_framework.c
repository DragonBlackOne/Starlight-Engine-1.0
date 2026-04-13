#include "starlight.h"
#include <stdio.h>
#include <stdio.h>

void do_heavy_work(void* data) {
    int* val = (int*)data;
    for (int i = 0; i < 100000; i++) {
        *val += (i % 2 == 0) ? 1 : 0;
    }
}

void on_start(SLF_App* app) {
    printf("*****************************************\n");
    printf("*   STARLIGHT GAME FRAMEWORK v1.1       *\n");
    printf("*****************************************\n");
    
    vec3 pos = {0, 10, 30};
    vec3 up = {0, 1, 0};
    camera_init(&app->camera, pos, up, -90.0f, -20.0f);
    
    app->post_processing_enabled = true;
    
    starlight_bind_action(app, "MoveForward",  SDL_SCANCODE_W, -1);
    starlight_bind_action(app, "MoveBackward", SDL_SCANCODE_S, -1);
    starlight_bind_action(app, "MoveLeft",     SDL_SCANCODE_A, -1);
    starlight_bind_action(app, "MoveRight",    SDL_SCANCODE_D, -1);
    
    printf("[GAME] Action Mapping & HUD READY.\n");

    printf("\n--- JOB SYSTEM STRESS TEST ---\n");
    printf("[TEST] Dispatching 2000 heavy mathematical jobs across all %d CPU worker threads...\n", SDL_GetCPUCount() - 1);
    
    uint32_t start_ticks = SDL_GetTicks();
    SDL_atomic_t counter;
    SDL_AtomicSet(&counter, 0);
    static int test_data[2000];
    
    for (int i = 0; i < 2000; i++) {
        test_data[i] = 0;
        job_system_push(do_heavy_work, &test_data[i], &counter);
    }
    
    job_system_wait(&counter);
    uint32_t ms = SDL_GetTicks() - start_ticks;
    
    printf("[TEST] SUCCESS! 2000 Jobs finished in %u ms.\n", ms);
    printf("[TEST] Sample Validation: data[0] = %d, data[1999] = %d\n", test_data[0], test_data[1999]);
    printf("------------------------------\n\n");
}

void on_update(SLF_App* app) {
    float speed = 10.0f * app->delta_time;
    
    if (starlight_is_action_pressed(app, "MoveForward"))  app->camera.position[2] -= speed;
    if (starlight_is_action_pressed(app, "MoveBackward")) app->camera.position[2] += speed;
    if (starlight_is_action_pressed(app, "MoveLeft"))     app->camera.position[0] -= speed;
    if (starlight_is_action_pressed(app, "MoveRight"))    app->camera.position[0] += speed;

    if (app->keys[SDL_SCANCODE_ESCAPE]) app->running = false;
}

void on_render(SLF_App* app) {
    // 3D Rendering test
}

void on_ui(SLF_App* app) {
    vec4 bg_color = {0.2f, 0.2f, 0.2f, 0.8f};
    vec4 hp_color = {0.0f, 1.0f, 0.2f, 1.0f};
    vec4 cross_color = {1.0f, 1.0f, 1.0f, 0.7f};

    // HUD: HP Bar
    starlight_ui_draw_rect(app, 30.0f, 30.0f, 250.0f, 25.0f, bg_color);
    starlight_ui_draw_rect(app, 30.0f, 30.0f, 180.0f, 25.0f, hp_color); 

    // HUD: Crosshair
    float cx = (float)app->width / 2.0f;
    float cy = (float)app->height / 2.0f;
    starlight_ui_draw_rect(app, cx - 2, cy - 2, 4.0f, 4.0f, cross_color);
}

int main(int argc, char* argv[]) {
    SLF_Config config;
    config.on_start  = on_start;
    config.on_update = on_update;
    config.on_render = on_render;
    config.on_ui     = on_ui;
    
    return starlight_framework_init("Starlight SLF v1.1 - Stable HUD", 1280, 720, &config);
}
