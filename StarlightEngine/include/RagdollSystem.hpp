#pragma once
#include <vector>
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include "EngineSystem.hpp"

namespace starlight {

    struct RagdollBone {
        int jointIndex = -1;
        JPH::BodyID bodyID;
        float length = 0.5f;
        float radius = 0.15f;
    };

    struct RagdollComponent {
        std::vector<RagdollBone> bones;
        bool active = false;
        float blendWeight = 1.0f; // 1.0f = full physics, 0.0f = full animation
    };

    class RagdollSystem : public ISystem {
    public:
        RagdollSystem() = default;
        ~RagdollSystem() = default;

        // ISystem
        bool OnInitialize(const EngineContext& context) override { (void)context; return true; }
        void OnShutdown() override {}
        void OnUpdate(float dt) override;
        const char* GetName() const override { return "RagdollSystem"; }
        bool IsMainThreadOnly() const override { return true; }

        static void Update(entt::registry& registry, float dt);
    };

}
