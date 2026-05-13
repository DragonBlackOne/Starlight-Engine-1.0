// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <entt/entt.hpp>
#include "Components.hpp"

namespace starlight {
    class CameraSystem {
    public:
        static void Update(entt::registry& registry, float aspectRatio);
    };
}
