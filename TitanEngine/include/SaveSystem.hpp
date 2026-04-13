#pragma once
#include <string>
#include <entt/entt.hpp>

namespace titan {

    class SaveSystem {
    public:
        static bool SaveWorld(entt::registry& registry, const std::string& filename);
        static bool LoadWorld(entt::registry& registry, const std::string& filename);
    };

}
