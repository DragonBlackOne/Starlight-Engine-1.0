// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <string>
#include <entt/entt.hpp>

namespace starlight {

    class Scene;

    class SceneSerializer {
    public:
        static void SaveToFile(const Scene& scene, const std::string& filepath);
        static void LoadFromFile(Scene& scene, const std::string& filepath);

    private:
        static void SerializeEntity(entt::registry& reg, entt::entity entity, class nlohmann_json_wrapper& out);
    };
}
