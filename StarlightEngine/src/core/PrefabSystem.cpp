#include "PrefabSystem.hpp"
#include "VFSSystem.hpp"
#include "Engine.hpp"
#include "Components.hpp"
#include "Log.hpp"
#include <json.hpp>
#include <fstream>

namespace starlight {
    entt::entity PrefabSystem::Instantiate(const std::string& prefabVfsPath, entt::registry* customRegistry) {
        entt::registry* regPtr = customRegistry;
        if (!regPtr) {
            if (!Engine::IsInitialized()) {
                Log::Error("PrefabSystem: Engine is not initialized and no custom registry was provided.");
                return entt::null;
            }
            auto activeScene = Engine::Get().GetSceneStack().Active();
            if (!activeScene) {
                Log::Error("PrefabSystem: No active scene and no custom registry to instantiate prefab.");
                return entt::null;
            }
            regPtr = &activeScene->GetRegistry();
        }

        auto& reg = *regPtr;

        std::vector<uint8_t> data = VFSSystem::Get().ReadFile(prefabVfsPath);
        if (data.empty()) {
            Log::Error("PrefabSystem: Failed to read prefab file from VFS or file is empty: " + prefabVfsPath);
            return entt::null;
        }

        std::string jsonStr(data.begin(), data.end());
        try {
            auto json = nlohmann::json::parse(jsonStr);
            entt::entity entity = reg.create();

            // Default components
            reg.emplace<TransformComponent>(entity);

            if (json.contains("TagComponent")) {
                auto tagVal = json["TagComponent"].value("tag", "Entity");
                reg.emplace<TagComponent>(entity, tagVal);
            } else {
                reg.emplace<TagComponent>(entity, "PrefabEntity");
            }

            if (json.contains("TransformComponent")) {
                auto& trans = reg.get<TransformComponent>(entity);
                auto transJ = json["TransformComponent"];
                if (transJ.contains("position")) {
                    trans.position = glm::vec3(
                        transJ["position"].value("x", 0.0f),
                        transJ["position"].value("y", 0.0f),
                        transJ["position"].value("z", 0.0f)
                    );
                }
                if (transJ.contains("scale")) {
                    trans.scale = glm::vec3(
                        transJ["scale"].value("x", 1.0f),
                        transJ["scale"].value("y", 1.0f),
                        transJ["scale"].value("z", 1.0f)
                    );
                }
                if (transJ.contains("rotation")) {
                    float rx = transJ["rotation"].value("x", 0.0f);
                    float ry = transJ["rotation"].value("y", 0.0f);
                    float rz = transJ["rotation"].value("z", 0.0f);
                    trans.rotation = glm::quat(glm::vec3(glm::radians(rx), glm::radians(ry), glm::radians(rz)));
                }
                trans.UpdateLocalMatrix();
            }

            if (json.contains("SpriteComponent")) {
                auto spriteJ = json["SpriteComponent"];
                SpriteComponent sc;
                sc.textureID = spriteJ.value("textureID", 0u);
                if (spriteJ.contains("color")) {
                    sc.color = glm::vec4(
                        spriteJ["color"].value("r", 1.0f),
                        spriteJ["color"].value("g", 1.0f),
                        spriteJ["color"].value("b", 1.0f),
                        spriteJ["color"].value("a", 1.0f)
                    );
                }
                sc.flipX = spriteJ.value("flipX", false);
                sc.flipY = spriteJ.value("flipY", false);
                sc.visible = spriteJ.value("visible", true);
                reg.emplace<SpriteComponent>(entity, sc);
            }

            if (json.contains("ReverbZoneComponent")) {
                auto reverbJ = json["ReverbZoneComponent"];
                ReverbZoneComponent rzc;
                rzc.minDistance = reverbJ.value("minDistance", 2.0f);
                rzc.maxDistance = reverbJ.value("maxDistance", 10.0f);
                rzc.reverbFactor = reverbJ.value("reverbFactor", 0.5f);
                rzc.active = reverbJ.value("active", true);
                reg.emplace<ReverbZoneComponent>(entity, rzc);
            }

            Log::Info("PrefabSystem: Instantiated prefab successfully: " + prefabVfsPath);
            return entity;

        } catch (const std::exception& e) {
            Log::Error("PrefabSystem: Error parsing prefab JSON: " + std::string(e.what()));
        }

        return entt::null;
    }

    bool PrefabSystem::Save(const std::string& prefabVfsPath, entt::entity entity, entt::registry* registry) {
        if (!registry || entity == entt::null) return false;
        nlohmann::json json;

        if (registry->all_of<TagComponent>(entity)) {
            auto& c = registry->get<TagComponent>(entity);
            json["TagComponent"] = {
                {"tag", c.tag}
            };
        }
        if (registry->all_of<TransformComponent>(entity)) {
            auto& c = registry->get<TransformComponent>(entity);
            glm::vec3 euler = glm::degrees(glm::eulerAngles(c.rotation));
            json["TransformComponent"] = {
                {"position", {{"x", c.position.x}, {"y", c.position.y}, {"z", c.position.z}}},
                {"scale", {{"x", c.scale.x}, {"y", c.scale.y}, {"z", c.scale.z}}},
                {"rotation", {{"x", euler.x}, {"y", euler.y}, {"z", euler.z}}}
            };
        }
        if (registry->all_of<SpriteComponent>(entity)) {
            auto& c = registry->get<SpriteComponent>(entity);
            json["SpriteComponent"] = {
                {"textureID", c.textureID},
                {"color", {{"r", c.color.r}, {"g", c.color.g}, {"b", c.color.b}, {"a", c.color.a}}},
                {"flipX", c.flipX},
                {"flipY", c.flipY},
                {"visible", c.visible}
            };
        }
        if (registry->all_of<ReverbZoneComponent>(entity)) {
            auto& c = registry->get<ReverbZoneComponent>(entity);
            json["ReverbZoneComponent"] = {
                {"minDistance", c.minDistance},
                {"maxDistance", c.maxDistance},
                {"reverbFactor", c.reverbFactor},
                {"active", c.active}
            };
        }

        std::ofstream f(prefabVfsPath);
        if (f.is_open()) {
            f << json.dump(4);
            Log::Info("PrefabSystem: Saved prefab successfully to " + prefabVfsPath);
            return true;
        }
        Log::Error("PrefabSystem: Failed to open file for saving prefab: " + prefabVfsPath);
        return false;
    }
}
