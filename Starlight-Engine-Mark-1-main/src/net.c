#include "net.h"
#include <stdio.h>

#ifdef _WIN32
static WSADATA wsa_data;
#endif

bool net_init(void) {
#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("[NET] Winsock falhou!\n");
        return false;
    }
#endif
    printf("[NET] Camada de rede inicializada.\n");
    return true;
}

bool net_socket_create(NetSocket* net, const char* ip, int port, bool is_server) {
    net->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (net->socket < 0) return false;

    net->addr.sin_family = AF_INET;
    net->addr.sin_port = htons((unsigned short)port);
    net->addr.sin_addr.s_addr = is_server ? INADDR_ANY : inet_addr(ip);

    if (is_server) {
        if (bind(net->socket, (struct sockaddr*)&net->addr, sizeof(net->addr)) < 0) {
            return false;
        }
        printf("[NET] Servidor ouvindo na porta %d\n", port);
    }

    // Non-blocking mode
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(net->socket, FIONBIO, &mode);
#endif

    return true;
}

int net_send(NetSocket* net, void* data, int size) {
    return sendto(net->socket, (const char*)data, size, 0, (struct sockaddr*)&net->addr, sizeof(net->addr));
}

int net_recv(NetSocket* net, void* data, int size) {
    struct sockaddr_in from;
    int from_len = sizeof(from);
    return recvfrom(net->socket, (char*)data, size, 0, (struct sockaddr*)&from, &from_len);
}

void net_shutdown(void) {
#ifdef _WIN32
    WSACleanup();
#endif
    printf("[NET] Camada de rede encerrada.\n");
}

void net_state_buffer_init(NetStateBuffer* buf) {
    memset(buf, 0, sizeof(NetStateBuffer));
    buf->latest_tick = 0;
}

void net_state_push(NetStateBuffer* buf, NetSnapshot snap) {
    // Array Circular por Modulo
    buf->buffer[snap.tick_id % NET_MAX_SNAPSHOTS] = snap;
    if (snap.tick_id > buf->latest_tick || buf->latest_tick == 0) {
        buf->latest_tick = snap.tick_id;
    }
}

bool net_state_get_lerp(NetStateBuffer* buf, unsigned int target_tick, NetSnapshot* out_a, NetSnapshot* out_b, float* out_alpha) {
    unsigned int a_tick = target_tick;
    unsigned int b_tick = target_tick + 1;

    *out_a = buf->buffer[a_tick % NET_MAX_SNAPSHOTS];
    *out_b = buf->buffer[b_tick % NET_MAX_SNAPSHOTS];

    if (out_a->tick_id != a_tick || out_b->tick_id != b_tick) {
        return false; // Pacote perdido no UDP ou buffer sobrescrito
    }

    *out_alpha = 0.5f; // Interpolacao estática sem sub-tick timing
    return true;
}
