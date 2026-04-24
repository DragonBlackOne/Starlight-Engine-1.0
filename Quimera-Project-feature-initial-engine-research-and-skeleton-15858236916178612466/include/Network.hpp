// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - Network Module (ENet)
// Licença: MIT | Origem: Lee Salzman
// ============================================================

#include <enet/enet.h>
#include <string>
#include <functional>
#include <vector>
#include "Log.hpp"

namespace Vamos {
    class NetworkSystem {
    public:
        using OnConnectCallback = std::function<void(uint32_t peerId)>;
        using OnDisconnectCallback = std::function<void(uint32_t peerId)>;
        using OnReceiveCallback = std::function<void(uint32_t peerId, const uint8_t* data, size_t length, uint8_t channel)>;

        NetworkSystem() {
            if (enet_initialize() != 0) {
                VAMOS_CORE_ERROR("Network: Falha ao inicializar ENet!");
                return;
            }
            initialized = true;
            VAMOS_CORE_INFO("Network: ENet inicializado");
        }

        ~NetworkSystem() {
            Stop();
            if (initialized) enet_deinitialize();
        }

        bool StartServer(uint16_t port, size_t maxClients = 32, size_t channels = 2) {
            ENetAddress address;
            address.host = ENET_HOST_ANY;
            address.port = port;
            host = enet_host_create(&address, maxClients, channels, 0, 0);
            if (!host) {
                VAMOS_CORE_ERROR("Network: Falha ao criar servidor na porta {0}", port);
                return false;
            }
            isServer = true;
            VAMOS_CORE_INFO("Network: Servidor iniciado na porta {0}", port);
            return true;
        }

        bool Connect(const std::string& hostname, uint16_t port, size_t channels = 2) {
            host = enet_host_create(nullptr, 1, channels, 0, 0);
            if (!host) return false;

            ENetAddress address;
            enet_address_set_host(&address, hostname.c_str());
            address.port = port;

            peer = enet_host_connect(host, &address, channels, 0);
            if (!peer) {
                VAMOS_CORE_ERROR("Network: Falha ao conectar em {0}:{1}", hostname, port);
                return false;
            }
            VAMOS_CORE_INFO("Network: Conectando a {0}:{1}...", hostname, port);
            return true;
        }

        void Poll(uint32_t timeoutMs = 0) {
            if (!host) return;
            ENetEvent event;
            while (enet_host_service(host, &event, timeoutMs) > 0) {
                switch (event.type) {
                    case ENET_EVENT_TYPE_CONNECT:
                        if (onConnect) onConnect(event.peer->connectID);
                        break;
                    case ENET_EVENT_TYPE_DISCONNECT:
                        if (onDisconnect) onDisconnect(event.peer->connectID);
                        break;
                    case ENET_EVENT_TYPE_RECEIVE:
                        if (onReceive) onReceive(event.peer->connectID,
                            event.packet->data, event.packet->dataLength, event.channelID);
                        enet_packet_destroy(event.packet);
                        break;
                    default: break;
                }
            }
        }

        void SendReliable(const void* data, size_t length, uint8_t channel = 0) {
            ENetPacket* packet = enet_packet_create(data, length, ENET_PACKET_FLAG_RELIABLE);
            if (isServer) {
                enet_host_broadcast(host, channel, packet);
            } else if (peer) {
                enet_peer_send(peer, channel, packet);
            }
        }

        void SendUnreliable(const void* data, size_t length, uint8_t channel = 1) {
            ENetPacket* packet = enet_packet_create(data, length, 0);
            if (isServer) {
                enet_host_broadcast(host, channel, packet);
            } else if (peer) {
                enet_peer_send(peer, channel, packet);
            }
        }

        void Stop() {
            if (peer) { enet_peer_disconnect(peer, 0); peer = nullptr; }
            if (host) { enet_host_destroy(host); host = nullptr; }
        }

        OnConnectCallback onConnect;
        OnDisconnectCallback onDisconnect;
        OnReceiveCallback onReceive;

    private:
        ENetHost* host = nullptr;
        ENetPeer* peer = nullptr;
        bool initialized = false;
        bool isServer = false;
    };
}
