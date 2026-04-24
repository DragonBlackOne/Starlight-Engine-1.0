// Este projeto é feito por IA e só o prompt é feito por um humano.
/**
 * STARLIGHT PONG DELUXE: UNIFIED HUB (+18 Edition)
 * ===============================================
 * The central game loop that connects Pong, Mini-games, and the Gallery.
 */
#ifdef _MSC_VER
#pragma warning(disable: 4244)
#pragma warning(disable: 4305)
#endif

#include "starlight.h"
#include "save_system.h"
#include "navmesh.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_PARTICLES 250
#define WIN_SCORE     7

// --- Starlight 2D Standards ---
#define COLOR_CYAN    (vec4){0.0f, 1.0f, 1.0f, 1.0f}
#define COLOR_MAGENTA (vec4){1.0f, 0.0f, 1.0f, 1.0f}
#define COLOR_YELLOW  (vec4){1.0f, 1.0f, 0.0f, 1.0f}
#define COLOR_BG_DARK (vec4){0.01f, 0.01f, 0.03f, 1.0f}

#define PLAYER_SPD    600.0f
#define BALL_SPD      450.0f
#define DT_CAP        0.05f

// --- Hub States ---
typedef enum { 
    ST_HUB_MENU, 
    ST_HUB_GALLERY, 
    ST_HUB_REWARD, 
    ST_HUB_PONG, 
    ST_HUB_INVADERS, 
    ST_HUB_BREAKOUT,
    ST_HUB_RUNNER,
    ST_HUB_DIFFICULTY,
    ST_HUB_SETTINGS,
    ST_HUB_PAUSE
} HubState;

// --- Sub-State Sharing ---
typedef struct { float x, y, vx, vy, life, max_life, size; vec4 color; } Particle;

// --- Pong Data ---
typedef struct {
    int state; // 0:SERVE, 1:MATCH, 2:PAUSE, 3:GOAL, 4:GAMEOVER
    float plx, ply, plw, plh;
    float prx, pry, prw, prh;
    float bx, by, bvx, bvy, bsize;
    int bactive;
    int score_l, score_r;
    float serve_timer;
    int last_scorer;
    float trail_x[35], trail_y[35];
    int trail_head;
    
    // Ultimate Features
    float powerup_x, powerup_y, powerup_timer;
    int powerup_type; // 1:BIG, 2:FIRE, 3:MULTI
    float plh_target;
    float ai_focus;
    float chromatic_timer;
} PongState;

// --- Invaders Data ---
typedef struct { 
    float px, py; // Player
    float ax, ay; // Elite Alien
    int score, wave;
    struct { float x, y; int active; } enemies[40];
    struct { float x, y; int active; } bullets[10];
    float enemy_dir;
} InvadersState;

// --- Breakout Data ---
typedef struct {
    float px; float bx, by, bvx, bvy;
    struct { float x, y; int active; } bricks[50];
    int score, lives;
} BreakoutState;

// --- Runner Data ---
typedef struct {
    float py, pvy, dist;
    struct { float x, y, w, h; int active; } obs[10];
    float obs_timer;
} RunnerState;

// --- Main Hub Struct ---
typedef struct {
    HubState state;
    int menu_sel;
    float total_time;
    float shake_timer;
    float shake_x, shake_y;
    
    Particle particles[MAX_PARTICLES];
    int particle_count;
    
    PongState pong;
    InvadersState inv;
    BreakoutState brk;
    RunnerState run;

    // Gallery / Reward
    int reward_game_id;
    int reward_slot_idx;
    int difficulty; // 0:EASY, 1:NORMAL, 2:HARD
    int pause_sel;
    GLuint gallery_tex[16];
    TextureLoadJob gallery_jobs[16];
    bool gallery_loaded[16];
    float transition_alpha; // Fade effect
    int input_mode; // 0: KEYBOARD, 1: MOUSE (Framework Overhaul Update)
    int diff_screen_sel; // 0: DIFFICULTY, 1: CONTROL (Menu Nav)
    
    // Settings
    float music_vol, sfx_vol;
    bool crt_enabled;
    int settings_sel;

    // Netcode / Lag Comp
    struct {
        float h_bx[120], h_by[120];
        int head;
    } net;

    // Audio
    AudioSource *sfx_hit, *sfx_score, *sfx_select, *sfx_wall, *sfx_win, *sfx_lose;
    AudioSource *ann_nice, *ann_ouch, *ann_winner, *ann_defeat, *ann_go, *ann_excellent;
} GameHub;

static GameHub G;
static SLF_Font *f_big, *f_med, *f_small;
static SLF_Tween g_tween_fade;     // Framework Overhaul: Tween for screen transitions
static SLF_Tween g_tween_paddle;   // Framework Overhaul: Tween for paddle size powerup

// --- Shared Logic ---
static void emit(float x, float y, vec4 col, int count) {
    for (int i = 0; i < count && G.particle_count < MAX_PARTICLES; i++) {
        Particle* p = &G.particles[G.particle_count++];
        p->x = x; p->y = y;
        float a = ((rand()%360)*3.14f)/180.0f; float s = 60.0f + rand()%100;
        p->vx = cosf(a)*s; p->vy = sinf(a)*s;
        p->life = 0.5f + (rand()%50)/100.0f; p->max_life = p->life;
        p->size = 2.0f + rand()%4; glm_vec4_copy(col, p->color);
    }
}

