// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace starlight {

    class HierarchySystem {
    public:
        static void Update(entt::registry& registry);

    private:
        static void UpdateRecursive(entt::registry& registry, entt::entity entity, const glm::mat4& parentMatrix);
    };

}
