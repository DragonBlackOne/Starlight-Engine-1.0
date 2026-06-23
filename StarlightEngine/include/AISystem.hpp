#pragma once
#include <entt/entt.hpp>

namespace starlight {

    class AISystem {
    public:
        static void Update(entt::registry& registry, float dt);
    };

}
