// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "camera.h"
#include <math.h>

void camera_init(Camera* camera, vec3 position, vec3 up, float yaw, float pitch) {
    glm_vec3_copy(position, camera->position);
    glm_vec3_copy(up, camera->worldUp);
    camera->yaw = yaw; camera->pitch = pitch;
    camera->movementSpeed = 5.0f; camera->mouseSensitivity = 0.1f;
    camera->zoom = 45.0f;
    camera_process_mouse_movement(camera, 0, 0);
}

void camera_get_view_matrix(Camera* camera, mat4 dest) {
    vec3 center; glm_vec3_add(camera->position, camera->front, center);
    glm_lookat(camera->position, center, camera->up, dest);
}

void camera_extract_frustum(Camera* camera, mat4 m) {
    camera->frustum.planes[0][0] = m[0][3] + m[0][0];
    camera->frustum.planes[0][1] = m[1][3] + m[1][0];
    camera->frustum.planes[0][2] = m[2][3] + m[2][0];
    camera->frustum.planes[0][3] = m[3][3] + m[3][0];
    camera->frustum.planes[1][0] = m[0][3] - m[0][0];
    camera->frustum.planes[1][1] = m[1][3] - m[1][0];
    camera->frustum.planes[1][2] = m[2][3] - m[2][0];
    camera->frustum.planes[1][3] = m[3][3] - m[3][0];
    camera->frustum.planes[2][0] = m[0][3] + m[0][1];
    camera->frustum.planes[2][1] = m[1][3] + m[1][1];
    camera->frustum.planes[2][2] = m[2][3] + m[2][1];
    camera->frustum.planes[2][3] = m[3][3] + m[3][1];
    camera->frustum.planes[3][0] = m[0][3] - m[0][1];
    camera->frustum.planes[3][1] = m[1][3] - m[1][1];
    camera->frustum.planes[3][2] = m[2][3] - m[2][1];
    camera->frustum.planes[3][3] = m[3][3] - m[3][1];
    camera->frustum.planes[4][0] = m[0][3] + m[0][2];
    camera->frustum.planes[4][1] = m[1][3] + m[1][2];
    camera->frustum.planes[4][2] = m[2][3] + m[2][2];
    camera->frustum.planes[4][3] = m[3][3] + m[3][2];
    camera->frustum.planes[5][0] = m[0][3] - m[0][2];
    camera->frustum.planes[5][1] = m[1][3] - m[1][2];
    camera->frustum.planes[5][2] = m[2][3] - m[2][2];
    camera->frustum.planes[5][3] = m[3][3] - m[3][2];
    for (int i = 0; i < 6; i++) {
        float len = glm_vec3_norm(camera->frustum.planes[i]);
        glm_vec4_scale(camera->frustum.planes[i], 1.0f / len, camera->frustum.planes[i]);
    }
}

bool camera_check_aabb(Frustum* frustum, const AABB* aabb) {
    for (int i = 0; i < 6; i++) {
        vec3 p;
        p[0] = (frustum->planes[i][0] > 0) ? aabb->max[0] : aabb->min[0];
        p[1] = (frustum->planes[i][1] > 0) ? aabb->max[1] : aabb->min[1];
        p[2] = (frustum->planes[i][2] > 0) ? aabb->max[2] : aabb->min[2];
        if (glm_vec3_dot(frustum->planes[i], p) + frustum->planes[i][3] < 0) return false;
    }
    return true;
}

void camera_process_keyboard(Camera* camera, const Uint8* kb, float dt) {
    float vel = camera->movementSpeed * dt;
    vec3 move;
    if (kb[SDL_SCANCODE_W]) { glm_vec3_scale(camera->front, vel, move); glm_vec3_add(camera->position, move, camera->position); }
    if (kb[SDL_SCANCODE_S]) { glm_vec3_scale(camera->front, vel, move); glm_vec3_sub(camera->position, move, camera->position); }
    if (kb[SDL_SCANCODE_A]) { glm_vec3_scale(camera->right, vel, move); glm_vec3_sub(camera->position, move, camera->position); }
    if (kb[SDL_SCANCODE_D]) { glm_vec3_scale(camera->right, vel, move); glm_vec3_add(camera->position, move, camera->position); }
}

void camera_process_mouse_movement(Camera* camera, float x, float y) {
    camera->yaw += x * camera->mouseSensitivity;
    camera->pitch += y * camera->mouseSensitivity;
    if (camera->pitch > 89.0f) camera->pitch = 89.0f;
    if (camera->pitch < -89.0f) camera->pitch = -89.0f;
    vec3 f;
    f[0] = cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
    f[1] = sinf(glm_rad(camera->pitch));
    f[2] = sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
    glm_vec3_normalize_to(f, camera->front);
    glm_vec3_crossn(camera->front, camera->worldUp, camera->right);
    glm_vec3_crossn(camera->right, camera->front, camera->up);
}

void camera_handle_resize(Camera* camera, int w, int h) {}
