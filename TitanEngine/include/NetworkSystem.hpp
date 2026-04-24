// Este projeto é feito por IA e só o prompt é feito por um humano.
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

namespace titan {

    struct NetworkSnapshot {
        uint32_t tick;
        glm::vec3 position;
        glm::quat rotation;
    };

    class NetworkSystem : public EngineModule {
    public:
        std::string GetName() const override { return "NetworkSystem"; }

        void Initialize() override;
        void Update(float dt) override {}
        void Shutdown() override;

        bool StartServer(int port);
        bool Connect(const std::string& ip, int port);

        void SendState(const NetworkSnapshot& snap);
        bool ReceiveState(NetworkSnapshot& outSnap);

        // Client-Side Interpolation
        static NetworkSnapshot LerpSnapshots(const NetworkSnapshot& a, const NetworkSnapshot& b, float alpha);

    private:
        int m_socket = -1;
        sockaddr_in m_remoteAddr;
        bool m_isServer = false;
        bool m_initialized = false;
    };

}
