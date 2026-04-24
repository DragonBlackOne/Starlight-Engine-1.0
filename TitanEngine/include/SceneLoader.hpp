// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <string>
#include <entt/entt.hpp>

namespace titan {

    class SceneLoader {
    public:
        // Loads a scene from a JSON file and populates the registry
        static bool Load(const std::string& filepath, entt::registry& registry);
        
        // Saves the current registry state to a JSON file
        static bool Save(const std::string& filepath, entt::registry& registry);
    };

}
