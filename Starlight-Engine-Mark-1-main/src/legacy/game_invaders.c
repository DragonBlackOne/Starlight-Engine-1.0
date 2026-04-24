// Este projeto é feito por IA e só o prompt é feito por um humano.
/**
 * STARLIGHT INVADERS 2D
 * =====================
 * Neon Arcade Wave Shooter.
 * Unlocks Game ID 1 Slots in Gallery.
 */
#include "starlight.h"
#include "save_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_PROJECTILES 32
#define MAX_ENEMIES     40
#define MAX_PARTICLES   150
#define ENEMY_COLS      8
#define ENEMY_ROWS      4

typedef enum { ST_MENU, ST_PLAY, ST_WIN, ST_GAMEOVER } GameState;

typedef struct { float x, y, w, h; int active; } Entity;
typedef struct { float x, y, vx, vy, life, max_life, size; vec4 color; } Particle;

typedef struct {
    GameState state;
    Entity player;
    Entity p_bullets[MAX_PROJECTILES];
    Entity e_bullets[MAX_PROJECTILES];
    Entity enemies[MAX_ENEMIES];
    int enemy_count;
    float enemy_dir;
    float enemy_speed;
    float enemy_move_timer;
    float fire_timer;

    int score, lives;
    int wave;
    
    // VFX
    Particle particles[MAX_PARTICLES];
    int particle_count;
    float shake_x, shake_y, shake_timer;
    
    // Audio
    AudioSource* sfx_shoot;
    AudioSource* sfx_exp;
    AudioSource* sfx_hit;

    float total_time;
} InvadersGame;

static InvadersGame G;
static SLF_Font *g_font, *g_font_big;

static void emit(float x, float y, vec4 col, int count) {
    for (int i = 0; i < count && G.particle_count < MAX_PARTICLES; i++) {
        Particle* p = &G.particles[G.particle_count++];
        p->x = x; p->y = y;
        float a = ((rand()%360)*3.14f)/180.0f; float s = 80.0f + rand()%120;
        p->vx = cosf(a)*s; p->vy = sinf(a)*s;
        p->life = 0.4f + (rand()%40)/100.0f; p->max_life = p->life;
        p->size = 2.0f + rand()%4; glm_vec4_copy(col, p->color);
    }
}

static void spawn_wave() {
    G.enemy_count = 0;
    for(int r=0; r<ENEMY_ROWS; r++) {
        for(int c=0; c<ENEMY_COLS; c++) {
            Entity* e = &G.enemies[G.enemy_count++];
            e->x = 150 + c * 100; e->y = 100 + r * 60;
            e->w = 40; e->h = 30; e->active = 1;
        }
    }
    G.enemy_dir = 1.0f; G.enemy_speed = 40.0f + G.wave * 15.0f;
}

