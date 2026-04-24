// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "HierarchySystem.hpp"
#include "Components.hpp"

namespace starlight {

    void HierarchySystem::Update(entt::registry& registry) {
        // 1. Update all local matrices first
        auto view = registry.view<TransformComponent>();
        for (auto entity : view) {
            view.get<TransformComponent>(entity).UpdateLocalMatrix();
        }

        // 2. Identify and update Root entities (entities with NO parent)
        for (auto entity : view) {
            bool hasParent = false;
            if (registry.all_of<RelationshipComponent>(entity)) {
                if (registry.get<RelationshipComponent>(entity).parent != entt::null) {
                    hasParent = true;
                }
            }

            if (!hasParent) {
                // This is a root entity
                auto& t = view.get<TransformComponent>(entity);
                t.worldMatrix = t.localMatrix;
                
                // Recursively update children
                UpdateRecursive(registry, entity, t.worldMatrix);
            }
        }
    }

    void HierarchySystem::UpdateRecursive(entt::registry& registry, entt::entity entity, const glm::mat4& parentMatrix) {
        if (!registry.all_of<RelationshipComponent>(entity)) return;

        auto& rel = registry.get<RelationshipComponent>(entity);
        for (auto child : rel.children) {
            if (registry.all_of<TransformComponent>(child)) {
                auto& ct = registry.get<TransformComponent>(child);
                ct.worldMatrix = parentMatrix * ct.localMatrix;
                
                // Tail call for the next level
                UpdateRecursive(registry, child, ct.worldMatrix);
            }
        }
    }

}
