// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <entt/entt.hpp>

namespace starlight {

    class AISystem {
    public:
        static void Update(entt::registry& registry, float dt);
    };

}
