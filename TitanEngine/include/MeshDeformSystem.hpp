// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace titan {

    struct MeshDeformComponent {
        std::vector<glm::vec3> originalVertices;
        std::vector<glm::vec3> currentOffsets;
        float strength = 1.0f;
    };

    class MeshDeformSystem {
    public:
        static void ApplyDamage(entt::registry& registry, entt::entity entity, const glm::vec3& contactPoint, float impulse);
        static void Update(entt::registry& registry, float dt);
    };

}
