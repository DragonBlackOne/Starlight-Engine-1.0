// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include "CoreMinimal.hpp"
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace starlight {

    class Arcade25DModule : public EngineModule {
    public:
        void Initialize() override;
        void Update(float dt) override;
        void Render() override;
        void RenderUI() override;
        void Shutdown() override;
        std::string GetName() const override { return "2.5D Arcade Suite"; }

    private:
        int m_currentGame = 0; // 0: Menu, 1-10: Games
        float m_totalTime = 0;
        
        // Shared ECS for 2.5D games
        entt::registry m_arcadeRegistry;
        
        void InitGame(int index);
        void UpdateGame(int index, float dt);
        
        // Game Implementations (Internal)
        void UpdateVoxelRush(float dt);
        void UpdateSolarDefender(float dt);
        void UpdateNeonTower(float dt);
        void UpdateTrenchFighter(float dt);
        void UpdateVortexPong(float dt);
        void UpdateBulletStorm(float dt);
        void UpdateCrystalCracker(float dt);
        void UpdateCyberMaze(float dt);
        void UpdateLavaEscape(float dt);
        void UpdateNexusArena(float dt);

        void RenderMenu();
        void SpawnPrimitive(const std::string& tag, glm::vec3 pos, glm::vec3 scale, glm::vec3 color, float metallic = 0.0f, float roughness = 0.5f);
    };

}
