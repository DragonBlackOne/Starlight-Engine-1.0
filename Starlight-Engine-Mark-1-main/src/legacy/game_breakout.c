/**
 * NEON BREAKOUT 2D
 * ================
 * Classic Brick-breaking with neon glow.
 * Unlocks Game ID 2 Slots in Gallery.
 */
#include "starlight.h"
#include "save_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BRICK_ROWS    5
#define BRICK_COLS    10
#define MAX_BRICKS    (BRICK_ROWS * BRICK_COLS)
#define MAX_PARTICLES 100

typedef enum { ST_MENU, ST_PLAY, ST_WIN, ST_GAMEOVER } GameState;

typedef struct { float x, y, w, h; int active; vec4 color; } Brick;
typedef struct { float x, y, vx, vy, size; int active; } Ball;
typedef struct { float x, y, vx, vy, life, size; vec4 color; } Particle;

typedef struct {
    GameState state;
    float paddle_x, paddle_w;
    Ball ball;
    Brick bricks[MAX_BRICKS];
    int bricks_left;
    int score, lives;
    
    Particle particles[MAX_PARTICLES];
    int particle_count;
    float total_time;
    
    AudioSource* sfx_hit;
    AudioSource* sfx_wall;
    AudioSource* sfx_break;
} BreakoutGame;

static BreakoutGame G;
static SLF_Font *g_font, *g_font_big;

static void emit(float x, float y, vec4 col, int count) {
    for (int i = 0; i < count && G.particle_count < MAX_PARTICLES; i++) {
        Particle* p = &G.particles[G.particle_count++];
        p->x = x; p->y = y;
        float a = ((rand()%360)*3.14f)/180.0f; float s = 60.0f + rand()%80;
        p->vx = cosf(a)*s; p->vy = sinf(a)*s;
        p->life = 0.5f; p->size = 2.0f + rand()%3; glm_vec4_copy(col, p->color);
    }
}

static void reset_bricks() {
    float bw = 100, bh = 30, gap = 10;
    float start_x = (1280 - (BRICK_COLS * (bw + gap))) / 2;
    float start_y = 100;
    G.bricks_left = 0;
    for(int r=0; r<BRICK_ROWS; r++) {
        for(int c=0; c<BRICK_COLS; c++) {
            Brick* b = &G.bricks[G.bricks_left++];
            b->x = start_x + c * (bw + gap); b->y = start_y + r * (bh + gap);
            b->w = bw; b->h = bh; b->active = 1;
            float hue = (float)r/BRICK_ROWS;
            glm_vec4_copy((vec4){1-hue, hue, 0.5+0.5*hue, 1}, b->color);
        }
    }
}

