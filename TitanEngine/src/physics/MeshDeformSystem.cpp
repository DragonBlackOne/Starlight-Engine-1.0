#include <glm/gtx/norm.hpp>
#include "MeshDeformSystem.hpp"
#include "Components.hpp"

namespace titan {

    void MeshDeformSystem::ApplyDamage(entt::registry& registry, entt::entity entity, const glm::vec3& contactPoint, float impulse) {
        (void)registry;
        (void)entity;
        (void)contactPoint;
        (void)impulse;

        if (!registry.all_of<MeshDeformComponent>(entity)) return;

        auto& deform = registry.get<MeshDeformComponent>(entity);
        auto& t = registry.get<TransformComponent>(entity);

        // Convert world contact point to local space
        glm::vec3 localContact = glm::vec3(glm::inverse(t.worldMatrix) * glm::vec4(contactPoint, 1.0f));
        float radius = impulse * 0.5f;

        for (size_t i = 0; i < deform.originalVertices.size(); ++i) {
            float distSq = glm::distance2(deform.originalVertices[i], localContact);
            if (distSq < radius * radius) {
                float dist = sqrt(distSq);
                float falloff = 1.0f - (dist / radius);
                
                // Move vertex towards local origin (crushing effect)
                glm::vec3 dir = glm::normalize(deform.originalVertices[i]);
                deform.currentOffsets[i] -= dir * (falloff * impulse * 0.1f);
            }
        }
    }

    void MeshDeformSystem::Update(entt::registry& registry, float dt) {
        (void)registry;
        (void)dt;
        // Here we would update the actual GPU buffers with currentOffsets
        // for each entity...
    }

}
