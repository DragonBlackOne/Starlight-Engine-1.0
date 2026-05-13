// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include "CoreMinimal.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include "EngineSystem.hpp"

namespace starlight {

    struct NetworkSnapshot {
        uint32_t tick;
        glm::vec3 position;
        glm::quat rotation;
    };

    class NetworkSystem : public ISystem {
    public:
        // ISystem
        bool OnInitialize(const EngineContext& context) override;
        void OnShutdown() override;
        void OnUpdate(float dt) override { (void)dt; }
        const char* GetName() const override { return "NetworkSystem"; }

        bool StartServer(int port);
        bool Connect(const std::string& ip, int port);

        void SendState(const NetworkSnapshot& snap);
        bool ReceiveState(NetworkSnapshot& outSnap);

        // Client-Side Interpolation
        static NetworkSnapshot LerpSnapshots(const NetworkSnapshot& a, const NetworkSnapshot& b, float alpha);

    private:
#ifdef _WIN32
        SOCKET m_socket = INVALID_SOCKET;
#else
        int m_socket = -1;
#endif
        sockaddr_in m_remoteAddr;
        bool m_isServer = false;
        bool m_initialized = false;
    };

}