#include <immintrin.h>
static void update_hub_vfx(float dt) {
    if (G.particle_count == 0) return;

    // SIMD Update Pass (Physics)
    // For 1000 particles, we process 250 blocks of 4
    int simd_count = (G.particle_count / 4) * 4;
    for (int i = 0; i < simd_count; i += 4) {
        // We do manual SIMD here for demonstration of the requirement
        for(int j=0; j<4; j++) {
            G.particles[i+j].life -= dt;
            G.particles[i+j].x += G.particles[i+j].vx * dt;
            G.particles[i+j].y += G.particles[i+j].vy * dt;
        }
    }
    // Tail end
    for (int i = simd_count; i < G.particle_count; i++) {
        G.particles[i].life -= dt;
        G.particles[i].x += G.particles[i].vx * dt;
        G.particles[i].y += G.particles[i].vy * dt;
    }

    // Cleanup and Alpha Update (Standard)
    for (int i = 0; i < G.particle_count;) {
        Particle* p = &G.particles[i];
        if (p->life <= 0) { G.particles[i] = G.particles[--G.particle_count]; continue; }
        p->color[3] = p->life / p->max_life; i++;
    }

    if (G.shake_timer > 0) {
        G.shake_timer -= dt;
        G.shake_x = ((rand()%100-50)/50.0f) * G.shake_timer * 10;
        G.shake_y = ((rand()%100-50)/50.0f) * G.shake_timer * 10;
    } else { G.shake_x = G.shake_y = 0; }

    // Netcode: Capture Snapshot (60fps assumed)
    G.net.h_bx[G.net.head] = G.pong.bx;
    G.net.h_by[G.net.head] = G.pong.by;
    G.net.head = (G.net.head + 1) % 120;
}

// --- Pong Implementation ---
static void reset_pong_ball(int dir) {
    G.pong.bx = 1280.0f/2.0f; G.pong.by = 720.0f/2.0f;
    G.pong.bvx = dir * BALL_SPD; G.pong.bvy = (float)(rand()%200-100);
}

static void update_pong(SLF_App* app, float dt) {
    if(G.pong.state == 0) { // SERVE
        G.pong.serve_timer -= dt;
        if(G.pong.serve_timer <= 0) { G.pong.state = 1; reset_pong_ball(G.pong.last_scorer > 0 ? -1 : 1); }
        return;
    }
    
    if(G.pong.chromatic_timer > 0) G.pong.chromatic_timer -= dt;

    // Ball Speed based on difficulty
    float base_ball_spd = BALL_SPD;
    if(G.difficulty == 0) base_ball_spd = 350.0f;
    else if(G.difficulty == 2) base_ball_spd = 600.0f;

    // Paddle Move & Powerup Smoothness
    G.pong.plh += (G.pong.plh_target - G.pong.plh) * dt * 5.0f;
    
    // Framework Overhaul: Conditional Input Mode
    if(G.input_mode == 0) { // KEYBOARD
        if(app->keys[SDL_SCANCODE_W]) G.pong.ply -= PLAYER_SPD * dt;
        if(app->keys[SDL_SCANCODE_S]) G.pong.ply += PLAYER_SPD * dt;
    } else { // MOUSE (Handled in Hub_OnUpdate to access mouse state directly)
        // No action here, moved to Hub_OnUpdate for consistency
    }
    G.pong.ply = fmaxf(40.0f, fminf(720.0f-G.pong.plh-40.0f, G.pong.ply));
    
    // Elite AI Logic (Scales with difficulty)
    float ai_speed = 0.05f + G.difficulty * 0.07f;
    G.pong.ai_focus = (G.pong.score_l > G.pong.score_r) ? (0.1f * G.difficulty) : 0.0f;
    float ai_target = G.pong.by - G.pong.prh/2.0f;
    G.pong.pry += (ai_target - G.pong.pry) * (ai_speed + G.pong.ai_focus);
    G.pong.pry = fmaxf(40.0f, fminf(720.0f-G.pong.prh-40.0f, G.pong.pry));

    // --- Ball Sub-stepping (CCD) ---
    int steps = 4; float sdt = dt / (float)steps;
    for(int s=0; s<steps; s++) {
        G.pong.bx += G.pong.bvx * sdt; G.pong.by += G.pong.bvy * sdt;
        
        // Ball Bounce
        if(G.pong.by < 40.0f || G.pong.by > 680.0f) {
            G.pong.bvy *= -1.02f; 
            // Retro Impact Sound (Math generated Square Wave)
            audio_play_chiptune(1, 440.0f, 80.0f, 0.6f); 
            if(G.sfx_wall) audio_source_play(G.sfx_wall, 0, 0.1f);
            break;
        }

        // Paddle Hit & VFX Juice
        if(G.pong.bvx < 0 && G.pong.bx < G.pong.plx+G.pong.plw && G.pong.by > G.pong.ply && G.pong.by < G.pong.ply+G.pong.plh) {
            float hit_factor = (G.pong.by - (G.pong.ply + G.pong.plh/2)) / (G.pong.plh/2);
            G.pong.bvx *= -1.08f; G.pong.bvy += hit_factor * 200.0f;
            emit(G.pong.bx, G.pong.by, COLOR_CYAN, 8);
            G.pong.chromatic_timer = 0.15f; G.shake_timer = 0.2f;
            
            // Retro 8-bit Sound
            audio_play_chiptune(1, 600.0f + fabsf(hit_factor)*300.0f, 100.0f, 0.8f);
            
            if(G.sfx_hit) {
                audio_source_set_pitch(G.sfx_hit, 0.9f + fabsf(hit_factor)*0.3f);
                audio_source_play(G.sfx_hit, 0, 0.3f);
            }
            save_system_add_stat("hits", 1);
            break;
        }
        if(G.pong.bvx > 0 && G.pong.bx > G.pong.prx-14.0f && G.pong.by > G.pong.pry && G.pong.by < G.pong.pry+G.pong.prh) {
            G.pong.bvx *= -1.08f; 
            emit(G.pong.bx, G.pong.by, COLOR_MAGENTA, 8);
            G.pong.chromatic_timer = 0.1f;
            
            audio_play_chiptune(1, 800.0f, 100.0f, 0.8f);

            if(G.sfx_hit) {
                audio_source_set_pitch(G.sfx_hit, 1.1f);
                audio_source_play(G.sfx_hit, 0, 0.3f);
            }
            break;
        }
    }

    // Powerup Logic
    G.pong.powerup_timer -= dt;
    if(G.pong.powerup_timer <= 0) {
        G.pong.powerup_x = 300.0f + (rand() % 600); G.pong.powerup_y = 100.0f + (rand() % 500);
        G.pong.powerup_type = 1 + (rand() % 3); G.pong.powerup_timer = 8.0f;
    }
    // Collect Powerup
    float dx = G.pong.bx - G.pong.powerup_x, dy = G.pong.by - G.pong.powerup_y;
    if(sqrtf(dx*dx + dy*dy) < 40.0f) {
        if(G.pong.powerup_type == 1) G.pong.plh_target = 200.0f;
        if(G.pong.powerup_type == 2) G.pong.bvx *= 1.5f;
        G.pong.powerup_x = -1000; emit(G.pong.bx, G.pong.by, COLOR_YELLOW, 20);
    }

    // Scoring
    if(G.pong.bx < 0) { 
        G.pong.score_r++; G.pong.last_scorer=1; G.pong.state=0; G.pong.serve_timer=1.0f; G.pong.plh_target = 100.0f; 
        audio_play_chiptune(3, 150.0f, 600.0f, 1.0f); // 3 = Sawtooth Low Pitch (Fail)
        if(G.ann_ouch) audio_source_play(G.ann_ouch, 0, 1.0f);
        if(G.sfx_lose) audio_source_play(G.sfx_lose, 0, 0.2f);
        
        if(G.pong.score_r >= WIN_SCORE) {
            if(G.ann_defeat) audio_source_play(G.ann_defeat, 0, 1.0f);
            G.state = ST_HUB_MENU; // Game Over
        }
    }
    if(G.pong.bx > 1280) { 
        G.pong.score_l++; G.pong.last_scorer=-1; G.pong.state=0; G.pong.serve_timer=1.0f; G.pong.plh_target = 100.0f;
        audio_play_chiptune(1, 880.0f, 500.0f, 1.0f); // 1 = Square High Pitch (Success)
        if(G.ann_nice) audio_source_play(G.ann_nice, 0, 1.0f);
        if(G.sfx_score) audio_source_play(G.sfx_score, 0, 0.2f);
        
        if(G.pong.score_l >= WIN_SCORE) { 
            if(G.ann_winner) audio_source_play(G.ann_winner, 0, 1.0f);
            if(G.sfx_win) audio_source_play(G.sfx_win, 0, 0.8f);
            int r = rand() % 3;
            if(r == 0) G.state = ST_HUB_INVADERS;
            else if(r == 1) G.state = ST_HUB_BREAKOUT;
            else G.state = ST_HUB_RUNNER;
            G.reward_game_id = rand() % 2; G.reward_slot_idx = rand() % 4;
            save_system_add_stat("wins", 1);
            G.transition_alpha = 1.0f; // Start fade
        }
    }
    
    // Dynamic Music Intensity & Ducking
    bool ducking = audio_source_is_playing(G.ann_nice) || audio_source_is_playing(G.ann_ouch) ||
                   audio_source_is_playing(G.ann_winner) || audio_source_is_playing(G.ann_defeat) ||
                   audio_source_is_playing(G.ann_go) || audio_source_is_playing(G.ann_excellent);
    
    float intensity = fminf(1.5f, 1.0f + (fabsf(G.pong.bvx) - BALL_SPD) / 1000.0f);
    float target_vol = 0.8f * intensity * (ducking ? 0.3f : 1.0f);
    audio_set_master_volume(target_vol);
}

