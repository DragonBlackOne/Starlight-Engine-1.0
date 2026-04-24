// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "net_sync.h"
#include <math.h>

void net_apply_prediction(NetPlayerState* state, uint8_t input, float dt) {
    // Mascaras de input (Ex: 1=W, 2=S, 4=A, 8=D)
    vec3 move = {0, 0, 0};
    if (input & 1) move[2] -= 1.0f;
    if (input & 2) move[2] += 1.0f;
    if (input & 4) move[0] -= 1.0f;
    if (input & 8) move[0] += 1.0f;

    if (glm_vec3_norm(move) > 0) {
        glm_vec3_normalize(move);
        glm_vec3_scale(move, 5.0f * dt, move);
        glm_vec3_add(state->position, move, state->position);
    }
}

void net_reconcile(NetPlayerState* local, NetPlayerState* server) {
    // Se a discrepancia for muito grande, teleportamos (Hard correction)
    float dist = glm_vec3_distance(local->position, server->position);
    if (dist > 0.5f) {
        glm_vec3_copy(server->position, local->position);
        debug_log("[NET] Hard reconciliation applied! Error: %.2f", dist);
    } else {
        // Interpolação suave (Soft correction)
        glm_vec3_lerp(local->position, server->position, 0.1f, local->position);
    }
}
