#pragma once
#include <entt/entt.hpp>
#include "Components.hpp"

namespace starlight {
    class CameraSystem {
    public:
        static void Update(entt::registry& registry, float aspectRatio);
    };
}
