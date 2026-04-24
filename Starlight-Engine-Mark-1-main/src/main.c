// Este projeto é feito por IA e só o prompt é feito por um humano.
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <cglm/cglm.h>
#include <flecs.h>
#include <stdio.h>
#include <stdbool.h>

#include "engine.h"
#include "shader.h"
#include "camera.h"
#include "loader.h"
#include "ecs_systems.h"

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    printf("=== Starlight Engine Mark-C ===\n");

    // --- INIT ENGINE (SDL2 + OpenGL) ---
    Engine engine;
    if (!engine_init(&engine, "Starlight Engine Mark-C", 1280, 720)) {
        return -1;
    }

    // --- INIT ECS (Flecs, 6 threads Ryzen) ---
    ecs_world_t *world = ecs_setup(6);

    // --- SHADERS (Blinn-Phong Lighting) ---
    GLuint shaderProgram = shader_load_program("assets/shaders/lighting.vert", "assets/shaders/lighting.frag");

    // --- ASSETS (OBJ + Texture) ---
    GLuint bananaTex = loader_load_texture("assets/textures/banana.png");
    if (bananaTex == 0) return -1;

    MeshRenderer bananaMesh;
    if (!loader_load_obj("assets/models/banana.obj", &bananaMesh)) return -1;

    // --- CAMERA ---
    Camera camera;
    vec3 camPos = {0.0f, 0.0f, 3.0f};
    vec3 camUp  = {0.0f, 1.0f, 0.0f};
    camera_init(&camera, camPos, camUp, -90.0f, 0.0f);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // --- TIMING ---
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;

    printf("[MAIN] Entrando no loop principal...\n");

    while (engine.running) {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());
        float dtSec = (float)deltaTime / 1000.0f;

        // --- INPUT ---
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                engine.running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                engine.running = false;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                engine.width = event.window.data1;
                engine.height = event.window.data2;
                glViewport(0, 0, engine.width, engine.height);
            }
            if (event.type == SDL_MOUSEMOTION) {
                camera_process_mouse_movement(&camera, (float)event.motion.xrel, (float)-event.motion.yrel);
            }
        }

        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        camera_process_keyboard(&camera, keystate, dtSec);

        // --- ECS ---
        ecs_progress(world, dtSec);

        // --- RENDER ---
        glViewport(0, 0, engine.width, engine.height);
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // MVP
        mat4 projection, view, model;
        glm_perspective(glm_rad(camera.zoom), (float)engine.width / (float)engine.height, 0.1f, 100.0f, projection);
        camera_get_view_matrix(&camera, view);
        glm_mat4_identity(model);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, (float*)projection);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, (float*)model);

        // Light (Sol direcional)
        glUniform3f(glGetUniformLocation(shaderProgram, "lightDir"), -0.3f, -1.0f, -0.5f);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 0.95f, 0.85f);
        glUniform1f(glGetUniformLocation(shaderProgram, "lightIntensity"), 1.2f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, camera.position);

        // Material
        glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), 0.15f);
        glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), 0.6f);
        glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), 32.0f);

        // Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bananaTex);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        // Draw
        glBindVertexArray(bananaMesh.vao);
        glDrawArrays(GL_TRIANGLES, 0, bananaMesh.vertex_count);

        SDL_GL_SwapWindow(engine.window);

        // Frame limiter (RX 550 protection)
        Uint64 FRAME_END = SDL_GetPerformanceCounter();
        double frameTimeMs = (double)((FRAME_END - NOW) * 1000 / (double)SDL_GetPerformanceFrequency());
        if (frameTimeMs < 16.666) {
            SDL_Delay((Uint32)(16.666 - frameTimeMs));
        }
    }

    // --- CLEANUP ---
    printf("[MAIN] Encerrando...\n");
    glDeleteVertexArrays(1, &bananaMesh.vao);
    glDeleteBuffers(1, &bananaMesh.vbo);
    glDeleteTextures(1, &bananaTex);
    glDeleteProgram(shaderProgram);
    ecs_fini(world);
    engine_shutdown(&engine);

    return 0;
}
