#pragma once
#include <entt/entt.hpp>
#include <cstdint>
#include <cassert>

namespace starlight::ecs {

/**
 * @brief Lightweight, object-oriented Entity handle wrapping an entt::entity ID and Registry.
 */
class Entity {
public:
    Entity() = default;
    Entity(entt::entity handle, entt::registry* registry)
        : m_handle(handle), m_registry(registry) {}
    Entity(const Entity& other) = default;

    bool IsValid() const {
        return m_registry != nullptr && m_handle != entt::null && m_registry->valid(m_handle);
    }

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args) {
        assert(m_registry && "Entity has no valid registry!");
        assert(!HasComponent<T>() && "Entity already has component!");
        return m_registry->emplace<T>(m_handle, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    T& AddOrReplaceComponent(Args&&... args) {
        assert(m_registry && "Entity has no valid registry!");
        return m_registry->emplace_or_replace<T>(m_handle, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    T& GetOrEmplaceComponent(Args&&... args) {
        assert(m_registry && "Entity has no valid registry!");
        return m_registry->get_or_emplace<T>(m_handle, std::forward<Args>(args)...);
    }

    template<typename T>
    T& GetComponent() {
        assert(m_registry && "Entity has no valid registry!");
        assert(HasComponent<T>() && "Entity does not have component!");
        return m_registry->get<T>(m_handle);
    }

    template<typename T>
    const T& GetComponent() const {
        assert(m_registry && "Entity has no valid registry!");
        assert(HasComponent<T>() && "Entity does not have component!");
        return m_registry->get<T>(m_handle);
    }

    template<typename T>
    bool HasComponent() const {
        assert(m_registry && "Entity has no valid registry!");
        return m_registry->all_of<T>(m_handle);
    }

    template<typename T>
    void RemoveComponent() {
        assert(m_registry && "Entity has no valid registry!");
        assert(HasComponent<T>() && "Entity does not have component!");
        m_registry->remove<T>(m_handle);
    }

    operator bool() const {
        return m_registry != nullptr && m_handle != entt::null && m_registry->valid(m_handle);
    }

    operator entt::entity() const { return m_handle; }
    operator uint32_t() const { return static_cast<uint32_t>(m_handle); }

    entt::entity GetHandle() const { return m_handle; }
    entt::registry* GetRegistry() const { return m_registry; }

    bool operator==(const Entity& other) const {
        return m_handle == other.m_handle && m_registry == other.m_registry;
    }

    bool operator!=(const Entity& other) const {
        return !(*this == other);
    }

private:
    entt::entity m_handle{ entt::null };
    entt::registry* m_registry{ nullptr };
};

} // namespace starlight::ecs
