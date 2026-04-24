// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "SceneLoader.hpp"
#include "Components.hpp"
#include "Log.hpp"
#include <fstream>
#include <sstream>
#include <vector>

namespace titan {

    // Simple JSON-like parser for the showcase
    // In a production environment, this would use cJSON or nlohmann/json
    bool SceneLoader::Load(const std::string& filepath, entt::registry& registry) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            Log::Error("SceneLoader: Failed to open scene file: " + filepath);
            return false;
        }

        Log::Info("SceneLoader: Loading data-driven scene from " + filepath);
        
        // Mock loading entities for the showcase
        // (Real implementation would parse the JSON string)
        for(int i = 0; i < 5; i++) {
            auto e = registry.create();
            registry.emplace<TransformComponent>(e, glm::vec3(i * 5, 0, 0));
            registry.emplace<TagComponent>(e, "LoadedEntity_" + std::to_string(i));
        }

        return true;
    }

    bool SceneLoader::Save(const std::string& filepath, entt::registry& registry) {
        std::ofstream file(filepath);
        if (!file.is_open()) return false;

        file << "{\n  \"entities\": [\n";
        auto view = registry.view<TransformComponent, TagComponent>();
        bool first = true;
        for (auto entity : view) {
            if (!first) file << ",\n";
            auto& t = view.get<TransformComponent>(entity);
            auto& tag = view.get<TagComponent>(entity);
            
            file << "    {\n";
            file << "      \"name\": \"" << tag.tag << "\",\n";
            file << "      \"position\": [" << t.position.x << "," << t.position.y << "," << t.position.z << "]\n";
            file << "    }";
            first = false;
        }
        file << "\n  ]\n}";
        
        Log::Info("SceneLoader: Registry state serialized to " + filepath);
        return true;
    }

}
