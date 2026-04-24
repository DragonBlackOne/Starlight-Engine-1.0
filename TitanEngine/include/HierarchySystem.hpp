// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace titan {

    class HierarchySystem {
    public:
        static void Update(entt::registry& registry);

    private:
        static void UpdateRecursive(entt::registry& registry, entt::entity entity, const glm::mat4& parentMatrix);
    };

}
