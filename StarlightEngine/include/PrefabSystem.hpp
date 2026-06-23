#pragma once
#include <string>
#include <entt/entt.hpp>

namespace starlight {
    class PrefabSystem {
    public:
        static entt::entity Instantiate(const std::string& prefabVfsPath, entt::registry* customRegistry = nullptr);
        static bool Save(const std::string& prefabVfsPath, entt::entity entity, entt::registry* registry);
    };
}
