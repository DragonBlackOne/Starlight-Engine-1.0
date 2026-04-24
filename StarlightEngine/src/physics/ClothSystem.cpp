// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "ClothSystem.hpp"

namespace starlight {

    void ClothSystem::Update(entt::registry& registry, float dt) {
        auto view = registry.view<ClothComponent>();
        glm::vec3 gravity(0, -9.8f, 0);

        for (auto entity : view) {
            auto& cloth = view.get<ClothComponent>(entity);

            // 1. Verlet Integration
            for (auto& p : cloth.particles) {
                if (p.pinned) continue;
                
                glm::vec3 velocity = p.position - p.oldPosition;
                p.oldPosition = p.position;
                p.position += velocity + (gravity + p.acceleration) * (dt * dt);
                p.acceleration = glm::vec3(0); // Reset for next frame
            }

            // 2. Constraint Satisfaction (Iterate 5 times for stability)
            for (int i = 0; i < 5; i++) {
                for (auto& c : cloth.constraints) {
                    auto& p1 = cloth.particles[c.p1];
                    auto& p2 = cloth.particles[c.p2];

                    glm::vec3 delta = p2.position - p1.position;
                    float dist = glm::length(delta);
                    float diff = (dist - c.restLength) / dist;

                    glm::vec3 correction = delta * 0.5f * diff;

                    if (!p1.pinned) p1.position += correction;
                    if (!p2.pinned) p2.position -= correction;
                }
            }
        }
    }

}
