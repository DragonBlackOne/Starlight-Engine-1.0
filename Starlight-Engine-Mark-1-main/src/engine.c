// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "engine.h"
#include <stdio.h>

bool engine_init(Engine* engine, const char* title, int width, int height, bool headless) {
    engine->width = width;
    engine->height = height;
    engine->running = true;

    if (headless) {
        if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
            printf("[ENGINE-HEADLESS] SDL Error: %s\n", SDL_GetError()); fflush(stdout);
            return false;
        }
        printf("[ENGINE-HEADLESS] Started in Server Mode.\n");
        return true;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("[ENGINE] SDL Error: %s\n", SDL_GetError()); fflush(stdout);
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    engine->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (!engine->window) {
        printf("[ENGINE] Window Error: %s\n", SDL_GetError()); fflush(stdout);
        return false;
    }

    engine->gl_context = SDL_GL_CreateContext(engine->window);
    if (!engine->gl_context) {
        printf("[ENGINE] OpenGL Error: %s\n", SDL_GetError()); fflush(stdout);
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("[ENGINE] GLAD failed!\n"); fflush(stdout);
        return false;
    }

    // VSync + GPU protection
    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    printf("[ENGINE] Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("[ENGINE] Renderer: %s\n", glGetString(GL_RENDERER));
    printf("[ENGINE] Version:  %s\n", glGetString(GL_VERSION));

    return true;
}

void engine_poll_events(Engine* engine, SDL_Event* event) {
    // Process SDL events (quit, ESC, resize)
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) {
            engine->running = false;
        }
        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
            engine->running = false;
        }
        if (event->type == SDL_WINDOWEVENT && (event->window.event == SDL_WINDOWEVENT_RESIZED || event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
            SDL_GL_GetDrawableSize(engine->window, &engine->width, &engine->height);
            glViewport(0, 0, engine->width, engine->height);
        }
    }
}

void starlight_engine_destroy(Engine* engine) {
    SDL_GL_DeleteContext(engine->gl_context);
    SDL_DestroyWindow(engine->window);
    SDL_Quit();
    printf("[ENGINE] Shutdown complete.\n");
}
