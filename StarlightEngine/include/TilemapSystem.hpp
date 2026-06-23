#pragma once
#include "EngineSystem.hpp"

namespace starlight {

    class TilemapSystem : public ISystem {
    public:
        bool OnInitialize(const EngineContext& context) override;
        void OnShutdown() override;
        void OnRender() override;
        const char* GetName() const override { return "TilemapSystem"; }
    };

}
