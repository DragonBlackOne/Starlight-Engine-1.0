// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    vec3 min;
    vec3 max;
} AABB;

typedef struct {
    vec4 planes[6]; // Top, Bottom, Left, Right, Near, Far
} Frustum;

typedef struct {
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    
    Frustum frustum;
} Camera;

// Inicializa os parametros da Camera
void camera_init(Camera* camera, vec3 position, vec3 up, float yaw, float pitch);

// Retorna a matriz LookAt (View)
void camera_get_view_matrix(Camera* camera, mat4 dest);

// Extrai os planos do frustum a partir da matriz View-Projection
void camera_extract_frustum(Camera* camera, mat4 view_proj);

// Verifica se um AABB esta dentro do Frustum
bool camera_check_aabb(Frustum* frustum, const AABB* aabb);

// Processa input do Teclado (WASD)
void camera_process_keyboard(Camera* camera, const Uint8* keystate, float deltaTime);

// Processa movimento do Mouse (Raw Delta)
void camera_process_mouse_movement(Camera* camera, float xoffset, float yoffset);
void camera_handle_resize(Camera* camera, int width, int height);

#endif
