// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <string>
#include <entt/entt.hpp>

namespace starlight {

    class SaveSystem {
    public:
        static bool SaveWorld(entt::registry& registry, const std::string& filename);
        static bool LoadWorld(entt::registry& registry, const std::string& filename);
    };

}
