// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    SDL_Window* window;
    SDL_GLContext gl_context;
    int width;
    int height;
    bool running;
    bool headless;
} Engine;

// Inicializa SDL2, OpenGL e a Janela
bool engine_init(Engine* engine, const char* title, int width, int height, bool headless);

// Processa eventos SDL (quit, ESC, resize)
void engine_poll_events(Engine* engine, SDL_Event* event);

// Limpa recursos e encerra SDL
void starlight_engine_destroy(Engine* engine);

#endif
