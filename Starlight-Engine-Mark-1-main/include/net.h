// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef NET_H
#define NET_H

#include <stdbool.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

typedef struct {
    int socket;
    struct sockaddr_in addr;
} NetSocket;

bool net_init(void);
bool net_socket_create(NetSocket* net, const char* ip, int port, bool is_server);
int  net_send(NetSocket* net, void* data, int size);
int  net_recv(NetSocket* net, void* data, int size);
void net_shutdown(void);

// ==========================================
// CLIENT-SIDE PREDICTION E SNAPSHOT BUFFER
// ==========================================
#define NET_MAX_SNAPSHOTS 64

typedef struct {
    unsigned int tick_id;
    float position[3];
    float velocity[3];
    float pitch, yaw;
} NetSnapshot;

typedef struct {
    NetSnapshot buffer[NET_MAX_SNAPSHOTS];
    unsigned int latest_tick;
    float predicted_error[3]; // Client-Side Prediction offset
} NetStateBuffer;

void net_state_buffer_init(NetStateBuffer* buf);
void net_state_push(NetStateBuffer* buf, NetSnapshot snap);
bool net_state_get_lerp(NetStateBuffer* buf, unsigned int target_tick, NetSnapshot* out_a, NetSnapshot* out_b, float* out_alpha);

// ==========================================
// SPATIAL VOIP PROTOCOL (PROXIMITY CHAT)
// ==========================================
#define VOIP_PACKET_SIZE 1024

typedef struct {
    unsigned int client_id;
    float position[3]; // Posicao onde o audio foi falado (para HRTF/Occlusion)
    unsigned char audio_data[VOIP_PACKET_SIZE]; 
} VoIPPacket;

#endif