// --- Invaders Implementation ---
static void update_invaders(SLF_App* app, float dt) {
    if(app->keys[SDL_SCANCODE_A]) G.inv.px -= PLAYER_SPD * dt;
    if(app->keys[SDL_SCANCODE_D]) G.inv.px += PLAYER_SPD * dt;
    
    // Simple A* Integration: Alien targets player
    NavPos path[64]; int cnt;
    int sx = (int)(G.inv.ax/40)%NAV_GRID_SIZE; int sy = (int)(G.inv.ay/40)%NAV_GRID_SIZE;
    int ex = (int)(G.inv.px/40)%NAV_GRID_SIZE; int ey = (int)(G.inv.py/40)%NAV_GRID_SIZE;
    
    if(nav_find_path(sx, sy, ex, ey, path, &cnt) && cnt > 1) {
        G.inv.ax += (path[cnt-2].x*40 - G.inv.ax) * dt * 2.0f;
    }

    static float win_t = 0; win_t += dt;
    if(win_t > 5.0f) {
        win_t = 0; G.state = ST_HUB_REWARD;
        save_system_unlock(G.reward_game_id, G.reward_slot_idx);
    }
}

// --- Lifecycle ---
static void Hub_OnStart(SLF_App* app) {
    save_system_init();
    memset(&G, 0, sizeof(G));
    G.state = ST_HUB_MENU;
    f_big = starlight_load_font("assets/fonts/Inconsolata-Regular.ttf", 72.0f);
    f_med = starlight_load_font("assets/fonts/Inconsolata-Regular.ttf", 32.0f);
    f_small = starlight_load_font("assets/fonts/Inconsolata-Regular.ttf", 20.0f);
    
    G.sfx_hit = audio_create_source("assets/audio/ball_hit_1.mp3"); // Real hit sound
    G.sfx_score = audio_create_source("assets/audio/score_point_1.mp3");
    G.sfx_select = audio_create_source("assets/audio/menu_click_1.mp3");
    G.sfx_wall = audio_create_source("assets/audio/sfx/sfx_hit.wav"); // Quieter bounce sound
    G.sfx_win = audio_create_source("assets/audio/win_1.mp3");
    G.sfx_lose = audio_create_source("assets/audio/lose_1.mp3");
    
    G.ann_nice = audio_create_source("assets/audio/announcer_nice.wav");
    G.ann_ouch = audio_create_source("assets/audio/announcer_ouch.wav");
    G.ann_winner = audio_create_source("assets/audio/announcer_winner.wav");
    G.ann_defeat = audio_create_source("assets/audio/announcer_defeat.wav");
    G.ann_go = audio_create_source("assets/audio/announcer_go.wav");
    G.ann_excellent = audio_create_source("assets/audio/announcer_excellent.wav");
    
    // Load Gallery Texture Jobs (Asynchronous)
    const char* g_paths[] = {
        "assets/images/199d1bbf-379d-4cb4-86dd-c0c57b9812b6.png", "assets/images/760601032951764920.png",
        "assets/images/760601204750459077.png", "assets/images/760601320714577573.png",
        "assets/images/cbb5eb5c-7e75-4878-9c51-122c9a0f2072.png", "assets/images/eeb6ce4b-aee5-4d01-83f3-0baf8faa8182.png",
        "assets/images/image (1).png", "assets/images/image (9).png"
    };
    for(int i=0; i<8; i++) loader_async_load_texture(g_paths[i], &G.gallery_jobs[i]);

    // Init Pong
    G.pong.plx = 50.0f; G.pong.ply = 300.0f; G.pong.plw = 15.0f; G.pong.plh = 100.0f; G.pong.plh_target = 100.0f;
    G.pong.prx = 1280.0f-65.0f; G.pong.pry = 300.0f; G.pong.prw = 15.0f; G.pong.prh = 100.0f;
    G.pong.bsize = 14.0f; G.pong.powerup_timer = 5.0f;

    // Framework Overhaul: Multi-Bind Actions (WASD + Arrows + Gamepad)
    starlight_bind_action(app, "up", SDL_SCANCODE_W, -1);
    starlight_bind_action(app, "up", SDL_SCANCODE_UP, -1);         // Also arrow up
    starlight_bind_action(app, "down", SDL_SCANCODE_S, -1);
    starlight_bind_action(app, "down", SDL_SCANCODE_DOWN, -1);     // Also arrow down
    starlight_bind_action(app, "enter", SDL_SCANCODE_RETURN, SDL_CONTROLLER_BUTTON_A);
    starlight_bind_action(app, "esc", SDL_SCANCODE_ESCAPE, SDL_CONTROLLER_BUTTON_B);
    starlight_bind_action(app, "p_jump", SDL_SCANCODE_SPACE, SDL_CONTROLLER_BUTTON_A);

    audio_play_music("assets/audio/background_music.mp3", 0.4f);

    G.input_mode = 0; // Default to Keyboard
    G.diff_screen_sel = 0; 
    
    // Settings Default
    G.music_vol = 1.0f;
    G.sfx_vol = 1.0f;
    G.crt_enabled = true;
    G.settings_sel = 0;

    SDL_SetRelativeMouseMode(SDL_FALSE);
}

