// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "OctreeSystem.hpp"

namespace titan {

    struct Ray {
        glm::vec3 origin;
        glm::vec3 direction;
    };

    class PickerSystem {
    public:
        // Converts screen mouse coordinates to a world-space Ray
        static Ray ScreenPointToRay(int mouseX, int mouseY, int screenWidth, int screenHeight, 
                                   const glm::mat4& view, const glm::mat4& projection);

        // Picks the closest entity intersected by the ray
        static entt::entity Pick(const Ray& ray, entt::registry& registry, OctreeSystem& octree);
        
        // Simple AABB-Ray intersection
        static bool IntersectsAABB(const Ray& ray, const glm::vec3& min, const glm::vec3& max, float& distance);
    };

}
