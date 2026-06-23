#pragma once
#include "EngineSystem.hpp"
#include "AnimationComponent.hpp"
#include <entt/entt.hpp>

namespace starlight {

    class AnimationSystem : public ISystem {
    public:
        // ISystem
        bool OnInitialize(const EngineContext& context) override { (void)context; return true; }
        void OnShutdown() override {}
        void OnUpdate(float dt) override { Update(dt); }
        const char* GetName() const override { return "AnimationSystem"; }

        static void UpdateEntity(entt::registry& registry, entt::entity entity, float dt);
        void Update(float dt); // Calls UpdateEntity for all relevant entities

    private:
        entt::registry* m_registry = nullptr;
    };

}
