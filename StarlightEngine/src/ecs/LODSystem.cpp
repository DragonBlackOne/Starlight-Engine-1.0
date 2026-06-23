#include "LODSystem.hpp"
#include "Components.hpp"
#include "LODComponent.hpp"

namespace starlight {
    void LODSystem::Update(entt::registry& registry) {
        // Find main camera for distance calculation
        glm::vec3 camPos = glm::vec3(0.0f, 2.0f, 5.0f);
        auto camView = registry.view<TransformComponent, CameraComponent>();
        camView.each([&camPos](const auto& transform, const auto& camera) {
            if (camera.primary) {
                camPos = transform.position;
            }
        });

        auto lodView = registry.view<TransformComponent, LODComponent>();
        lodView.each([&camPos](const auto& transform, auto& lod) {
            if (lod.levels.empty()) return;

            float dist = glm::distance(transform.position, camPos);
            
            int selectedLevel = 0;
            for (size_t i = 0; i < lod.levels.size(); i++) {
                if (dist < lod.levels[i].distance) {
                    selectedLevel = (int)i;
                    break;
                }
                selectedLevel = (int)i;
            }
            lod.currentLevel = selectedLevel;
        });
    }
}
