// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include "CoreMinimal.hpp"
#include "AnimationComponent.hpp"
#include <entt/entt.hpp>

namespace starlight {

    class AnimationSystem : public EngineModule {
    public:
        std::string GetName() const override { return "AnimationSystem"; }

        void Initialize() override {}
        void Update(float dt) override {}
        void Shutdown() override {}

        static void UpdateEntity(entt::registry& registry, entt::entity entity, float dt);
        void Update(float dt); // Calls UpdateEntity for all relevant entities

    private:
        entt::registry* m_registry = nullptr;
    };

}
