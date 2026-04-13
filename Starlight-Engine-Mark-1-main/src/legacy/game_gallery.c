/**
 * STARLIGHT GALLERY
 * =================
 * Displays progress and unlocked +18 rewards.
 */
#include "starlight.h"
#include "save_system.h"
#include <stdio.h>

static void Gallery_OnStart(SLF_App* app) {
    save_system_init();
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

static void Gallery_OnUI(SLF_App* app) {
    glDisable(GL_DEPTH_TEST); glEnable(GL_BLEND);
    float cw = (float)app->width, ch = (float)app->height;
    
    // Background
    starlight_ui_draw_rect(app, 0, 0, cw, ch, (vec4){0.02, 0.02, 0.05, 1});
    
    static SLF_Font* font_big = NULL;
    if(!font_big) font_big = starlight_load_font("assets/fonts/Inconsolata-Regular.ttf", 48.0f);
    static SLF_Font* font = NULL;
    if(!font) font = starlight_load_font("assets/fonts/Inconsolata-Regular.ttf", 24.0f);

    starlight_ui_draw_text(app, font_big, "STARLIGHT GALLERY", cw/2-180, 40, (vec4){1, 0.8, 0, 1});

    // Display Slots (Grid 4 rows x 4 columns)
    float start_x = 100, start_y = 80;
    float card_w = 200, card_h = 130, gap_x = 40, gap_y = 30;

    int total_unlocked = 0;
    const char* game_names[] = {"PONG DELUXE", "STARLIGHT INVADERS", "NEON BREAKOUT", "VOID RUNNER"};

    for (int g = 0; g < 4; g++) {
        float gy = start_y + g * (card_h + gap_y);
        starlight_ui_draw_text(app, font, game_names[g], start_x, gy - 20, (vec4){0, 1, 1, 0.7});
        
        for (int i = 0; i < 4; i++) {
            float x = start_x + i * (card_w + gap_x);
            float y = gy;
            
            bool unlocked = save_system_is_unlocked(g, i); 
            if (unlocked) total_unlocked++;

            starlight_ui_draw_rect(app, x, y, card_w, card_h, (unlocked ? (vec4){0.1, 0.1, 0.3, 1} : (vec4){0.05, 0.05, 0.05, 1}));
            starlight_ui_draw_rect(app, x, y, card_w, 2, (vec4){1, 1, 1, 0.2});

            char b[16]; snprintf(b, sizeof(b), "SLOT %d", g * 4 + i + 1);
            starlight_ui_draw_text(app, font, unlocked ? "UNLOCKED!" : "LOCKED", x+40, y+card_h/2-10, (unlocked ? (vec4){0,1,0,1} : (vec4){1,1,1,0.3}));
            starlight_ui_draw_text(app, font, b, x+40, y+card_h-20, (vec4){1,1,1,0.15});
        }
    }

    // Master Piece
    if (total_unlocked >= 16) {
        starlight_ui_draw_rect(app, cw/2-150, ch-130, 300, 100, (vec4){1, 0, 1, 1});
        starlight_ui_draw_text(app, font, "MASTER PIECE UNLOCKED!", cw/2-130, ch-80, (vec4){1,1,1,1});
    } else {
        char b[64]; snprintf(b, sizeof(b), "Progress: %d / 16 slots unlocked", total_unlocked);
        starlight_ui_draw_text(app, font, b, cw/2-150, ch-30, (vec4){1,1,1,0.3});
    }
}

int main(int argc, char* argv[]) {
    SLF_Config config = { .on_start=Gallery_OnStart, .on_ui=Gallery_OnUI };
    return starlight_framework_init("Starlight Gallery", 1280, 720, &config);
}
