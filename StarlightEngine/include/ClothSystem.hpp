#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace starlight {

    struct ClothParticle {
        glm::vec3 position{0.0f};
        glm::vec3 oldPosition{0.0f};
        glm::vec3 acceleration{0.0f};
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
