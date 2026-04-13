#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace titan {

    struct ClothParticle {
        glm::vec3 position;
        glm::vec3 oldPosition;
        glm::vec3 acceleration;
        bool pinned = false;
    };

    struct ClothConstraint {
        int p1, p2;
        float restLength;
    };

    struct ClothComponent {
        std::vector<ClothParticle> particles;
        std::vector<ClothConstraint> constraints;
        int width, height;
    };

    class ClothSystem {
    public:
        static void Update(entt::registry& registry, float dt);
    };

}
