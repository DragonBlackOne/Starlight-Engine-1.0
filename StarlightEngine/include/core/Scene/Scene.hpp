#pragma once
#include "core/ECS/Entity.hpp"
#include "core/ECS/CoreComponents.hpp"
#include "SceneGraph.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <random>

namespace starlight::ecs {

class Scene {
public:
    explicit Scene(std::string name = "Untitled Scene") : m_name(std::move(name)) {}
    virtual ~Scene() {
        m_registry.clear();
    }

    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }

    Entity CreateEntity(const std::string& name = "Entity") {
        static std::random_device rd;
        static std::mt19937_64 gen(rd());
        static std::uniform_int_distribution<uint64_t> dis;
        return CreateEntityWithUUID(dis(gen), name);
    }

    Entity CreateEntityWithUUID(uint64_t uuid, const std::string& name = "Entity") {
        Entity entity(m_registry.create(), &m_registry);
        entity.AddComponent<UUIDComponent>(uuid);
        entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<RelationshipComponent>();
        
        m_entityMap[uuid] = entity.GetHandle();
        return entity;
    }

    void DestroyEntity(Entity entity) {
        if (!entity) return;

        // Reparent or destroy children
        if (entity.HasComponent<RelationshipComponent>()) {
            auto& rel = entity.GetComponent<RelationshipComponent>();
            for (auto childHandle : rel.children) {
                Entity child(childHandle, &m_registry);
                if (child) {
                    DestroyEntity(child);
                }
            }
        }

        if (entity.HasComponent<UUIDComponent>()) {
            m_entityMap.erase(entity.GetComponent<UUIDComponent>().uuid);
        }

        SceneGraph::RemoveParent(entity);
        m_registry.destroy(entity.GetHandle());
    }

    Entity GetEntityByUUID(uint64_t uuid) {
        auto it = m_entityMap.find(uuid);
        if (it != m_entityMap.end() && m_registry.valid(it->second)) {
            return Entity(it->second, &m_registry);
        }
        return Entity();
    }

    Entity FindEntityByName(const std::string& name) {
        auto view = m_registry.view<TagComponent>();
        for (auto entity : view) {
            const auto& tag = view.get<TagComponent>(entity);
            if (tag.tag == name) {
                return Entity(entity, &m_registry);
            }
        }
        return Entity();
    }

    void UpdateHierarchy() {
        SceneGraph::UpdateTransforms(m_registry);
    }

    void OnUpdate(float dt) {
        (void)dt;
        UpdateHierarchy();
    }

    entt::registry& GetRegistry() { return m_registry; }
    const entt::registry& GetRegistry() const { return m_registry; }

private:
    std::string m_name;
    entt::registry m_registry;
    std::unordered_map<uint64_t, entt::entity> m_entityMap;
};

} // namespace starlight::ecs
