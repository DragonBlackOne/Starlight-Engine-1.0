#ifndef NET_SYNC_H
#define NET_SYNC_H

#include "starlight.h"

typedef struct {
    uint32_t sequence;
    vec3 position;
    vec3 velocity;
    uint8_t input_mask;
} NetPlayerState;

/**
 * @brief Aplica predição local no movimento do jogador baseado no input.
 */
void net_apply_prediction(NetPlayerState* state, uint8_t input, float dt);

/**
 * @brief Reconcilia o estado local com a verdade vinda do servidor.
 */
void net_reconcile(NetPlayerState* local, NetPlayerState* server_truth);

#endif
