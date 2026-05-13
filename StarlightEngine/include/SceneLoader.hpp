// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <string>
#include <entt/entt.hpp>

namespace starlight {

    class SceneLoader {
    public:
        // Loads a scene from a JSON file and populates the registry
        static bool Load(const std::string& filepath, entt::registry& registry);
        
        // Saves the current registry state to a JSON file
        static bool Save(const std::string& filepath, entt::registry& registry);
    };

}
