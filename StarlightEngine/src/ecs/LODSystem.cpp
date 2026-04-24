// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "LODSystem.hpp"
#include "Components.hpp"
#include "LODComponent.hpp"

namespace starlight {
    void LODSystem::Update(entt::registry& registry) {
        // Find main camera for distance calculation
        glm::vec3 camPos = glm::vec3(0.0f, 2.0f, 5.0f);
        auto camView = registry.view<TransformComponent, CameraComponent>();
        for (auto camEntity : camView) {
            if (camView.get<CameraComponent>(camEntity).primary) {
                camPos = camView.get<TransformComponent>(camEntity).position;
                break;
            }
        }

        auto lodView = registry.view<TransformComponent, LODComponent>();
        for (auto entity : lodView) {
            auto& t = lodView.get<TransformComponent>(entity);
            auto& lod = lodView.get<LODComponent>(entity);
            
            if (lod.levels.empty()) continue;

            float dist = glm::distance(t.position, camPos);
            
            int selectedLevel = 0;
            for (size_t i = 0; i < lod.levels.size(); i++) {
                if (dist < lod.levels[i].distance) {
                    selectedLevel = (int)i;
                    break;
                }
                selectedLevel = (int)i; // fallback to lowest detail if beyond all distances
            }
            
            lod.currentLevel = selectedLevel;
        }
    }
}
