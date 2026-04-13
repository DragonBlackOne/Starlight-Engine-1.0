#pragma once
#include <entt/entt.hpp>

namespace titan {

    class AISystem {
    public:
        static void Update(entt::registry& registry, float dt);
    };

}
