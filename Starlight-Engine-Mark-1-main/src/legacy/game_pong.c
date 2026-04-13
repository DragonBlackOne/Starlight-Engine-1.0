#include "starlight.h"
#include <stdio.h>
#include <stdlib.h>

// --- Game Constants ---
#define PADDLE_WIDTH 20.0f
#define PADDLE_HEIGHT 100.0f
#define BALL_SIZE 15.0f
#define PADDLE_SPEED 400.0f
#define INITIAL_BALL_SPEED 300.0f

typedef struct {
    float x, y;
    float w, h;
    float speed_y;
} Paddle;

typedef struct {
    float x, y;
    float vx, vy;
} Ball;

typedef struct {
    Paddle left;
    Paddle right;
    Ball ball;
    int score_l;
    int score_r;
} PongState;

static PongState g_pong;

void reset_ball() {
    g_pong.ball.x = 1280.0f / 2.0f;
    g_pong.ball.y = 720.0f / 2.0f;
    g_pong.ball.vx = (rand() % 2 == 0 ? 1 : -1) * INITIAL_BALL_SPEED;
    g_pong.ball.vy = ((float)(rand() % 100) / 50.0f - 1.0f) * INITIAL_BALL_SPEED;
}

void Pong_OnStart(SLF_App* app) {
    g_pong.left.x = 50.0f;
    g_pong.left.y = (720.0f - PADDLE_HEIGHT) / 2.0f;
    
    g_pong.right.x = 1280.0f - 50.0f - PADDLE_WIDTH;
    g_pong.right.y = (720.0f - PADDLE_HEIGHT) / 2.0f;

    reset_ball();
    g_pong.score_l = 0;
    g_pong.score_r = 0;

    starlight_bind_action(app, "L_Up", SDL_SCANCODE_W, -1);
    starlight_bind_action(app, "L_Down", SDL_SCANCODE_S, -1);
    starlight_bind_action(app, "R_Up", SDL_SCANCODE_UP, -1);
    starlight_bind_action(app, "R_Down", SDL_SCANCODE_DOWN, -1);

    printf("[PONG] Game Initialized. Good luck!\n");
}

void Pong_OnUpdate(SLF_App* app) {
    float dt = app->delta_time;

    // Paddle Movement
    if (starlight_is_action_pressed(app, "L_Up")) g_pong.left.y -= PADDLE_SPEED * dt;
    if (starlight_is_action_pressed(app, "L_Down")) g_pong.left.y += PADDLE_SPEED * dt;
    if (starlight_is_action_pressed(app, "R_Up")) g_pong.right.y -= PADDLE_SPEED * dt;
    if (starlight_is_action_pressed(app, "R_Down")) g_pong.right.y += PADDLE_SPEED * dt;

    // Clamp Paddles
    if (g_pong.left.y < 0) g_pong.left.y = 0;
    if (g_pong.left.y > 720 - PADDLE_HEIGHT) g_pong.left.y = 720 - PADDLE_HEIGHT;
    if (g_pong.right.y < 0) g_pong.right.y = 0;
    if (g_pong.right.y > 720 - PADDLE_HEIGHT) g_pong.right.y = 720 - PADDLE_HEIGHT;

    // Ball Movement
    g_pong.ball.x += g_pong.ball.vx * dt;
    g_pong.ball.y += g_pong.ball.vy * dt;

    // Ball Wall Collision
    if (g_pong.ball.y <= 0 || g_pong.ball.y >= 720 - BALL_SIZE) {
        g_pong.ball.vy *= -1.05f; // Slight speed up on bounce
    }

    // Ball Paddle Collision (Square Collision)
    // Left
    if (g_pong.ball.x <= g_pong.left.x + PADDLE_WIDTH && 
        g_pong.ball.y + BALL_SIZE >= g_pong.left.y && 
        g_pong.ball.y <= g_pong.left.y + PADDLE_HEIGHT) {
        g_pong.ball.vx *= -1.1f; 
        g_pong.ball.x = g_pong.left.x + PADDLE_WIDTH + 1.0f;
    }
    // Right
    if (g_pong.ball.x + BALL_SIZE >= g_pong.right.x && 
        g_pong.ball.y + BALL_SIZE >= g_pong.right.y && 
        g_pong.ball.y <= g_pong.right.y + PADDLE_HEIGHT) {
        g_pong.ball.vx *= -1.1f;
        g_pong.ball.x = g_pong.right.x - BALL_SIZE - 1.0f;
    }

    // Goal Detection
    if (g_pong.ball.x < 0) {
        g_pong.score_r++;
        reset_ball();
    }
    if (g_pong.ball.x > 1280) {
        g_pong.score_l++;
        reset_ball();
    }

    if (app->keys[SDL_SCANCODE_ESCAPE]) app->running = false;
}

void Pong_OnRender(SLF_App* app) {
    // Background clear is handled by framework
}

void Pong_OnUI(SLF_App* app) {
    vec4 white = {1.0f, 1.0f, 1.0f, 1.0f};
    vec4 gray = {0.5f, 0.5f, 0.5f, 0.5f};

    // Draw Left Paddle
    starlight_ui_draw_rect(app, g_pong.left.x, g_pong.left.y, PADDLE_WIDTH, PADDLE_HEIGHT, white);
    // Draw Right Paddle
    starlight_ui_draw_rect(app, g_pong.right.x, g_pong.right.y, PADDLE_WIDTH, PADDLE_HEIGHT, white);
    // Draw Ball
    starlight_ui_draw_rect(app, g_pong.ball.x, g_pong.ball.y, BALL_SIZE, BALL_SIZE, white);

    // Draw Score Hint (Lines for now until Text is ready)
    for(int i=0; i < g_pong.score_l; i++) {
        starlight_ui_draw_rect(app, 100.0f + (i*15), 50.0f, 10.0f, 10.0f, white);
    }
    for(int i=0; i < g_pong.score_r; i++) {
        starlight_ui_draw_rect(app, 1280 - 100.0f - (i*15), 50.0f, 10.0f, 10.0f, white);
    }

    // Mid Line
    starlight_ui_draw_rect(app, 638.0f, 0, 4.0f, 720.0f, gray);
}

int main(int argc, char* argv[]) {
    SLF_Config config;
    config.on_start  = Pong_OnStart;
    config.on_update = Pong_OnUpdate;
    config.on_render = Pong_OnRender;
    config.on_ui     = Pong_OnUI;
    
    return starlight_framework_init("Starlight Pong - Mark-C Edition", 1280, 720, &config);
}
