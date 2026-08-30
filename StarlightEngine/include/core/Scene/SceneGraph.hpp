#pragma once
#include "core/ECS/Entity.hpp"
#include "core/ECS/CoreComponents.hpp"
#include <vector>
#include <algorithm>

namespace starlight::ecs {

class SceneGraph {
public:
    static void SetParent(Entity child, Entity parent) {
        if (!child || child == parent) return;

        auto& childRel = child.GetOrEmplaceComponent<RelationshipComponent>();
        
        // Remove from old parent if any
        if (childRel.parent != entt::null) {
            Entity oldParent(childRel.parent, child.GetRegistry());
            if (oldParent && oldParent.HasComponent<RelationshipComponent>()) {
                auto& oldChildren = oldParent.GetComponent<RelationshipComponent>().children;
                oldChildren.erase(std::remove(oldChildren.begin(), oldChildren.end(), child.GetHandle()), oldChildren.end());
            }
        }

        if (parent) {
            childRel.parent = parent.GetHandle();
            auto& parentRel = parent.GetOrEmplaceComponent<RelationshipComponent>();
            if (std::find(parentRel.children.begin(), parentRel.children.end(), child.GetHandle()) == parentRel.children.end()) {
                parentRel.children.push_back(child.GetHandle());
            }
        } else {
            childRel.parent = entt::null;
        }

        if (child.HasComponent<TransformComponent>()) {
            child.GetComponent<TransformComponent>().isDirty = true;
        }
    }

    static void RemoveParent(Entity child) {
        SetParent(child, Entity());
    }

    static void UpdateTransforms(entt::registry& registry) {
        auto view = registry.view<TransformComponent>();

        // Update all local matrices first
        for (auto entity : view) {
            auto& tc = view.get<TransformComponent>(entity);
            if (tc.isDirty) {
                tc.UpdateLocalMatrix();
            }
        }

        // Recursively update from roots down
        for (auto entity : view) {
            if (registry.all_of<RelationshipComponent>(entity)) {
                const auto& rel = registry.get<RelationshipComponent>(entity);
                if (rel.parent == entt::null) {
                    UpdateNodeTransform(registry, entity, glm::mat4(1.0f));
                }
            } else {
                auto& tc = view.get<TransformComponent>(entity);
                tc.worldMatrix = tc.localMatrix;
                tc.isDirty = false;
            }
        }
    }

private:
    static void UpdateNodeTransform(entt::registry& registry, entt::entity entity, const glm::mat4& parentWorld) {
        if (!registry.valid(entity)) return;

        if (registry.all_of<TransformComponent>(entity)) {
            auto& tc = registry.get<TransformComponent>(entity);
            tc.worldMatrix = parentWorld * tc.localMatrix;
            tc.isDirty = false;

            if (registry.all_of<RelationshipComponent>(entity)) {
                const auto& rel = registry.get<RelationshipComponent>(entity);
                for (auto child : rel.children) {
                    UpdateNodeTransform(registry, child, tc.worldMatrix);
                }
            }
        }
    }
};

} // namespace starlight::ecs