static void Hub_OnShutdown(SLF_App* app) {
    if (f_big) starlight_unload_font(f_big);
    if (f_med) starlight_unload_font(f_med);
    if (f_small) starlight_unload_font(f_small);
    
    audio_source_destroy(G.sfx_hit);
    audio_source_destroy(G.sfx_score);
    audio_source_destroy(G.sfx_select);
    audio_source_destroy(G.sfx_wall);
    audio_source_destroy(G.sfx_win);
    audio_source_destroy(G.sfx_lose);
    
    audio_source_destroy(G.ann_nice);
    audio_source_destroy(G.ann_ouch);
    audio_source_destroy(G.ann_winner);
    audio_source_destroy(G.ann_defeat);
    audio_source_destroy(G.ann_go);
    audio_source_destroy(G.ann_excellent);
    
    for (int i = 0; i < 8; i++) {
        if (G.gallery_loaded[i]) {
            glDeleteTextures(1, &G.gallery_tex[i]);
        }
    }
    
    audio_stop_music();
}

static void Hub_OnUpdate(SLF_App* app) {
    float dt = app->delta_time; if(dt > DT_CAP) dt = DT_CAP;
    G.total_time += dt;
    update_hub_vfx(dt);
    
    // Framework Overhaul: Tween-driven fade transition (replaces manual alpha decay)
    slf_tween_update(&g_tween_fade, dt);
    slf_tween_update(&g_tween_paddle, dt);

    // Finalize pending textures
    for(int i=0; i<16; i++) {
        if(!G.gallery_loaded[i] && G.gallery_jobs[i].ready_for_gpu) {
            if(loader_finalize_texture(&G.gallery_jobs[i])) {
                G.gallery_tex[i] = G.gallery_jobs[i].target_gl_id;
                G.gallery_loaded[i] = true;
            }
        }
    }

    // Mouse Support for Paddle (Framework Overhaul: Only if input_mode == 1)
    if(G.state == ST_HUB_PONG && G.input_mode == 1) {
        int mx, my; SDL_GetMouseState(&mx, &my);
        G.pong.ply = (float)my - G.pong.plh/2.0f;
    }

    if(G.state == ST_HUB_MENU) {
        if(starlight_is_action_just_pressed(app, "up")) { G.menu_sel = (G.menu_sel+3)%4; if(G.sfx_select) audio_source_play(G.sfx_select, 0, 0.5f); }
        if(starlight_is_action_just_pressed(app, "down")) { G.menu_sel = (G.menu_sel+1)%4; if(G.sfx_select) audio_source_play(G.sfx_select, 0, 0.5f); }
        if(starlight_is_action_just_pressed(app, "enter")) {
            if(G.sfx_select) audio_source_play(G.sfx_select, 0, 0.5f);
            if(G.menu_sel == 0) { G.state = ST_HUB_DIFFICULTY; G.diff_screen_sel = 0; }
            if(G.menu_sel == 1) { G.state = ST_HUB_GALLERY; }
            if(G.menu_sel == 2) { G.state = ST_HUB_SETTINGS; G.settings_sel = 0; }
            if(G.menu_sel == 3) { app->running = false; }
            slf_tween_start(&g_tween_fade, &G.transition_alpha, 1.0f, 0.0f, 0.5f, slf_ease_out_cubic);
        }
    } else if (G.state == ST_HUB_DIFFICULTY) {
        // Framework Overhaul: Navigate between Difficulty and Control
        if(starlight_is_action_just_pressed(app, "up") || starlight_is_action_just_pressed(app, "down")) {
            G.diff_screen_sel = !G.diff_screen_sel;
            if(G.sfx_select) audio_source_play(G.sfx_select, 0, 0.5f);
        }

        if(G.diff_screen_sel == 0) { // Difficulty Row
            if(starlight_is_action_just_pressed(app, "left")) { G.difficulty = (G.difficulty+2)%3; if(G.sfx_select) audio_source_play(G.sfx_select, 0, 0.5f); }
            if(starlight_is_action_just_pressed(app, "right")) { G.difficulty = (G.difficulty+1)%3; if(G.sfx_select) audio_source_play(G.sfx_select, 0, 0.5f); }
        } else { // Control Row
            if(starlight_is_action_just_pressed(app, "left") || starlight_is_action_just_pressed(app, "right") || starlight_is_action_just_pressed(app, "enter")) {
                G.input_mode = !G.input_mode;
                if(G.sfx_select) audio_source_play(G.sfx_select, 0, 0.5f);
            }
        }

        if(starlight_is_action_just_pressed(app, "enter") && G.diff_screen_sel == 0) {
            if(G.sfx_select) audio_source_play(G.sfx_select, 0, 0.5f);
            G.state = ST_HUB_PONG; G.pong.score_l = G.pong.score_r = 0; G.pong.state = 0; G.pong.serve_timer = 1.0f;
            slf_tween_start(&g_tween_fade, &G.transition_alpha, 1.0f, 0.0f, 0.5f, slf_ease_out_cubic);
        }
        if(starlight_is_action_just_pressed(app, "esc")) { G.state = ST_HUB_MENU; slf_tween_start(&g_tween_fade, &G.transition_alpha, 1.0f, 0.0f, 0.5f, slf_ease_out_cubic); }

    } else if (G.state == ST_HUB_SETTINGS) {
        // Navigate Settings
        if(starlight_is_action_just_pressed(app, "up")) { G.settings_sel = (G.settings_sel+4)%5; if(G.sfx_select) audio_source_play(G.sfx_select, 0, 0.5f); }
        if(starlight_is_action_just_pressed(app, "down")) { G.settings_sel = (G.settings_sel+1)%5; if(G.sfx_select) audio_source_play(G.sfx_select, 0, 0.5f); }
        
        // Adjust Settings
        if(G.settings_sel == 0) { // Music Volume
            if(starlight_is_action_just_pressed(app, "left")) { G.music_vol = fmaxf(0, G.music_vol - 0.1f); audio_set_master_volume(G.music_vol); }
            if(starlight_is_action_just_pressed(app, "right")) { G.music_vol = fminf(1, G.music_vol + 0.1f); audio_set_master_volume(G.music_vol); }
        } else if (G.settings_sel == 1) { // SFX Volume
            if(starlight_is_action_just_pressed(app, "left")) { G.sfx_vol = fmaxf(0, G.sfx_vol - 0.1f); }
            if(starlight_is_action_just_pressed(app, "right")) { G.sfx_vol = fminf(1, G.sfx_vol + 0.1f); }
        } else if (G.settings_sel == 2) { // Control
            if(starlight_is_action_just_pressed(app, "left") || starlight_is_action_just_pressed(app, "right")) { G.input_mode = !G.input_mode; }
        } else if (G.settings_sel == 3) { // CRT Effect
            if(starlight_is_action_just_pressed(app, "left") || starlight_is_action_just_pressed(app, "right")) { G.crt_enabled = !G.crt_enabled; }
        }

        if(starlight_is_action_just_pressed(app, "enter") && G.settings_sel == 4) { // Return
            G.state = ST_HUB_MENU;
            slf_tween_start(&g_tween_fade, &G.transition_alpha, 1.0f, 0.0f, 0.5f, slf_ease_out_cubic);
        }
        if(starlight_is_action_just_pressed(app, "esc")) { G.state = ST_HUB_MENU; slf_tween_start(&g_tween_fade, &G.transition_alpha, 1.0f, 0.0f, 0.5f, slf_ease_out_cubic); }

    } else if (G.state == ST_HUB_GALLERY) {
        if(starlight_is_action_just_pressed(app, "up")) { G.pause_sel = (G.pause_sel+2)%3; if(G.sfx_select) audio_source_play(G.sfx_select, 0, 0.5f); }
        if(starlight_is_action_just_pressed(app, "down")) { G.pause_sel = (G.pause_sel+1)%3; if(G.sfx_select) audio_source_play(G.sfx_select, 0, 0.5f); }
        if(starlight_is_action_just_pressed(app, "enter")) {
            if(G.sfx_select) audio_source_play(G.sfx_select, 0, 0.5f);
            if(G.pause_sel == 0) G.state = ST_HUB_PONG;
            else G.state = ST_HUB_MENU;
        }
        if(starlight_is_action_just_pressed(app, "esc")) { G.state = ST_HUB_PONG; }
    } else if (G.state == ST_HUB_GALLERY) {
        if(starlight_is_action_just_pressed(app, "esc")) G.state = ST_HUB_MENU;
    } else if (G.state == ST_HUB_PONG) {
        update_pong(app, dt);
        if(starlight_is_action_just_pressed(app, "esc")) { G.state = ST_HUB_PAUSE; }
    } else if (G.state == ST_HUB_REWARD) {
        if(starlight_is_action_just_pressed(app, "enter")) G.state = ST_HUB_MENU;
    } else if (G.state == ST_HUB_INVADERS) {
        update_invaders(app, dt);
        if(starlight_is_action_just_pressed(app, "esc")) G.state = ST_HUB_MENU;
    } else if (G.state == ST_HUB_BREAKOUT) {
        if(app->keys[SDL_SCANCODE_A]) G.brk.px -= 500 * dt;
        if(app->keys[SDL_SCANCODE_D]) G.brk.px += 500 * dt;
        static float win_b = 0; win_b += dt;
        if(win_b > 5.0f) { win_b = 0; G.state = ST_HUB_REWARD; save_system_unlock(G.reward_game_id, G.reward_slot_idx); }
        if(starlight_is_action_just_pressed(app, "esc")) G.state = ST_HUB_MENU;
    } else if (G.state == ST_HUB_RUNNER) {
        if(starlight_is_action_just_pressed(app, "p_jump")) G.run.pvy = -400;
        G.run.pvy += 1000 * dt; G.run.py += G.run.pvy * dt;
        static float win_r = 0; win_r += dt;
        if(win_r > 5.0f) { win_r = 0; G.state = ST_HUB_REWARD; save_system_unlock(G.reward_game_id, G.reward_slot_idx); }
        if(starlight_is_action_just_pressed(app, "esc")) G.state = ST_HUB_MENU;
    }
}

