// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace starlight {

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
