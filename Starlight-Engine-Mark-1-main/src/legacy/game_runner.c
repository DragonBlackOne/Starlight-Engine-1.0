// Este projeto é feito por IA e só o prompt é feito por um humano.
/**
 * VOID RUNNER 2D
 * ==============
 * Side-scrolling obstacle avoidance.
 * Unlocks Game ID 3 Slots in Gallery.
 */
#include "starlight.h"
#include "save_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_OBSTACLES 16
#define MAX_PARTICLES 80

typedef enum { ST_MENU, ST_PLAY, ST_WIN, ST_GAMEOVER } GameState;

typedef struct { float x, y, w, h; int active; } Obstacle;
typedef struct { float x, y, vx, vy, life, size; vec4 color; } Particle;

typedef struct {
    GameState state;
    float player_y, player_vy;
    Obstacle obs[MAX_OBSTACLES];
    float obs_timer;
    float speed;
    float distance;
    int score;
    
    Particle particles[MAX_PARTICLES];
    int particle_count;
    float total_time;
    
    AudioSource* sfx_jump;
    AudioSource* sfx_hit;
} RunnerGame;

static RunnerGame G;
static SLF_Font *g_font, *g_font_big;

static void emit(float x, float y, vec4 col, int count) {
    for (int i = 0; i < count && G.particle_count < MAX_PARTICLES; i++) {
        Particle* p = &G.particles[G.particle_count++];
        p->x = x; p->y = y;
        float a = ((rand()%360)*3.14f)/180.0f; float s = 40.0f + rand()%60;
        p->vx = cosf(a)*s; p->vy = sinf(a)*s;
        p->life = 0.4f; p->size = 2.0f + rand()%2; glm_vec4_copy(col, p->color);
    }
}

