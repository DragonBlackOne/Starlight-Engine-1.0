// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

namespace starlight {

    struct Wheel {
        glm::vec3 attachmentPoint;
        float suspensionLength = 0.5f;
        float radius = 0.4f;
        glm::mat4 transform = glm::mat4(1.0f);
        bool isGrounded = false;
    };

    struct VehicleComponent {
        std::vector<Wheel> wheels;
        float engineForce = 0.0f;
        float steeringAngle = 0.0f;
        JPH::BodyID bodyID;
    };

    class VehicleSystem {
    public:
        static void Update(entt::registry& registry, float dt);
    };

}
