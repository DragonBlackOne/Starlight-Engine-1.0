#pragma once
#include "EngineSystem.hpp"
#include "Components.hpp"

namespace starlight {

    class SpriteRendererSystem : public ISystem {
    public:
        bool OnInitialize(const EngineContext& context) override;
        void OnShutdown() override;
        void OnRender() override;
        const char* GetName() const override { return "SpriteRendererSystem"; }
    };

}
