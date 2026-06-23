#pragma once
#include <entt/entt.hpp>
#include "EngineSystem.hpp"

namespace starlight {

    class FootIKSystem : public ISystem {
    public:
        FootIKSystem();
        ~FootIKSystem();

        // ISystem
        bool OnInitialize(const EngineContext& context) override { (void)context; return true; }
        void OnShutdown() override {}
        void OnUpdate(float dt) override;
        const char* GetName() const override { return "FootIKSystem"; }
        bool IsMainThreadOnly() const override { return true; }

        void ResolveIK(entt::registry& reg);
    };

}
