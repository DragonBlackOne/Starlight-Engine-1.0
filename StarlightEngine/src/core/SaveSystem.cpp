// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "SaveSystem.hpp"
#include "Components.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "Log.hpp"

using json = nlohmann::json;

namespace starlight {

    bool SaveSystem::SaveWorld(entt::registry& registry, const std::string& filename) {
        json j;
        j["entities"] = json::array();

        auto view = registry.view<TransformComponent, TagComponent>();
        for (auto entity : view) {
            auto& t = view.get<TransformComponent>(entity);
            auto& n = view.get<TagComponent>(entity);

            json e;
            e["name"] = n.tag;
            e["pos"] = { t.position.x, t.position.y, t.position.z };
            e["rot"] = { t.rotation.x, t.rotation.y, t.rotation.z, t.rotation.w };
            
            j["entities"].push_back(e);
        }

        std::string tempFilename = filename + ".tmp";
        std::ofstream file(tempFilename);
        if (file.is_open()) {
            file << j.dump(4);
            file.close();

            // Transactional Swap (Atomic on many systems, safer on all)
            if (std::rename(tempFilename.c_str(), filename.c_str()) == 0) {
                Log::Info("Transactional Save Success: %s", filename.c_str());
                return true;
            } else {
                Log::Error("Transactional Rename Failed: %s", filename.c_str());
                return false;
            }
        }
        return false;
    }

    bool SaveSystem::LoadWorld(entt::registry& registry, const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        json j;
        file >> j;

        registry.clear(); // Fresh start

        for (const auto& e : j["entities"]) {
            auto entity = registry.create();
            registry.emplace<TagComponent>(entity, e["name"].get<std::string>());
            
            glm::vec3 pos(e["pos"][0], e["pos"][1], e["pos"][2]);
            glm::quat rot(e["rot"][3], e["rot"][0], e["rot"][1], e["rot"][2]); // w, x, y, z
            
            auto& tc = registry.emplace<TransformComponent>(entity);
            tc.position = pos;
            tc.rotation = rot;
            tc.UpdateLocalMatrix();
        }

        return true;
    }

}
