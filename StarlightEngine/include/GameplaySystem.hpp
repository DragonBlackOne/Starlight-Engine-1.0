#pragma once
#include "EngineSystem.hpp"
#include <entt/entt.hpp>

namespace starlight {

    class GameplaySystem : public ISystem {
    public:
        GameplaySystem() = default;
        ~GameplaySystem() = default;

        bool OnInitialize(const EngineContext& context) override;
        void OnShutdown() override;
        void OnUpdate(float dt) override;
        
        const char* GetName() const override { return "GameplaySystem"; }

    private:
        Engine* m_engine = nullptr;
    };

}
