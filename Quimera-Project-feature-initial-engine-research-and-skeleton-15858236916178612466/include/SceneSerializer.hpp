// Este projeto é feito por IA e só o prompt é feito por um humano.
#include <yaml-cpp/yaml.h>
#include <entt/entt.hpp>
#include <fstream>
#include <filesystem>
#include "MeshComponent.hpp"
#include "CameraComponent.hpp"
#include "TransformComponent.hpp"
#include "ScriptComponent.hpp"

namespace Vamos {

    class SceneSerializer {
    public:
        static void Serialize(entt::registry& registry, const std::string& filepath) {
            YAML::Emitter out;
            out << YAML::BeginMap;
            out << YAML::Key << "Scene" << YAML::Value << "Untitled Scene";
            out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

            for (auto entityID : registry.storage<entt::entity>()) {
                out << YAML::BeginMap;
                out << YAML::Key << "Entity" << YAML::Value << (uint32_t)entityID;

                if (registry.all_of<Transform>(entityID)) {
                    auto& t = registry.get<Transform>(entityID);
                    out << YAML::Key << "Transform" << YAML::BeginMap;
                    out << YAML::Key << "Position" << YAML::Flow << YAML::BeginSeq << t.position.x << t.position.y << t.position.z << YAML::EndSeq;
                    out << YAML::Key << "Rotation" << YAML::Flow << YAML::BeginSeq << t.rotation.x << t.rotation.y << t.rotation.z << YAML::EndSeq;
                    out << YAML::Key << "Scale" << YAML::Flow << YAML::BeginSeq << t.scale.x << t.scale.y << t.scale.z << YAML::EndSeq;
                    out << YAML::EndMap;
                }

                if (registry.all_of<Mesh>(entityID)) {
                    auto& m = registry.get<Mesh>(entityID);
                    out << YAML::Key << "Mesh" << YAML::BeginMap;
                    out << YAML::Key << "VertexCount" << YAML::Value << m.vertexCount;
                    // Em um cenário real, salvaríamos o path do .obj ou .gltf
                    out << YAML::EndMap;
                }

                if (registry.all_of<ScriptComponent>(entityID)) {
                    auto& s = registry.get<ScriptComponent>(entityID);
                    out << YAML::Key << "ScriptComponent" << YAML::BeginMap;
                    out << YAML::Key << "Path" << YAML::Value << s.scriptPath;
                    out << YAML::EndMap;
                }

                if (registry.all_of<CameraComponent>(entityID)) {
                    auto& c = registry.get<CameraComponent>(entityID);
                    out << YAML::Key << "CameraComponent" << YAML::BeginMap;
                    out << YAML::Key << "FOV" << YAML::Value << c.fov;
                    out << YAML::Key << "AspectRatio" << YAML::Value << c.aspectRatio;
                    out << YAML::EndMap;
                }

                out << YAML::EndMap;
            }

            out << YAML::EndSeq;
            out << YAML::EndMap;

            std::filesystem::path p(filepath);
            std::filesystem::create_directories(p.parent_path());
            
            std::ofstream fout(filepath);
            fout << out.c_str();
        }

        static bool Deserialize(entt::registry& registry, const std::string& filepath) {
            std::ifstream stream(filepath);
            if (!stream.is_open()) return false;

            YAML::Node data = YAML::Load(stream);
            if (!data["Scene"]) return false;

            registry.clear();

            auto entities = data["Entities"];
            if (entities) {
                for (auto entity : entities) {
                    auto ent = registry.create();

                    auto transform = entity["Transform"];
                    if (transform) {
                        auto& t = registry.emplace<Transform>(ent);
                        t.position.x = transform["Position"][0].as<float>();
                        t.position.y = transform["Position"][1].as<float>();
                        t.position.z = transform["Position"][2].as<float>();
                        
                        t.rotation.x = transform["Rotation"][0].as<float>();
                        t.rotation.y = transform["Rotation"][1].as<float>();
                        t.rotation.z = transform["Rotation"][2].as<float>();

                        t.scale.x = transform["Scale"][0].as<float>();
                        t.scale.y = transform["Scale"][1].as<float>();
                        t.scale.z = transform["Scale"][2].as<float>();
                    }

                    auto mesh = entity["Mesh"];
                    if (mesh) {
                        auto& m = registry.emplace<Mesh>(ent);
                        m.vertexCount = mesh["VertexCount"].as<int>();
                        // Mock do triângulo padrão se for recreado
                        m.vertices = {
                            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
                             0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
                             0.0f,  0.5f, 0.0f,  0.5f, 1.0f
                        };
                    }

                    auto script = entity["ScriptComponent"];
                    if (script) {
                        auto& s = registry.emplace<ScriptComponent>(ent);
                        s.scriptPath = script["Path"].as<std::string>();
                    }

                    auto camera = entity["CameraComponent"];
                    if (camera) {
                        auto& c = registry.emplace<CameraComponent>(ent);
                        c.fov = camera["FOV"].as<float>();
                        c.aspectRatio = camera["AspectRatio"].as<float>();
                    }
                }
            }
            return true;
        }
    };
}
