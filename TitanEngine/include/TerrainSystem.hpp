// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include "CoreMinimal.hpp"
#include <vector>

namespace titan {

    struct Terrain {
        uint32_t vao = 0, vbo = 0, ebo = 0;
        uint32_t indexCount = 0;
    };

    class TerrainSystem : public EngineModule {
    public:
        std::string GetName() const override { return "TerrainSystem"; }

        void Initialize() override {}
        void Update(float dt) override {}
        void Shutdown() override {}

        Terrain CreateProcedural(int width, int depth, float scale);
        void Render(const Terrain& terrain, uint32_t shader);
        void Destroy(Terrain& terrain);

    private:
        float SimplexNoise(float x, float y); // Implementation stub or link to noise lib
    };

}
