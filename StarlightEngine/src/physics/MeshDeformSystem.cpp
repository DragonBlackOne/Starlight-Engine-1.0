#include <glm/gtx/norm.hpp>
#include "MeshDeformSystem.hpp"
#include "Components.hpp"
#include "JobSystem.hpp"
#include "wicked/core/wiJobSystem.h"

namespace starlight {

    void MeshDeformSystem::ApplyDamage(entt::registry& registry, entt::entity entity, const glm::vec3& contactPoint, float impulse) {
        if (!registry.all_of<MeshDeformComponent>(entity)) return;
        if (!registry.all_of<TransformComponent>(entity)) return;

        auto& deform = registry.get<MeshDeformComponent>(entity);
        auto& t = registry.get<TransformComponent>(entity);

        if (deform.originalVertices.size() != deform.currentOffsets.size()) {
            return;
        }

        glm::vec3 localContact = glm::vec3(glm::inverse(t.worldMatrix) * glm::vec4(contactPoint, 1.0f));
        float radius = impulse * 0.5f;

        uint32_t vertexCount = static_cast<uint32_t>(deform.originalVertices.size());
        if (vertexCount > 128) {
            JobContext ctx;
            JobSystem::Dispatch(ctx, vertexCount, 128, [&deform, localContact, radius, impulse](uint32_t i) {
                float distSq = glm::distance2(deform.originalVertices[i], localContact);
                if (distSq < radius * radius) {
                    float dist = sqrt(distSq);
                    float falloff = (radius > 0.0f) ? (1.0f - (dist / radius)) : 1.0f;

                    float len = glm::length(deform.originalVertices[i]);
                    if (len >= 0.0001f) {
                        glm::vec3 dir = deform.originalVertices[i] / len;
                        deform.currentOffsets[i] -= dir * (falloff * impulse * 0.1f * deform.strength);
                    }
                }
            });
            JobSystem::Wait(ctx);
        } else {
            for (uint32_t i = 0; i < vertexCount; ++i) {
                float distSq = glm::distance2(deform.originalVertices[i], localContact);
                if (distSq < radius * radius) {
                    float dist = sqrt(distSq);
                    float falloff = (radius > 0.0f) ? (1.0f - (dist / radius)) : 1.0f;

                    float len = glm::length(deform.originalVertices[i]);
                    if (len < 0.0001f) continue;
                    glm::vec3 dir = deform.originalVertices[i] / len;
                    deform.currentOffsets[i] -= dir * (falloff * impulse * 0.1f * deform.strength);
                }
            }
        }
    }

    void MeshDeformSystem::Update(entt::registry& registry, float dt) {
        auto view = registry.view<MeshDeformComponent>();
        float factor = std::max(0.0f, 1.0f - dt * 2.0f);
        if (factor == 1.0f) return;

        for (auto entity : view) {
            auto& deform = view.get<MeshDeformComponent>(entity);
            uint32_t count = static_cast<uint32_t>(deform.currentOffsets.size());
            if (count == 0) continue;

            if (factor == 0.0f) {
                std::fill(deform.currentOffsets.begin(), deform.currentOffsets.end(), glm::vec3(0.0f));
            } else if (count > 256) {
                JobContext ctx;
                JobSystem::Dispatch(ctx, count, 256, [&deform, factor](uint32_t i) {
                    deform.currentOffsets[i] *= factor;
                });
                JobSystem::Wait(ctx);
            } else {
                for (auto& offset : deform.currentOffsets) {
                    offset *= factor;
                }
            }
        }
    }

}