static void Runner_OnStart(SLF_App* app) {
    save_system_init();
    memset(&G, 0, sizeof(G));
    G.state = ST_MENU; G.player_y = (float)app->height/2; G.speed = 400.0f;
    
    g_font = starlight_load_font("assets/fonts/Inconsolata-Regular.ttf", 32.0f);
    g_font_big = starlight_load_font("assets/fonts/Inconsolata-Regular.ttf", 64.0f);
    
    G.sfx_jump = audio_create_source("assets/audio/ball_hit_1.mp3");
    G.sfx_hit = audio_create_source("assets/audio/score_point_1.mp3");
    
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

static void Runner_OnUpdate(SLF_App* app) {
    float dt = app->delta_time; if(dt > 0.05) dt = 0.05f;
    G.total_time += dt;

    for(int i=0; i<G.particle_count; i++) {
        G.particles[i].life -= dt;
        if(G.particles[i].life <= 0) { G.particles[i] = G.particles[--G.particle_count]; i--; continue; }
        G.particles[i].x += G.particles[i].vx * dt; G.particles[i].y += G.particles[i].vy * dt;
        G.particles[i].color[3] = G.particles[i].life / 0.4f;
    }

    if(G.state == ST_MENU) {
        if(app->keys[SDL_SCANCODE_SPACE] || app->keys[SDL_SCANCODE_RETURN]) G.state = ST_PLAY;
        return;
    }

    if(G.state == ST_PLAY) {
        // Player Gravity & Jump
        G.player_vy += 1200 * dt;
        if(starlight_is_action_just_pressed(app, "Ok") || app->keys[SDL_SCANCODE_SPACE]) {
            G.player_vy = -450; if(G.sfx_jump) audio_source_play(G.sfx_jump,0,0.3);
            emit(120, G.player_y+15, (vec4){0, 1, 1, 0.5}, 3);
        }
        G.player_y += G.player_vy * dt;
        if(G.player_y < 0 || G.player_y > app->height-40) G.state = ST_GAMEOVER;

        // Speed Escalate
        G.speed += 5 * dt;
        G.distance += G.speed * dt;
        G.score = (int)(G.distance/100);

        // Obstacles
        G.obs_timer -= dt;
        if(G.obs_timer <= 0) {
            for(int i=0; i<MAX_OBSTACLES; i++) {
                if(!G.obs[i].active) {
                    float gap = 200 - (G.speed/10); if(gap < 120) gap = 120;
                    G.obs[i] = (Obstacle){ (float)app->width, (float)(rand()%(int)(app->height-gap)), 60, gap, 1 };
                    G.obs_timer = 1.8f - (G.speed/1000); if(G.obs_timer < 0.6f) G.obs_timer = 0.6f;
                    break;
                }
            }
        }

        for(int i=0; i<MAX_OBSTACLES; i++) {
            if(G.obs[i].active) {
                G.obs[i].x -= G.speed * dt;
                if(G.obs[i].x < -100) G.obs[i].active = 0;
                // Collision (Top/Bottom parts)
                bool hit_top = (100 < G.obs[i].x+G.obs[i].w && 140 > G.obs[i].x && G.player_y < G.obs[i].y);
                bool hit_bot = (100 < G.obs[i].x+G.obs[i].w && 140 > G.obs[i].x && G.player_y+40 > G.obs[i].y+G.obs[i].h);
                if(hit_top || hit_bot) G.state = ST_GAMEOVER;
            }
        }
        
        // Progression
        if(G.score >= 100) save_system_unlock(3, 0);
        if(G.score >= 500) save_system_unlock(3, 1);
    }
}

static void Runner_OnUI(SLF_App* app) {
    glDisable(GL_DEPTH_TEST); glEnable(GL_BLEND);
    float cw = (float)app->width, ch = (float)app->height;
    starlight_ui_draw_rect(app, 0, 0, cw, ch, (vec4){0, 0.02, 0.04, 1});

    // Player
    starlight_ui_draw_rect(app, 100, G.player_y, 40, 40, (vec4){0, 1, 1, 1});
    starlight_ui_draw_rect(app, 110, G.player_y+10, 20, 20, (vec4){1, 1, 1, 1});

    // Obstacles
    for(int i=0; i<MAX_OBSTACLES; i++) {
        if(G.obs[i].active) {
            starlight_ui_draw_rect(app, G.obs[i].x, 0, G.obs[i].w, G.obs[i].y, (vec4){1, 0, 0.4, 1});
            starlight_ui_draw_rect(app, G.obs[i].x, G.obs[i].y+G.obs[i].h, G.obs[i].w, ch-(G.obs[i].y+G.obs[i].h), (vec4){1, 0, 0.4, 1});
        }
    }

    // Particles
    for(int i=0; i<G.particle_count; i++) starlight_ui_draw_rect(app, G.particles[i].x, G.particles[i].y, G.particles[i].size, G.particles[i].size, G.particles[i].color);

    // Hud
    char b[64]; snprintf(b, sizeof(b), "SCORE: %d", G.score); starlight_ui_draw_text(app, g_font, b, 20, 20, (vec4){1,1,1,1});

    if(G.state == ST_MENU) {
        starlight_ui_draw_rect(app, 0, 0, cw, ch, (vec4){0,0,0,0.6});
        starlight_ui_draw_text(app, g_font_big, "VOID RUNNER", cw/2-150, ch/2-100, (vec4){0,1,1,1});
        starlight_ui_draw_text(app, g_font, "PRESS SPACE TO JUMP", cw/2-140, ch/2+20, (vec4){1,1,1,0.8});
    } else if(G.state == ST_GAMEOVER) {
        starlight_ui_draw_text(app, g_font_big, "GAME OVER", cw/2-130, ch/2, (vec4){1,0,0,1});
    }
}

int main(int argc, char* argv[]) {
    SLF_Config config = { .on_start=Runner_OnStart, .on_update=Runner_OnUpdate, .on_ui=Runner_OnUI };
    return starlight_framework_init("Void Runner 2D", 1280, 720, &config);
}
