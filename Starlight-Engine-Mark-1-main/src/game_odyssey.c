#include <math.h>
#include "starlight.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- Game Constants ---
#define MAX_BEACONS 3
#define MAX_CRYSTALS 10
#define ENTROPY_DECAY 3.0f

typedef struct {
    float entropy;
    int beacons_active;
    bool screenshot_taken;
    vec3 beacon_positions[MAX_BEACONS];
    bool beacon_status[MAX_BEACONS];
    vec3 crystal_positions[MAX_CRYSTALS];
    bool crystal_active[MAX_CRYSTALS];
} GameData;

static GameData g_game;

// --- Callbacks ---

void Odyssey_OnStart(SLF_App* app) {
    printf(">>> STARLIGHT ODYSSEY: THE SIGNAL (C-NATIVE) <<<\n");
    
    g_game.entropy = 100.0f;
    g_game.beacons_active = 0;
    g_game.screenshot_taken = false;

    // Mood Setup
    app->post_processing_enabled = true;
    // Set sun, fog etc via engine hooks if exposed, 
    // for now we use the SLF defaults or explicit calls
    
    // Spawn Beacons
    g_game.beacon_positions[0][0] = 30.0f; g_game.beacon_positions[0][1] = 10.0f; g_game.beacon_positions[0][2] = 30.0f;
    g_game.beacon_positions[1][0] = -50.0f; g_game.beacon_positions[1][1] = 15.0f; g_game.beacon_positions[1][2] = -50.0f;
    g_game.beacon_positions[2][0] = 60.0f; g_game.beacon_positions[2][1] = 20.0f; g_game.beacon_positions[2][2] = -60.0f;

    // Build Action Mapping
    starlight_bind_action(app, "interact", SDL_SCANCODE_E, -1);
    starlight_bind_action(app, "jump", SDL_SCANCODE_SPACE, SDL_CONTROLLER_BUTTON_A);
}

void Odyssey_OnUpdate(SLF_App* app) {
    float dt = app->delta_time;

    // 1. Entropy Decay
    g_game.entropy -= ENTROPY_DECAY * dt;
    if (g_game.entropy <= 0) {
        g_game.entropy = 0;
        printf(">>> CRITICAL FAILURE: ENTROPY DEPLETED <<<\n");
        // app->running = false; // Game Over
    }

    // 2. Proximity Checks & Interactions
    vec3 player_pos;
    glm_vec3_copy(app->camera.position, player_pos);

    for (int i = 0; i < MAX_BEACONS; i++) {
        if (g_game.beacon_status[i]) continue;

        float dist = glm_vec3_distance(player_pos, g_game.beacon_positions[i]);
        if (dist < 5.0f && starlight_is_action_pressed(app, "interact")) {
            g_game.beacon_status[i] = true;
            g_game.beacons_active++;
            printf("Beacon %d Activated! (%d/%d)\n", i+1, g_game.beacons_active, MAX_BEACONS);
        }
    }

    // 3. UI/HUD logic (Console for now)
    static float timer = 0;
    timer += dt;
    if (timer > 1.0f) {
        printf("Entropy: %.1f%% | Signal: %d/%d\n", g_game.entropy, g_game.beacons_active, MAX_BEACONS);
        timer = 0;
    }
}

void Odyssey_OnUI(SLF_App* app) {
    // Draw Bar
    vec4 bar_bg = {0.2f, 0.2f, 0.2f, 0.8f};
    vec4 bar_fg = {0.0f, 1.0f, 0.5f, 1.0f};
    if (g_game.entropy < 20.0f) bar_fg[0] = 1.0f; // Red warning

    starlight_ui_draw_rect(app, 20, 20, 200, 20, bar_bg);
    starlight_ui_draw_rect(app, 20, 20, g_game.entropy * 2.0f, 20, bar_fg);
}

int main(int argc, char* argv[]) {
    SLF_Config config = {0};
    config.on_start = Odyssey_OnStart;
    config.on_update = Odyssey_OnUpdate;
    config.on_ui = Odyssey_OnUI;

    return starlight_framework_init("Starlight Odyssey: The Signal", 1280, 720, &config);
}
