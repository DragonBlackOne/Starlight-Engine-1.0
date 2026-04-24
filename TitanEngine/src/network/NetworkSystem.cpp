// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "NetworkSystem.hpp"
#include "Log.hpp"
#include <iostream>

namespace titan {

    NetworkSystem::NetworkSystem() {
        for (int i = 0; i < MAX_SNAPSHOTS; ++i) m_history[i].tickId = 0;
    }

    NetworkSystem::~NetworkSystem() {
        Shutdown();
    }

    bool NetworkSystem::Initialize() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            Log::Error("NetworkSystem: Winsock initialization failed!");
            return false;
        }
        Log::Info("NetworkSystem: Winsock initialized.");
        return true;
    }

    bool NetworkSystem::CreateSocket(const std::string& ip, int port, bool isServer) {
        m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_socket == INVALID_SOCKET) {
            Log::Error("NetworkSystem: Socket creation failed!");
            return false;
        }

        m_isServer = isServer;
        m_addr.sin_family = AF_INET;
        m_addr.sin_port = htons((unsigned short)port);
        
        if (isServer) {
            m_addr.sin_addr.s_addr = INADDR_ANY;
            if (bind(m_socket, (struct sockaddr*)&m_addr, sizeof(m_addr)) == SOCKET_ERROR) {
                Log::Error("NetworkSystem: Bind failed!");
                return false;
            }
            Log::Info("NetworkSystem: Server listening on UDP port {}.", port);
        } else {
            inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr);
            Log::Info("NetworkSystem: Client configured for remote {}:{}.", ip, port);
        }

        // Set to Non-Blocking
        u_long mode = 1;
        ioctlsocket(m_socket, FIONBIO, &mode);

        return true;
    }

    int NetworkSystem::Send(const void* data, int size) {
        if (m_socket == INVALID_SOCKET) return -1;
        return sendto(m_socket, (const char*)data, size, 0, (struct sockaddr*)&m_addr, sizeof(m_addr));
    }

    int NetworkSystem::Recv(void* data, int size) {
        if (m_socket == INVALID_SOCKET) return -1;
        struct sockaddr_in from;
        int fromLen = sizeof(from);
        return recvfrom(m_socket, (char*)data, size, 0, (struct sockaddr*)&from, &fromLen);
    }

    void NetworkSystem::Shutdown() {
        if (m_socket != INVALID_SOCKET) {
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
        }
        WSACleanup();
        Log::Info("NetworkSystem: Shut down.");
    }

    void NetworkSystem::PushSnapshot(const NetSnapshot& snap) {
        m_history[snap.tickId % MAX_SNAPSHOTS] = snap;
        if (snap.tickId > m_latestTick || m_latestTick == 0) {
            m_latestTick = snap.tickId;
        }
    }

    bool NetworkSystem::GetInterpolatedState(uint32_t targetTick, NetSnapshot& outA, NetSnapshot& outB, float& outAlpha) {
        uint32_t aTick = targetTick;
        uint32_t bTick = targetTick + 1;

        outA = m_history[aTick % MAX_SNAPSHOTS];
        outB = m_history[bTick % MAX_SNAPSHOTS];

        if (outA.tickId != aTick || outB.tickId != bTick) {
            return false; // Gap in history
        }

        outAlpha = 0.5f; // Static interpolation for now
        return true;
    }

}
