#include "NetworkSystem.hpp"
#include "Log.hpp"
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#ifdef _WIN32
#include <ws2tcpip.h>
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

namespace starlight {

    bool NetworkSystem::OnInitialize(const EngineContext& context) {
        (void)context;
#ifdef _WIN32
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            Log::Error("NetworkSystem: Winsock startup failed.");
            return false;
        }
#endif
        m_initialized = true;
        Log::Info("NetworkSystem: Networking layer active.");
        return true;
    }

    void NetworkSystem::OnShutdown() {
        if (!m_initialized) return;
        if (m_socket != INVALID_SOCKET) {
#ifdef _WIN32
            closesocket(m_socket);
#else
            close(m_socket);
#endif
        }
#ifdef _WIN32
        WSACleanup();
#endif
        Log::Info("NetworkSystem: Networking layer shutdown.");
    }

    bool NetworkSystem::StartServer(int port) {
        m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_socket == INVALID_SOCKET) return false;

        // Set non-blocking
#ifdef _WIN32
        u_long mode = 1;
        ioctlsocket(m_socket, FIONBIO, &mode);
#else
        fcntl(m_socket, F_SETFL, O_NONBLOCK);
#endif

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons((unsigned short)port);
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(m_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            Log::Error("NetworkSystem: Failed to bind to port " + std::to_string(port));
            return false;
        }

        m_isServer = true;
        Log::Info("NetworkSystem: Server started on port " + std::to_string(port));
        return true;
    }

    bool NetworkSystem::Connect(const std::string& ip, int port) {
        m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_socket == INVALID_SOCKET) return false;

        // Set non-blocking
#ifdef _WIN32
        u_long mode = 1;
        ioctlsocket(m_socket, FIONBIO, &mode);
#else
        fcntl(m_socket, F_SETFL, O_NONBLOCK);
#endif

        m_remoteAddr.sin_family = AF_INET;
        m_remoteAddr.sin_port = htons((unsigned short)port);
        inet_pton(AF_INET, ip.c_str(), &m_remoteAddr.sin_addr);

        m_isServer = false;
        Log::Info("NetworkSystem: Connected to " + ip + ":" + std::to_string(port));
        return true;
    }

    void NetworkSystem::SendState(const NetworkSnapshot& snap) {
        if (m_socket == INVALID_SOCKET) return;
        sendto(m_socket, (const char*)&snap, sizeof(NetworkSnapshot), 0, (struct sockaddr*)&m_remoteAddr, sizeof(m_remoteAddr));
    }

    bool NetworkSystem::ReceiveState(NetworkSnapshot& outSnap) {
        if (m_socket == INVALID_SOCKET) return false;
        sockaddr_in from;
#ifdef _WIN32
        int fromLen = sizeof(from);
#else
        socklen_t fromLen = sizeof(from);
#endif
        int bytes = recvfrom(m_socket, (char*)&outSnap, sizeof(NetworkSnapshot), 0, (struct sockaddr*)&from, &fromLen);
        if (bytes > 0) {
            m_remoteAddr = from; // Auto-detect client if server
            return true;
        }
        return false;
    }

    NetworkSnapshot NetworkSystem::LerpSnapshots(const NetworkSnapshot& a, const NetworkSnapshot& b, float alpha) {
        NetworkSnapshot res;
        res.tick = a.tick;
        res.position = glm::mix(a.position, b.position, alpha);
        res.rotation = glm::slerp(a.rotation, b.rotation, alpha);
        return res;
    }
}
