// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#pragma warning(push, 0)
#include <codeanalysis/warnings.h>
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#pragma warning(pop)

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