static void Breakout_OnStart(SLF_App* app) {
    save_system_init();
    memset(&G, 0, sizeof(G));
    G.state = ST_MENU; G.lives = 3;
    G.paddle_x = (float)app->width/2 - 60; G.paddle_w = 120;
    G.ball = (Ball){ (float)app->width/2, (float)app->height-150, 300, -300, 12, 1 };
    
    g_font = starlight_load_font("assets/fonts/Inconsolata-Regular.ttf", 32.0f);
    g_font_big = starlight_load_font("assets/fonts/Inconsolata-Regular.ttf", 64.0f);
    
    G.sfx_hit = audio_create_source("assets/audio/ball_hit_1.mp3");
    G.sfx_wall = audio_create_source("assets/audio/menu_click_1.mp3");
    G.sfx_break = audio_create_source("assets/audio/score_point_1.mp3");
    
    reset_bricks();
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

static void Breakout_OnUpdate(SLF_App* app) {
    float dt = app->delta_time; if(dt > 0.05) dt = 0.05f;
    G.total_time += dt;

    for(int i=0; i<G.particle_count; i++) {
        G.particles[i].life -= dt;
        if(G.particles[i].life <= 0) { G.particles[i] = G.particles[--G.particle_count]; i--; continue; }
        G.particles[i].x += G.particles[i].vx * dt; G.particles[i].y += G.particles[i].vy * dt;
        G.particles[i].color[3] = G.particles[i].life / 0.5f;
    }

    if(G.state == ST_MENU) {
        if(app->keys[SDL_SCANCODE_SPACE] || app->keys[SDL_SCANCODE_RETURN]) G.state = ST_PLAY;
        return;
    }

    if(G.state == ST_PLAY) {
        // Paddle
        if(app->keys[SDL_SCANCODE_A] || app->keys[SDL_SCANCODE_LEFT]) G.paddle_x -= 600 * dt;
        if(app->keys[SDL_SCANCODE_D] || app->keys[SDL_SCANCODE_RIGHT]) G.paddle_x += 600 * dt;
        G.paddle_x = fmaxf(10, fminf(app->width-G.paddle_w-10, G.paddle_x));

        // Ball
        G.ball.x += G.ball.vx * dt; G.ball.y += G.ball.vy * dt;
        
        // Walls
        if(G.ball.x < 0 || G.ball.x > app->width-G.ball.size) { G.ball.vx *= -1; if(G.sfx_wall) audio_source_play(G.sfx_wall,0,0.3); }
        if(G.ball.y < 0) { G.ball.vy *= -1; if(G.sfx_wall) audio_source_play(G.sfx_wall,0,0.3); }
        if(G.ball.y > app->height) { 
            G.lives--; G.ball = (Ball){G.paddle_x+G.paddle_w/2, app->height-150, 300, -300, 12, 1};
            if(G.lives <= 0) G.state = ST_GAMEOVER;
        }

        // Paddle Collision
        if(G.ball.vy > 0 && G.ball.y+G.ball.size > app->height-100 && G.ball.x > G.paddle_x && G.ball.x < G.paddle_x+G.paddle_w) {
            G.ball.vy *= -1.05f; if(G.sfx_hit) audio_source_play(G.sfx_hit,0,0.5);
            float hit = (G.ball.x+G.ball.size/2 - (G.paddle_x+G.paddle_w/2))/(G.paddle_w/2);
            G.ball.vx = 400 * hit;
        }

        // Bricks Collision
        int active_bricks = 0;
        for(int i=0; i<MAX_BRICKS; i++) {
            Brick* b = &G.bricks[i];
            if(!b->active) continue;
            active_bricks++;
            if(G.ball.x+G.ball.size > b->x && G.ball.x < b->x+b->w && G.ball.y+G.ball.size > b->y && G.ball.y < b->y+b->h) {
                b->active = 0; G.ball.vy *= -1; G.score += 50;
                emit(b->x+b->w/2, b->y+b->h/2, b->color, 6);
                if(G.sfx_break) audio_source_play(G.sfx_break,0,0.4);
                break;
            }
        }
        if(active_bricks == 0) { 
            G.state = ST_WIN; 
            save_system_unlock(2, 0); // Unlock Slot for Breakout (Game 2)
        }
    }
}

static void Breakout_OnUI(SLF_App* app) {
    glDisable(GL_DEPTH_TEST); glEnable(GL_BLEND);
    float cw = (float)app->width, ch = (float)app->height;
    starlight_ui_draw_rect(app, 0, 0, cw, ch, (vec4){0.04, 0.01, 0.01, 1});

    // Paddle
    starlight_ui_draw_rect(app, G.paddle_x, ch-100, G.paddle_w, 15, (vec4){1, 0.4, 0, 1});
    // Ball
    starlight_ui_draw_rect(app, G.ball.x, G.ball.y, G.ball.size, G.ball.size, (vec4){1, 1, 1, 1});
    // Bricks
    for(int i=0; i<MAX_BRICKS; i++) {
        if(G.bricks[i].active) starlight_ui_draw_rect(app, G.bricks[i].x, G.bricks[i].y, G.bricks[i].w, G.bricks[i].h, G.bricks[i].color);
    }
    // Particles
    for(int i=0; i<G.particle_count; i++) starlight_ui_draw_rect(app, G.particles[i].x, G.particles[i].y, G.particles[i].size, G.particles[i].size, G.particles[i].color);

    // Hud
    char b[64]; snprintf(b, sizeof(b), "SCORE: %d", G.score); starlight_ui_draw_text(app, g_font, b, 20, 20, (vec4){1,1,1,1});
    snprintf(b, sizeof(b), "LIVES: %d", G.lives); starlight_ui_draw_text(app, g_font, b, cw-150, 20, (vec4){1,0,0,1});

    if(G.state == ST_MENU) {
        starlight_ui_draw_rect(app, 0, 0, cw, ch, (vec4){0,0,0,0.6});
        starlight_ui_draw_text(app, g_font_big, "NEON BREAKOUT", cw/2-200, ch/2-100, (vec4){1,0.5,0,1});
        starlight_ui_draw_text(app, g_font, "PRESS ENTER TO START", cw/2-150, ch/2+20, (vec4){1,1,1,0.8});
    } else if(G.state == ST_WIN) {
        starlight_ui_draw_text(app, g_font_big, "YOU WIN!", cw/2-100, ch/2, (vec4){0,1,0,1});
    }
}

int main(int argc, char* argv[]) {
    SLF_Config config = { .on_start=Breakout_OnStart, .on_update=Breakout_OnUpdate, .on_ui=Breakout_OnUI };
    return starlight_framework_init("Neon Breakout 2D", 1280, 720, &config);
}
