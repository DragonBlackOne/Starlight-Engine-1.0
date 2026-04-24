// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "PickerSystem.hpp"
#include "Components.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace titan {

    Ray PickerSystem::ScreenPointToRay(int mouseX, int mouseY, int screenWidth, int screenHeight, 
                                     const glm::mat4& view, const glm::mat4& projection) {
        float x = (2.0f * mouseX) / screenWidth - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / screenHeight;
        
        glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
        glm::vec4 rayView = glm::inverse(projection) * rayClip;
        rayView = glm::vec4(rayView.x, rayView.y, -1.0f, 0.0f);
        
        glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayView);
        rayWorld = glm::normalize(rayWorld);

        glm::vec4 camPos = glm::inverse(view) * glm::vec4(0, 0, 0, 1);
        
        return { glm::vec3(camPos), rayWorld };
    }

    bool PickerSystem::IntersectsAABB(const Ray& ray, const glm::vec3& min, const glm::vec3& max, float& distance) {
        float t1 = (min.x - ray.origin.x) / ray.direction.x;
        float t2 = (max.x - ray.origin.x) / ray.direction.x;
        float t3 = (min.y - ray.origin.y) / ray.direction.y;
        float t4 = (max.y - ray.origin.y) / ray.direction.y;
        float t5 = (min.z - ray.origin.z) / ray.direction.z;
        float t6 = (max.z - ray.origin.z) / ray.direction.z;

        float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
        float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

        if (tmax < 0 || tmin > tmax) return false;

        distance = tmin;
        return true;
    }

    entt::entity PickerSystem::Pick(const Ray& ray, entt::registry& registry, OctreeSystem& octree) {
        entt::entity closest = entt::null;
        float minDistance = 10000.0f;

        auto view = registry.view<TransformComponent>();
        for (auto entity : view) {
            auto& t = view.get<TransformComponent>(entity);
            // Default AABB size based on scale
            glm::vec3 min = t.position - t.scale * 0.5f;
            glm::vec3 max = t.position + t.scale * 0.5f;

            float dist;
            if (IntersectsAABB(ray, min, max, dist)) {
                if (dist < minDistance) {
                    minDistance = dist;
                    closest = entity;
                }
            }
        }

        return closest;
    }

}
