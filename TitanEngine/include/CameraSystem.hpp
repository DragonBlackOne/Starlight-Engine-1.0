// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <entt/entt.hpp>
#include "Components.hpp"

namespace titan {
    class CameraSystem {
    public:
        static void Update(entt::registry& registry, float aspectRatio);
    };
}