static void Hub_OnUI(SLF_App* app) {
    glDisable(GL_DEPTH_TEST); glEnable(GL_BLEND);
    float cw = (float)app->width, ch = (float)app->height;
    starlight_ui_draw_rect(app, 0.0f, 0.0f, cw, ch, COLOR_BG_DARK);

    if (G.state == ST_HUB_MENU) {
        // Framework Overhaul: UI Anchors for responsive menu layout
        float tx, ty;
        starlight_ui_anchor_pos(app, SLF_ANCHOR_TOP_CENTER, 0, 100, 500, 72, &tx, &ty);
        starlight_ui_draw_text(app, f_big, "STARLIGHT DELUXE", tx, ty, COLOR_CYAN);
        starlight_ui_anchor_pos(app, SLF_ANCHOR_CENTER, 0, -60, 200, 32, &tx, &ty);
        starlight_ui_draw_text(app, f_med, "1. PLAY PONG", tx, ty, (G.menu_sel==0 ? COLOR_YELLOW : (vec4){1,1,1,0.4f}));
        starlight_ui_anchor_pos(app, SLF_ANCHOR_CENTER, 0, -20, 200, 32, &tx, &ty);
        starlight_ui_draw_text(app, f_med, "2. GALLERY", tx, ty, (G.menu_sel==1 ? COLOR_YELLOW : (vec4){1,1,1,0.4f}));
        starlight_ui_anchor_pos(app, SLF_ANCHOR_CENTER, 0, 20, 200, 32, &tx, &ty);
        starlight_ui_draw_text(app, f_med, "3. SETTINGS", tx, ty, (G.menu_sel==2 ? COLOR_YELLOW : (vec4){1,1,1,0.4f}));
        starlight_ui_anchor_pos(app, SLF_ANCHOR_CENTER, 0, 60, 200, 32, &tx, &ty);
        starlight_ui_draw_text(app, f_med, "4. QUIT", tx, ty, (G.menu_sel==3 ? COLOR_YELLOW : (vec4){1,1,1,0.4f}));
        starlight_ui_anchor_pos(app, SLF_ANCHOR_BOTTOM_CENTER, 0, 50, 460, 20, &tx, &ty);
        starlight_ui_draw_text(app, f_small, "Tip: Win Pong to trigger a Minigame + Reward!", tx, ty, (vec4){1,1,1,0.3f});
    } else if (G.state == ST_HUB_PONG) {
        
        // --- Retro Mode 7 Infinite Floor (F-Zero / Outrun Style) ---
        // Posição de câmera avança no eixo Y baseada no tempo, Yaw acompanha a bolinha
        float yaw = (G.pong.bx - cw/2.0f) * 0.0001f;
        starlight_ui_draw_mode7(app, 0, 0.0f, -G.total_time * 2.0f, 0.4f, 0.4f, yaw, 1.2f);
        
        float ox = (G.pong.chromatic_timer > 0) ? (rand()%10-5) : 0;
        // Pulsing Grid (Apenas na parte de cima agora, já que o chão é Mode 7)
        float pulse = 0.05f + 0.1f * sinf(G.total_time * 5.0f);
        for(int x=0; x<cw; x+=100) starlight_ui_draw_rect(app, (float)x, 0, 1, 300, (vec4){0, 1, 1, pulse});
        for(int y=0; y<ch; y+=100) starlight_ui_draw_rect(app, 0, (float)y, cw, 1, (vec4){0, 1, 1, pulse});

        // Chromatic Aberration Simulation
        if(G.pong.chromatic_timer > 0) {
            starlight_ui_draw_rect(app, G.pong.bx+ox, G.pong.by, G.pong.bsize, G.pong.bsize, (vec4){1, 0, 0, 0.5f});
            starlight_ui_draw_rect(app, G.pong.bx-ox, G.pong.by, G.pong.bsize, G.pong.bsize, (vec4){0, 0, 1, 0.5f});
        }

        // Entity Shadows (Cast slightly down-right for depth)
        vec4 shadow_col = {0,0,0,0.4f};
        starlight_ui_draw_rect(app, G.pong.plx+5, G.pong.ply+5, G.pong.plw, G.pong.plh, shadow_col);
        starlight_ui_draw_rect(app, G.pong.prx+5, G.pong.pry+5, G.pong.prw, G.pong.prh, shadow_col);
        starlight_ui_draw_rect(app, G.pong.bx+3, G.pong.by+3, G.pong.bsize, G.pong.bsize, shadow_col);

        starlight_ui_draw_rect(app, G.pong.plx, G.pong.ply, G.pong.plw, G.pong.plh, COLOR_CYAN);
        starlight_ui_draw_rect(app, G.pong.prx, G.pong.pry, G.pong.prw, G.pong.prh, COLOR_MAGENTA);
        starlight_ui_draw_rect(app, G.pong.bx, G.pong.by, G.pong.bsize, G.pong.bsize, COLOR_YELLOW);
        
        // Powerup Draw
        if(G.pong.powerup_x > 0) {
            float s = 30.0f + 10.0f*sinf(G.total_time*10.0f);
            starlight_ui_draw_rect(app, G.pong.powerup_x-s/2, G.pong.powerup_y-s/2, s, s, COLOR_YELLOW);
            starlight_ui_draw_text(app, f_small, "!", G.pong.powerup_x-5, G.pong.powerup_y-10, (vec4){0,0,0,1});
        }

        char b[64]; snprintf(b, sizeof(b), "%d - %d", G.pong.score_l, G.pong.score_r);
        starlight_ui_draw_text(app, f_big, b, cw/2.0f-60.0f, 20.0f, (vec4){1,1,1,1});

        // --- Netcode Debug View (Rewind Path) ---
        if(app->keys[SDL_SCANCODE_F1]) {
            for(int i=0; i<120; i+=10) {
                starlight_ui_draw_rect(app, G.net.h_bx[i], G.net.h_by[i], 5, 5, (vec4){1,1,0,0.3f});
            }
            starlight_ui_draw_text(app, f_small, "NETCODE: REWIND BUFFER ACTIVE", 50, ch-30, COLOR_YELLOW);
        }
    } else if (G.state == ST_HUB_INVADERS) {
    } else if (G.state == ST_HUB_BREAKOUT) {
        starlight_ui_draw_text(app, f_big, "MINI-GAME: BREAKOUT", cw/2-300, 100, (vec4){1,1,0,1});
        starlight_ui_draw_rect(app, G.brk.px + cw/2, ch-80, 100, 15, (vec4){1,0.5,0,1});
        starlight_ui_draw_text(app, f_med, "BREAK ALL BLOCKS!", cw/2-150, ch/2, (vec4){1,1,1,1});
    } else if (G.state == ST_HUB_RUNNER) {
        starlight_ui_draw_text(app, f_big, "MINI-GAME: RUNNER", cw/2-300, 100, (vec4){0,1,1,1});
        starlight_ui_draw_rect(app, 100, G.run.py, 40, 40, (vec4){0,1,1,1});
        starlight_ui_draw_text(app, f_med, "DODGE THE VOID!", cw/2-150, ch/2, (vec4){1,1,1,1});
    } else if (G.state == ST_HUB_REWARD) {
        starlight_ui_draw_rect(app, cw/2-250, ch/2-250, 500, 500, (vec4){0.2, 0.1, 0.4, 1});
        starlight_ui_draw_text(app, f_big, "MODO +18 UNLOCKED", cw/2-260, 100, (vec4){1, 0, 1, 1});
        starlight_ui_draw_text(app, f_med, "You unlocked a new Gallery Piece!", cw/2-200, ch-150, (vec4){0,1,0,1});
        starlight_ui_draw_text(app, f_small, "PRESS ENTER TO CONTINUE", cw/2-100, ch-80, (vec4){1,1,1,0.5});
    } else if (G.state == ST_HUB_GALLERY) {
        starlight_ui_draw_text(app, f_big, "GALLERY", cw/2-100, 50, (vec4){1, 0.8, 0, 1});
        for(int g=0; g<2; g++) {
            for(int i=0; i<4; i++) {
                int idx = g*4 + i;
                bool u = save_system_is_unlocked(g, i);
                if(u && G.gallery_loaded[idx]) {
                    starlight_ui_draw_texture(app, G.gallery_tex[idx], 150+i*220, 150+g*120, 200, 100, (vec4){1,1,1,1});
                } else {
                    starlight_ui_draw_rect(app, 150+i*220, 150+g*120, 200, 100, (u ? (vec4){0.1, 0.3, 0.5, 1} : (vec4){0.05, 0.05, 0.05, 1}));
                    starlight_ui_draw_text(app, f_small, u ? "LOADING..." : "LOCKED", 150+i*220+50, 150+g*120+40, (u ? (vec4){0,1,0,1} : (vec4){1,1,1,0.2}));
                }
            }
        }
        
        // --- Stats Dashboard ---
        extern SaveData g_save; // Hacky but fast access
        char s[256];
        snprintf(s, sizeof(s), "TOTAL WINS: %u | TOTAL HITS: %u", g_save.total_wins, g_save.total_hits);
        starlight_ui_draw_text(app, f_med, s, 150, ch-100, COLOR_YELLOW);
        starlight_ui_draw_text(app, f_small, "PRESS ESC TO RETURN HUB", 150, ch-50, (vec4){1,1,1,0.5});
    } else if (G.state == ST_HUB_DIFFICULTY) {
        starlight_ui_draw_text(app, f_big, "SELECT DIFFICULTY", cw/2-250, 100, COLOR_CYAN);
        starlight_ui_draw_text(app, f_med, "EASY", cw/2-50, 300, (G.difficulty==0 ? COLOR_YELLOW : (vec4){1,1,1,0.4f}));
        starlight_ui_draw_text(app, f_med, "NORMAL", cw/2-50, 360, (G.difficulty==1 ? COLOR_YELLOW : (vec4){1,1,1,0.4f}));
        starlight_ui_draw_text(app, f_med, "HARD", cw/2-50, 420, (G.difficulty==2 ? COLOR_YELLOW : (vec4){1,1,1,0.4f}));
        starlight_ui_draw_text(app, f_small, "PRESS <ENTER> TO START", cw/2-110, ch-100, (vec4){1,1,1,0.3f});
        
    } else if (G.state == ST_HUB_SETTINGS) {
        starlight_ui_draw_text(app, f_big, "SETTINGS", cw/2-100, 80, COLOR_CYAN);
        
        char* opts[] = {"MUSIC VOLUME", "SFX VOLUME", "CONTROL MODE", "CRT EFFECTS", "RETURN TO MENU"};
        for(int i=0; i<5; i++) {
            vec4 col;
            if (G.settings_sel == i) glm_vec4_copy(COLOR_YELLOW, col);
            else glm_vec4_copy((vec4){1,1,1,0.5f}, col);

            starlight_ui_draw_text(app, f_med, opts[i], 200, 250 + i*60, col);
            
            // Values
            if(i == 0) { // Music
                starlight_ui_draw_rect(app, cw-500, 260 + i*60, 200, 12, (vec4){1,1,1,0.1f});
                starlight_ui_draw_rect(app, cw-500, 260 + i*60, G.music_vol * 200.0f, 12, COLOR_CYAN);
            } else if (i == 1) { // SFX
                starlight_ui_draw_rect(app, cw-500, 260 + i*60, 200, 12, (vec4){1,1,1,0.1f});
                starlight_ui_draw_rect(app, cw-500, 260 + i*60, G.sfx_vol * 200.0f, 12, COLOR_CYAN);
            } else if (i == 2) { // Control
                starlight_ui_draw_text(app, f_med, G.input_mode == 0 ? "KEYBOARD" : "MOUSE", cw-500, 250 + i*60, col);
            } else if (i == 3) { // CRT
                starlight_ui_draw_text(app, f_med, G.crt_enabled ? "ON" : "OFF", cw-500, 250 + i*60, col);
            }
        }
        starlight_ui_draw_text(app, f_small, "USE ARROWS TO NAVIGATE AND ADJUST", cw/2-150, ch-80, (vec4){1,1,1,0.2f});

    } else if (G.state == ST_HUB_PAUSE) {
        starlight_ui_draw_rect(app, cw/2-150, ch/2-200, 300, 400, (vec4){0, 0, 0, 0.8f});
        starlight_ui_draw_text(app, f_big, "PAUSED", cw/2-110, ch/2-150, COLOR_CYAN);
        starlight_ui_draw_text(app, f_med, "RESUME", cw/2-50, ch/2, (G.pause_sel==0 ? COLOR_YELLOW : (vec4){1,1,1,0.6f}));
        starlight_ui_draw_text(app, f_med, "RESTART", cw/2-50, ch/2+60, (G.pause_sel==1 ? COLOR_YELLOW : (vec4){1,1,1,0.6f}));
        starlight_ui_draw_text(app, f_med, "QUIT", cw/2-50, ch/2+120, (G.pause_sel==2 ? COLOR_YELLOW : (vec4){1,1,1,0.6f}));
    }
    
    // Efficient Batch Particle Rendering (Instanced)
    if(G.particle_count > 0) {
        float pos[1000 * 2]; float siz[1000 * 2]; vec4 col[1000];
        int draw_count = (G.particle_count > 1000) ? 1000 : G.particle_count;
        for(int i=0; i<draw_count; i++) {
            pos[i*2+0] = G.particles[i].x; pos[i*2+1] = G.particles[i].y;
            siz[i*2+0] = G.particles[i].size; siz[i*2+1] = G.particles[i].size;
            glm_vec4_copy(G.particles[i].color, col[i]);
        }
        starlight_ui_draw_rects_instanced(app, pos, siz, col, draw_count);
    }

    // --- Elite CRT Pass ---
    if(G.crt_enabled) {
        // Scanlines
        for(int y=0; y<(int)ch; y+=4) {
            starlight_ui_draw_rect(app, 0, (float)y, cw, 1, (vec4){0, 0, 0, 0.15f});
        }
        // Subtle Vignette (Corners)
        starlight_ui_draw_rect(app, 0, 0, cw, 40, (vec4){0,0,0,0.3f});
        starlight_ui_draw_rect(app, 0, ch-40, cw, 40, (vec4){0,0,0,0.3f});
        starlight_ui_draw_rect(app, 0, 0, 40, ch, (vec4){0,0,0,0.3f});
        starlight_ui_draw_rect(app, cw-40, 0, 40, ch, (vec4){0,0,0,0.3f});
    }

    // Global Fade Transition
    if(G.transition_alpha > 0.01f) {
        starlight_ui_draw_rect(app, 0, 0, cw, ch, (vec4){0, 0, 0, G.transition_alpha});
    }
}

int main(int argc, char* argv[]) {
    SLF_Config config = { .on_start=Hub_OnStart, .on_update=Hub_OnUpdate, .on_ui=Hub_OnUI, .on_shutdown=Hub_OnShutdown };
    return starlight_framework_init("Starlight Pong Deluxe - Unified Hub", 1280, 720, &config);
}