static void Invaders_OnStart(SLF_App* app) {
    save_system_init();
    memset(&G, 0, sizeof(G));
    G.state = ST_MENU; G.lives = 3; G.wave = 1;
    G.player = (Entity){ (float)app->width/2-25, (float)app->height-80, 50, 30, 1 };
    
    g_font = starlight_load_font("assets/fonts/Inconsolata-Regular.ttf", 32.0f);
    g_font_big = starlight_load_font("assets/fonts/Inconsolata-Regular.ttf", 64.0f);
    
    G.sfx_shoot = audio_create_source("assets/audio/ball_hit_1.mp3");
    G.sfx_exp = audio_create_source("assets/audio/score_point_1.mp3");
    G.sfx_hit = audio_create_source("assets/audio/ball_hit_1.mp3");

    starlight_bind_action(app, "Left", SDL_SCANCODE_A, -1);
    starlight_bind_action(app, "Right", SDL_SCANCODE_D, -1);
    starlight_bind_action(app, "Fire", SDL_SCANCODE_SPACE, -1);
    starlight_bind_action(app, "Ok", SDL_SCANCODE_RETURN, -1);
    
    spawn_wave();
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

static void Invaders_OnUpdate(SLF_App* app) {
    float dt = app->delta_time; if(dt > 0.05) dt = 0.05f;
    G.total_time += dt;

    if(G.shake_timer > 0) {
        G.shake_timer -= dt;
        G.shake_x = (rand()%100-50)/50.0f * G.shake_timer * 10;
        G.shake_y = (rand()%100-50)/50.0f * G.shake_timer * 10;
    } else { G.shake_x = G.shake_y = 0; }

    for(int i=0; i<G.particle_count; i++) {
        G.particles[i].life -= dt;
        if(G.particles[i].life <= 0) { G.particles[i] = G.particles[--G.particle_count]; i--; continue; }
        G.particles[i].x += G.particles[i].vx * dt; G.particles[i].y += G.particles[i].vy * dt;
        G.particles[i].color[3] = G.particles[i].life/G.particles[i].max_life;
    }

    if(G.state == ST_MENU) {
        if(starlight_is_action_just_pressed(app, "Ok")) G.state = ST_PLAY;
        return;
    }
    if(G.state == ST_PLAY) {
        // Player
        if(app->keys[SDL_SCANCODE_A] || app->keys[SDL_SCANCODE_LEFT]) G.player.x -= 400 * dt;
        if(app->keys[SDL_SCANCODE_D] || app->keys[SDL_SCANCODE_RIGHT]) G.player.x += 400 * dt;
        G.player.x = fmaxf(20, fminf(app->width-70, G.player.x));

        G.fire_timer -= dt;
        if(starlight_is_action_just_pressed(app, "Fire") && G.fire_timer <= 0) {
            for(int i=0; i<MAX_PROJECTILES; i++) {
                if(!G.p_bullets[i].active) {
                    G.p_bullets[i] = (Entity){ G.player.x+G.player.w/2-2, G.player.y-10, 4, 15, 1 };
                    G.fire_timer = 0.4f; if(G.sfx_shoot) audio_source_play(G.sfx_shoot,0,0.4);
                    break;
                }
            }
        }

        // Bullets
        for(int i=0; i<MAX_PROJECTILES; i++) {
            if(G.p_bullets[i].active) {
                G.p_bullets[i].y -= 600 * dt;
                if(G.p_bullets[i].y < 0) G.p_bullets[i].active = 0;
                // Collision with enemies
                for(int j=0; j<G.enemy_count; j++) {
                    Entity* e = &G.enemies[j];
                    if(e->active && G.p_bullets[i].x < e->x+e->w && G.p_bullets[i].x+4 > e->x && G.p_bullets[i].y < e->y+e->h && G.p_bullets[i].y+15 > e->y) {
                        e->active = 0; G.p_bullets[i].active = 0; G.score += 100;
                        emit(e->x+e->w/2, e->y+e->h/2, (vec4){1, 1, 0, 1}, 8);
                        if(G.sfx_exp) audio_source_play(G.sfx_exp,0,0.5);
                        G.shake_timer = 0.15f;
                        break;
                    }
                }
            }
        }

        // Enemies
        int active_count = 0; float min_x = 2000, max_x = -2000;
        for(int i=0; i<G.enemy_count; i++) {
            if(G.enemies[i].active) {
                active_count++;
                if(G.enemies[i].x < min_x) min_x = G.enemies[i].x;
                if(G.enemies[i].x+G.enemies[i].w > max_x) max_x = G.enemies[i].x+G.enemies[i].w;
            }
        }
        if(active_count == 0) {
            G.wave++; spawn_wave();
            save_system_unlock(1, G.wave - 2); // Unlock Slot for Invaders (Game 1)
            return;
        }

        bool hit_wall = (G.enemy_dir > 0 && max_x > app->width-20) || (G.enemy_dir < 0 && min_x < 20);
        if(hit_wall) {
            G.enemy_dir *= -1;
            for(int i=0; i<G.enemy_count; i++) G.enemies[i].y += 30;
        }
        for(int i=0; i<G.enemy_count; i++) {
            if(G.enemies[i].active) {
                G.enemies[i].x += G.enemy_dir * G.enemy_speed * dt;
                if(G.enemies[i].y + G.enemies[i].h > G.player.y) G.state = ST_GAMEOVER;
            }
        }
    }
}

static void Invaders_OnUI(SLF_App* app) {
    glDisable(GL_DEPTH_TEST); glEnable(GL_BLEND);
    float cw = (float)app->width, ch = (float)app->height, ox = G.shake_x, oy = G.shake_y;
    starlight_ui_draw_rect(app, 0, 0, cw, ch, (vec4){0.01, 0, 0.05, 1});

    // Player
    starlight_ui_draw_rect(app, G.player.x+ox, G.player.y+oy, G.player.w, G.player.h, (vec4){0, 1, 1, 1});
    starlight_ui_draw_rect(app, G.player.x+15+ox, G.player.y-10+oy, 20, 10, (vec4){0, 1, 1, 1});

    // Enemies
    for(int i=0; i<G.enemy_count; i++) {
        if(G.enemies[i].active) {
            float g = 0.5f + 0.5f*sinf(G.total_time*5 + i);
            starlight_ui_draw_rect(app, G.enemies[i].x+ox, G.enemies[i].y+oy, G.enemies[i].w, G.enemies[i].h, (vec4){1, 0, 1, 0.6+0.4*g});
            starlight_ui_draw_rect(app, G.enemies[i].x+10+ox, G.enemies[i].y+10+oy, 5, 5, (vec4){1, 1, 1, 1});
            starlight_ui_draw_rect(app, G.enemies[i].x+25+ox, G.enemies[i].y+10+oy, 5, 5, (vec4){1, 1, 1, 1});
        }
    }

    // Bullets
    for(int i=0; i<MAX_PROJECTILES; i++) {
        if(G.p_bullets[i].active) starlight_ui_draw_rect(app, G.p_bullets[i].x+ox, G.p_bullets[i].y+oy, 4, 15, (vec4){1, 1, 0, 1});
    }

    // Particles
    for(int i=0; i<G.particle_count; i++) starlight_ui_draw_rect(app, G.particles[i].x+ox, G.particles[i].y+oy, G.particles[i].size, G.particles[i].size, G.particles[i].color);

    // Hud
    char b[64]; snprintf(b, sizeof(b), "SCORE: %06d", G.score); starlight_ui_draw_text(app, g_font, b, 20, 20, (vec4){1,1,1,1});
    snprintf(b, sizeof(b), "WAVE: %d", G.wave); starlight_ui_draw_text(app, g_font, b, cw-150, 20, (vec4){1,0,1,1});

    if(G.state == ST_MENU) {
        starlight_ui_draw_rect(app, 0, 0, cw, ch, (vec4){0,0,0,0.7});
        starlight_ui_draw_text(app, g_font_big, "STARLIGHT INVADERS", cw/2-250, ch/2-100, (vec4){0,1,1,1});
        starlight_ui_draw_text(app, g_font, "PRESS ENTER TO START", cw/2-150, ch/2+20, (vec4){1,1,1,0.5+0.5*sinf(G.total_time*8)});
    }
}

int main(int argc, char* argv[]) {
    SLF_Config config = { .on_start=Invaders_OnStart, .on_update=Invaders_OnUpdate, .on_ui=Invaders_OnUI };
    return starlight_framework_init("Starlight Invaders 2D", 1280, 720, &config);
}
