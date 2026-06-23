#pragma once
#include "EngineSystem.hpp"

namespace starlight {

    class Camera2DSystem : public ISystem {
    public:
        bool OnInitialize(const EngineContext& context) override;
        void OnShutdown() override;
        void OnUpdate(float dt) override;
        const char* GetName() const override { return "Camera2DSystem"; }
    };

}
