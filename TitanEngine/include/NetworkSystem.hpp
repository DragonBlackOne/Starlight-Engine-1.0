// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")

namespace titan {

    struct NetSnapshot {
        uint32_t tickId;
        glm::vec3 position;
        glm::quat rotation;
    };

    class NetworkSystem {
    public:
        NetworkSystem();
        ~NetworkSystem();

        bool Initialize();
        bool CreateSocket(const std::string& ip, int port, bool isServer);
        
        int Send(const void* data, int size);
        int Recv(void* data, int size);
        
        void Shutdown();

        // State interpolation helpers
        void PushSnapshot(const NetSnapshot& snap);
        bool GetInterpolatedState(uint32_t targetTick, NetSnapshot& outA, NetSnapshot& outB, float& outAlpha);

    private:
        SOCKET m_socket = INVALID_SOCKET;
        sockaddr_in m_addr;
        bool m_isServer = false;

        static constexpr int MAX_SNAPSHOTS = 64;
        NetSnapshot m_history[MAX_SNAPSHOTS];
        uint32_t m_latestTick = 0;
    };

}
