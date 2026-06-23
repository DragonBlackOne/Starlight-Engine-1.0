#pragma once
#include <string>
#include <entt/entt.hpp>
#include <json.hpp>

namespace starlight {

    class Scene;

    class SceneSerializer {
    public:
        static void SaveToFile(const Scene& scene, const std::string& filepath);
        static void LoadFromFile(Scene& scene, const std::string& filepath);
        static void SaveToJson(entt::registry& registry, nlohmann::json& outJson);
        static void LoadFromJson(entt::registry& registry, const nlohmann::json& inJson);
    };
}
