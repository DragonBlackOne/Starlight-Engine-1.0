// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "SceneSerializer.hpp"
#include "CoreMinimal.hpp"
#include "Components.hpp"
#include "Log.hpp"
#include <json.hpp>
#include <fstream>

using json = nlohmann::json;

namespace starlight {

    // --- glm helpers ---
    static json Vec3ToJson(const glm::vec3& v) {
        return { {"x", v.x}, {"y", v.y}, {"z", v.z} };
    }
    static glm::vec3 JsonToVec3(const json& j) {
        return { j["x"].get<float>(), j["y"].get<float>(), j["z"].get<float>() };
    }
    static json QuatToJson(const glm::quat& q) {
        return { {"w", q.w}, {"x", q.x}, {"y", q.y}, {"z", q.z} };
    }
    static glm::quat JsonToQuat(const json& j) {
        return glm::quat(j["w"].get<float>(), j["x"].get<float>(), j["y"].get<float>(), j["z"].get<float>());
    }

    // =========================================================================
    //  SAVE
    // =========================================================================
    void SceneSerializer::SaveToFile(Scene& scene, const std::string& filepath) {
        auto& registry = scene.GetRegistry();
        json root;
        root["titan_version"] = "1.0.0";
        root["scene"] = json::array();

        auto& storage = registry.storage<entt::entity>();
        for (auto it = storage.begin(); it != storage.end(); ++it) {
            entt::entity entity = *it;
            json entityObj;
            entityObj["id"] = static_cast<uint32_t>(entity);

            // TransformComponent
            if (registry.all_of<TransformComponent>(entity)) {
                auto& t = registry.get<TransformComponent>(entity);
                entityObj["transform"] = {
                    {"position", Vec3ToJson(t.position)},
                    {"rotation", QuatToJson(t.rotation)},
                    {"scale", Vec3ToJson(t.scale)}
                };
            }

            // CameraComponent
            if (registry.all_of<CameraComponent>(entity)) {
                auto& c = registry.get<CameraComponent>(entity);
                entityObj["camera"] = {
                    {"fov", c.fov},
                    {"near", c.nearPlane},
                    {"far", c.farPlane},
                    {"primary", c.primary}
                };
            }

            // MeshComponent (we save the material properties, not the GPU mesh)
            if (registry.all_of<MeshComponent>(entity)) {
                auto& m = registry.get<MeshComponent>(entity).material;
                entityObj["material"] = {
                    {"color", Vec3ToJson(m.color)},
                    {"isPBR", m.isPBR},
                    {"albedo", Vec3ToJson(m.albedo)},
                    {"metallic", m.metallic},
                    {"roughness", m.roughness},
                    {"ao", m.ao}
                };
            }

            // PointLightComponent
            if (registry.all_of<PointLightComponent>(entity)) {
                auto& l = registry.get<PointLightComponent>(entity);
                entityObj["pointlight"] = {
                    {"color", Vec3ToJson(l.color)},
                    {"intensity", l.intensity}
                };
            }

            // RetroComponent
            if (registry.all_of<RetroComponent>(entity)) {
                auto& r = registry.get<RetroComponent>(entity);
                entityObj["retro"] = {
                    {"map_x", r.map_x}, {"map_y", r.map_y}, {"map_z", r.map_z},
                    {"horizon", r.horizon}, {"angle", r.angle}, {"pitch", r.pitch},
                    {"active", r.active},
                    {"sky", Vec3ToJson(r.skyColor)},
                    {"ground1", Vec3ToJson(r.groundColor1)},
                    {"ground2", Vec3ToJson(r.groundColor2)}
                };
            }

            root["scene"].push_back(entityObj);
        }

        std::ofstream file(filepath);
        if (file.is_open()) {
            file << root.dump(2);
            file.close();
            Log::Info("Scene saved to: " + filepath);
        } else {
            Log::Error("Failed to save scene: " + filepath);
        }
    }

    // =========================================================================
    //  LOAD
    // =========================================================================
    void SceneSerializer::LoadFromFile(Scene& scene, const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            Log::Error("Failed to load scene: " + filepath);
            return;
        }

        json root;
        try {
            file >> root;
        } catch (const std::exception& e) {
            Log::Error("JSON parse error: " + std::string(e.what()));
            return;
        }

        auto& registry = scene.GetRegistry();
        // Clear existing entities
        registry.clear();

        if (!root.contains("scene")) return;

        for (auto& entityObj : root["scene"]) {
            auto entity = registry.create();

            // TransformComponent
            if (entityObj.contains("transform")) {
                auto& tj = entityObj["transform"];
                auto& t = registry.emplace<TransformComponent>(entity);
                t.position = JsonToVec3(tj["position"]);
                t.rotation = JsonToQuat(tj["rotation"]);
                t.scale = JsonToVec3(tj["scale"]);
            }

            // CameraComponent
            if (entityObj.contains("camera")) {
                auto& cj = entityObj["camera"];
                auto& c = registry.emplace<CameraComponent>(entity);
                c.fov = cj["fov"].get<float>();
                c.nearPlane = cj["near"].get<float>();
                c.farPlane = cj["far"].get<float>();
                c.primary = cj["primary"].get<bool>();
            }

            // MeshComponent (material only â€” mesh assignment must be done by the module)
            if (entityObj.contains("material")) {
                auto& mj = entityObj["material"];
                auto& mc = registry.emplace<MeshComponent>(entity);
                mc.material.color = JsonToVec3(mj["color"]);
                mc.material.isPBR = mj["isPBR"].get<bool>();
                mc.material.albedo = JsonToVec3(mj["albedo"]);
                mc.material.metallic = mj["metallic"].get<float>();
                mc.material.roughness = mj["roughness"].get<float>();
                mc.material.ao = mj["ao"].get<float>();
            }

            // PointLightComponent
            if (entityObj.contains("pointlight")) {
                auto& lj = entityObj["pointlight"];
                auto& l = registry.emplace<PointLightComponent>(entity);
                l.color = JsonToVec3(lj["color"]);
                l.intensity = lj["intensity"].get<float>();
            }

            // RetroComponent
            if (entityObj.contains("retro")) {
                auto& rj = entityObj["retro"];
                auto& r = registry.emplace<RetroComponent>(entity);
                r.map_x = rj["map_x"].get<float>();
                r.map_y = rj["map_y"].get<float>();
                r.map_z = rj["map_z"].get<float>();
                r.horizon = rj["horizon"].get<float>();
                r.angle = rj["angle"].get<float>();
                r.pitch = rj["pitch"].get<float>();
                r.active = rj["active"].get<bool>();
                r.skyColor = JsonToVec3(rj["sky"]);
                r.groundColor1 = JsonToVec3(rj["ground1"]);
                r.groundColor2 = JsonToVec3(rj["ground2"]);
            }
        }

        Log::Info("Scene loaded from: " + filepath + " (" + std::to_string(root["scene"].size()) + " entities)");
    }
}
