#include "SceneSerializer.hpp"
#include "CoreMinimal.hpp"
#include "Components.hpp"
#include "SpriteAnimationComponent.hpp"
#include "Log.hpp"
#include "VFSSystem.hpp"
#include "PathResolver.hpp"
#include <fstream>
#include <unordered_map>

using json = nlohmann::json;

namespace starlight {

    // --- glm helpers ---
    static json Vec2ToJson(const glm::vec2& v) {
        return { {"x", v.x}, {"y", v.y} };
    }
    static glm::vec2 JsonToVec2(const json& j) {
        return { j["x"].get<float>(), j["y"].get<float>() };
    }
    static json Vec3ToJson(const glm::vec3& v) {
        return { {"x", v.x}, {"y", v.y}, {"z", v.z} };
    }
    static glm::vec3 JsonToVec3(const json& j) {
        return { j["x"].get<float>(), j["y"].get<float>(), j["z"].get<float>() };
    }
    static json Vec4ToJson(const glm::vec4& v) {
        return { {"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w} };
    }
    static glm::vec4 JsonToVec4(const json& j) {
        return { j["x"].get<float>(), j["y"].get<float>(), j["z"].get<float>(), j["w"].get<float>() };
    }
    static json QuatToJson(const glm::quat& q) {
        return { {"w", q.w}, {"x", q.x}, {"y", q.y}, {"z", q.z} };
    }
    static glm::quat JsonToQuat(const json& j) {
        return glm::quat(j["w"].get<float>(), j["x"].get<float>(), j["y"].get<float>(), j["z"].get<float>());
    }

    // =========================================================================
    //  SAVE TO JSON  (registry -> json object, all 2D components)
    // =========================================================================
    void SceneSerializer::SaveToJson(entt::registry& registry, nlohmann::json& outJson) {
        outJson["starlight_version"] = "2.0.0";
        outJson["scene"] = json::array();

        auto& storage = registry.storage<entt::entity>();
        for (auto it = storage.begin(); it != storage.end(); ++it) {
            entt::entity entity = *it;
            json entityObj;
            entityObj["id"] = static_cast<uint32_t>(entity);

            // TagComponent
            if (registry.all_of<TagComponent>(entity)) {
                entityObj["tag"] = registry.get<TagComponent>(entity).tag;
            }

            // TransformComponent
            if (registry.all_of<TransformComponent>(entity)) {
                auto& t = registry.get<TransformComponent>(entity);
                entityObj["transform"] = {
                    {"position", Vec3ToJson(t.position)},
                    {"rotation", QuatToJson(t.rotation)},
                    {"scale", Vec3ToJson(t.scale)}
                };
            }

            // CameraComponent (3D)
            if (registry.all_of<CameraComponent>(entity)) {
                auto& c = registry.get<CameraComponent>(entity);
                entityObj["camera"] = {
                    {"fov", c.fov},
                    {"near", c.nearPlane},
                    {"far", c.farPlane},
                    {"primary", c.primary}
                };
            }

            // Camera2DComponent
            if (registry.all_of<Camera2DComponent>(entity)) {
                auto& c = registry.get<Camera2DComponent>(entity);
                entityObj["camera2d"] = {
                    {"zoom", c.zoom},
                    {"rotation", c.rotation},
                    {"primary", c.primary}
                };
            }

            // SpriteComponent
            if (registry.all_of<SpriteComponent>(entity)) {
                auto& s = registry.get<SpriteComponent>(entity);
                entityObj["sprite"] = {
                    {"textureID", s.textureID},
                    {"color", Vec4ToJson(s.color)},
                    {"pivot", Vec2ToJson(s.pivot)},
                    {"layer", s.layer},
                    {"orderInLayer", s.orderInLayer},
                    {"flipX", s.flipX},
                    {"flipY", s.flipY},
                    {"visible", s.visible}
                };
            }

            // TilemapComponent
            if (registry.all_of<TilemapComponent>(entity)) {
                auto& t = registry.get<TilemapComponent>(entity);
                entityObj["tilemap"] = {
                    {"tiles", t.tiles},
                    {"mapWidth", t.mapWidth},
                    {"mapHeight", t.mapHeight},
                    {"tileSize", t.tileSize},
                    {"tilesetTexture", t.tilesetTexture},
                    {"tilesetColumns", t.tilesetColumns},
                    {"tilesetRows", t.tilesetRows},
                    {"color", Vec4ToJson(t.color)},
                    {"layer", t.layer}
                };
            }

            // MeshComponent (material only)
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

            // SpriteAnimationComponent
            if (registry.all_of<SpriteAnimationComponent>(entity)) {
                auto& a = registry.get<SpriteAnimationComponent>(entity);
                entityObj["sprite_animation"] = {
                    {"frames", a.frames},
                    {"currentFrame", a.currentFrame},
                    {"fps", a.fps},
                    {"timer", a.timer},
                    {"playing", a.playing},
                    {"looping", a.looping}
                };
            }

            // ReverbZoneComponent
            if (registry.all_of<ReverbZoneComponent>(entity)) {
                auto& r = registry.get<ReverbZoneComponent>(entity);
                entityObj["reverb_zone"] = {
                    {"minDistance", r.minDistance},
                    {"maxDistance", r.maxDistance},
                    {"reverbFactor", r.reverbFactor},
                    {"active", r.active}
                };
            }

            // CharacterControllerComponent
            if (registry.all_of<CharacterControllerComponent>(entity)) {
                auto& c = registry.get<CharacterControllerComponent>(entity);
                entityObj["character_controller"] = {
                    {"height", c.height},
                    {"radius", c.radius},
                    {"maxSlopeAngle", c.maxSlopeAngle},
                    {"jumpStrength", c.jumpStrength},
                    {"speed", c.speed},
                    {"velocity", Vec3ToJson(c.velocity)}
                };
            }

            // FootIKComponent
            if (registry.all_of<FootIKComponent>(entity)) {
                auto& f = registry.get<FootIKComponent>(entity);
                entityObj["foot_ik"] = {
                    {"leftFootOffset", Vec3ToJson(f.leftFootOffset)},
                    {"rightFootOffset", Vec3ToJson(f.rightFootOffset)},
                    {"enabled", f.enabled}
                };
            }

            // NavAgentComponent
            if (registry.all_of<NavAgentComponent>(entity)) {
                auto& n = registry.get<NavAgentComponent>(entity);
                entityObj["nav_agent"] = {
                    {"target", Vec3ToJson(n.target)},
                    {"velocity", Vec3ToJson(n.velocity)},
                    {"radius", n.radius},
                    {"maxSpeed", n.maxSpeed},
                    {"active", n.active}
                };
            }

            // SoftBodyComponent
            if (registry.all_of<SoftBodyComponent>(entity)) {
                auto& s = registry.get<SoftBodyComponent>(entity);
                entityObj["soft_body"] = {
                    {"mass", s.mass},
                    {"pressure", s.pressure},
                    {"enabled", s.enabled}
                };
            }

            // RelationshipComponent (save IDs; resolved on load)
            if (registry.all_of<RelationshipComponent>(entity)) {
                auto& r = registry.get<RelationshipComponent>(entity);
                json relObj;
                relObj["parent"] = (r.parent != entt::null) ? static_cast<uint32_t>(r.parent) : -1;
                relObj["children"] = json::array();
                for (auto child : r.children) {
                    relObj["children"].push_back(static_cast<uint32_t>(child));
                }
                entityObj["relationship"] = relObj;
            }

            outJson["scene"].push_back(entityObj);
        }
    }

    // =========================================================================
    //  SAVE TO FILE
    // =========================================================================
    void SceneSerializer::SaveToFile(const Scene& scene, const std::string& filepath) {
        auto& registry = const_cast<Scene&>(scene).GetRegistry();
        json root;
        SaveToJson(registry, root);

        std::string resolvedPath = filepath;
        if (filepath.find(":") == std::string::npos && filepath.front() != '/' && filepath.front() != '\\') {
            resolvedPath = PathResolver::ExeDir() + "/" + filepath;
        }

        std::filesystem::create_directories(std::filesystem::path(resolvedPath).parent_path());

        std::ofstream file(resolvedPath);
        if (file.is_open()) {
            file << root.dump(2);
            file.close();
            Log::Info("Scene saved to: " + resolvedPath);
        } else {
            Log::Error("Failed to save scene: " + resolvedPath);
        }
    }

    // =========================================================================
    //  LOAD FROM JSON  (json object -> registry, all 2D components)
    // =========================================================================
    void SceneSerializer::LoadFromJson(entt::registry& registry, const nlohmann::json& inJson) {
        if (!inJson.contains("scene")) return;

        // First pass: create entities and store oldID -> newEntity mapping
        std::unordered_map<uint32_t, entt::entity> idMap;
        for (auto& entityObj : inJson["scene"]) {
            entt::entity entity = registry.create();
            uint32_t oldID = entityObj["id"].get<uint32_t>();
            idMap[oldID] = entity;
        }

        // Second pass: populate components
        for (auto& entityObj : inJson["scene"]) {
            uint32_t oldID = entityObj["id"].get<uint32_t>();
            entt::entity entity = idMap[oldID];

            // TagComponent
            if (entityObj.contains("tag")) {
                registry.emplace<TagComponent>(entity, entityObj["tag"].get<std::string>());
            }

            // TransformComponent
            if (entityObj.contains("transform")) {
                auto& tj = entityObj["transform"];
                auto& t = registry.emplace<TransformComponent>(entity);
                t.position = JsonToVec3(tj["position"]);
                t.rotation = JsonToQuat(tj["rotation"]);
                t.scale = JsonToVec3(tj["scale"]);
            }

            // CameraComponent (3D)
            if (entityObj.contains("camera")) {
                auto& cj = entityObj["camera"];
                auto& c = registry.emplace<CameraComponent>(entity);
                c.fov = cj["fov"].get<float>();
                c.nearPlane = cj["near"].get<float>();
                c.farPlane = cj["far"].get<float>();
                c.primary = cj["primary"].get<bool>();
            }

            // Camera2DComponent
            if (entityObj.contains("camera2d")) {
                auto& cj = entityObj["camera2d"];
                auto& c = registry.emplace<Camera2DComponent>(entity);
                c.zoom = cj["zoom"].get<float>();
                c.rotation = cj["rotation"].get<float>();
                c.primary = cj["primary"].get<bool>();
            }

            // SpriteComponent
            if (entityObj.contains("sprite")) {
                auto& sj = entityObj["sprite"];
                auto& s = registry.emplace<SpriteComponent>(entity);
                s.textureID = sj["textureID"].get<uint32_t>();
                s.color = JsonToVec4(sj["color"]);
                s.pivot = JsonToVec2(sj["pivot"]);
                s.layer = sj["layer"].get<int>();
                s.orderInLayer = sj["orderInLayer"].get<int>();
                s.flipX = sj["flipX"].get<bool>();
                s.flipY = sj["flipY"].get<bool>();
                s.visible = sj["visible"].get<bool>();
            }

            // TilemapComponent
            if (entityObj.contains("tilemap")) {
                auto& tj = entityObj["tilemap"];
                auto& t = registry.emplace<TilemapComponent>(entity);
                t.tiles = tj["tiles"].get<std::vector<int>>();
                t.mapWidth = tj["mapWidth"].get<int>();
                t.mapHeight = tj["mapHeight"].get<int>();
                t.tileSize = tj["tileSize"].get<int>();
                t.tilesetTexture = tj["tilesetTexture"].get<uint32_t>();
                t.tilesetColumns = tj["tilesetColumns"].get<int>();
                t.tilesetRows = tj["tilesetRows"].get<int>();
                t.color = JsonToVec4(tj["color"]);
                t.layer = tj["layer"].get<int>();
            }

            // MeshComponent (material only)
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

            // SpriteAnimationComponent
            if (entityObj.contains("sprite_animation")) {
                auto& aj = entityObj["sprite_animation"];
                auto& a = registry.emplace<SpriteAnimationComponent>(entity);
                if (aj.contains("frames")) a.frames = aj["frames"].get<std::vector<uint32_t>>();
                if (aj.contains("currentFrame")) a.currentFrame = aj["currentFrame"].get<int>();
                if (aj.contains("fps")) a.fps = aj["fps"].get<float>();
                if (aj.contains("timer")) a.timer = aj["timer"].get<float>();
                if (aj.contains("playing")) a.playing = aj["playing"].get<bool>();
                if (aj.contains("looping")) a.looping = aj["looping"].get<bool>();
            }

            // ReverbZoneComponent
            if (entityObj.contains("reverb_zone")) {
                auto& rj = entityObj["reverb_zone"];
                auto& r = registry.emplace<ReverbZoneComponent>(entity);
                if (rj.contains("minDistance")) r.minDistance = rj["minDistance"].get<float>();
                if (rj.contains("maxDistance")) r.maxDistance = rj["maxDistance"].get<float>();
                if (rj.contains("reverbFactor")) r.reverbFactor = rj["reverbFactor"].get<float>();
                if (rj.contains("active")) r.active = rj["active"].get<bool>();
            }

            // CharacterControllerComponent
            if (entityObj.contains("character_controller")) {
                auto& cj = entityObj["character_controller"];
                auto& c = registry.emplace<CharacterControllerComponent>(entity);
                if (cj.contains("height")) c.height = cj["height"].get<float>();
                if (cj.contains("radius")) c.radius = cj["radius"].get<float>();
                if (cj.contains("maxSlopeAngle")) c.maxSlopeAngle = cj["maxSlopeAngle"].get<float>();
                if (cj.contains("jumpStrength")) c.jumpStrength = cj["jumpStrength"].get<float>();
                if (cj.contains("speed")) c.speed = cj["speed"].get<float>();
                if (cj.contains("velocity")) c.velocity = JsonToVec3(cj["velocity"]);
            }

            // FootIKComponent
            if (entityObj.contains("foot_ik")) {
                auto& fj = entityObj["foot_ik"];
                auto& f = registry.emplace<FootIKComponent>(entity);
                if (fj.contains("leftFootOffset")) f.leftFootOffset = JsonToVec3(fj["leftFootOffset"]);
                if (fj.contains("rightFootOffset")) f.rightFootOffset = JsonToVec3(fj["rightFootOffset"]);
                if (fj.contains("enabled")) f.enabled = fj["enabled"].get<bool>();
            }

            // NavAgentComponent
            if (entityObj.contains("nav_agent")) {
                auto& nj = entityObj["nav_agent"];
                auto& n = registry.emplace<NavAgentComponent>(entity);
                if (nj.contains("target")) n.target = JsonToVec3(nj["target"]);
                if (nj.contains("velocity")) n.velocity = JsonToVec3(nj["velocity"]);
                if (nj.contains("radius")) n.radius = nj["radius"].get<float>();
                if (nj.contains("maxSpeed")) n.maxSpeed = nj["maxSpeed"].get<float>();
                if (nj.contains("active")) n.active = nj["active"].get<bool>();
            }

            // SoftBodyComponent
            if (entityObj.contains("soft_body")) {
                auto& sj = entityObj["soft_body"];
                auto& s = registry.emplace<SoftBodyComponent>(entity);
                if (sj.contains("mass")) s.mass = sj["mass"].get<float>();
                if (sj.contains("pressure")) s.pressure = sj["pressure"].get<float>();
                if (sj.contains("enabled")) s.enabled = sj["enabled"].get<bool>();
            }

            // RelationshipComponent (resolve IDs via idMap)
            if (entityObj.contains("relationship")) {
                auto& rj = entityObj["relationship"];
                auto& r = registry.emplace<RelationshipComponent>(entity);
                int parentID = rj["parent"].get<int>();
                if (parentID >= 0) {
                    r.parent = idMap[static_cast<uint32_t>(parentID)];
                }
                for (auto& childVal : rj["children"]) {
                    uint32_t childID = childVal.get<uint32_t>();
                    auto it = idMap.find(childID);
                    if (it != idMap.end()) {
                        r.children.push_back(it->second);
                    }
                }
            }
        }
    }

    // =========================================================================
    //  LOAD FROM FILE
    // =========================================================================
    void SceneSerializer::LoadFromFile(Scene& scene, const std::string& filepath) {
        auto bytes = VFSSystem::Get().ReadFile(filepath);
        if (bytes.empty()) {
            Log::Error("Failed to load scene: " + filepath);
            return;
        }

        json root;
        try {
            root = json::parse(bytes.begin(), bytes.end());
        } catch (const std::exception& e) {
            Log::Error("JSON parse error: " + std::string(e.what()));
            return;
        }

        auto& registry = scene.GetRegistry();
        registry.clear();
        try {
            LoadFromJson(registry, root);
        } catch (const std::exception& e) {
            Log::Error("Error deserializing components: " + std::string(e.what()));
            return;
        }

        Log::Info("Scene loaded from: " + filepath + " (" + std::to_string(root["scene"].size()) + " entities)");
    }
}
