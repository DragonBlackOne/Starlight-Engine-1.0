#pragma once
#include "EngineSystem.hpp"

namespace starlight {

    struct Terrain {
        uint32_t vao = 0, vbo = 0, ebo = 0;
        uint32_t indexCount = 0;
    };

    class TerrainSystem : public ISystem {
    public:
        // ISystem
        bool OnInitialize(const EngineContext& context) override { (void)context; return true; }
        void OnShutdown() override {}
        void OnUpdate(float dt) override { (void)dt; }
        const char* GetName() const override { return "TerrainSystem"; }

        Terrain CreateProcedural(int width, int depth, float scale);
        void Render(const Terrain& terrain, uint32_t shader);
        void Destroy(Terrain& terrain);

    private:
        float SimplexNoise(float x, float y); // Implementation stub or link to noise lib
    };

}
