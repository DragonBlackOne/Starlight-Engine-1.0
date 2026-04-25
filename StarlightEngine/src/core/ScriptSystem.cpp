// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "ScriptSystem.hpp"
#include "Log.hpp"
#include "Engine.hpp"
#include "InputSystem.hpp"
#include "TitanAudio.hpp"
#include "Components.hpp"
#include "SceneSerializer.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <sol/sol.hpp>

namespace starlight {
    ScriptSystem::ScriptSystem() {
        m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table, sol::lib::os);
    }

    ScriptSystem::~ScriptSystem() {}

    void ScriptSystem::Initialize() {
        BindCore();
        Log::Info("Script System (Lua) Initialized with Advanced Bindings.");
    }

    void ScriptSystem::BindCore() {
        // --- LOG BINDINGS ---
        m_lua.new_usertype<Log>("Log",
            sol::no_constructor,
            "info", [](const std::string& msg) { Log::Info(msg); },
            "warn", [](const std::string& msg) { Log::Warn(msg); },
            "error", [](const std::string& msg) { Log::Error(msg); }
        );

        // --- MATH BINDINGS (GLM) ---
        m_lua.new_usertype<glm::vec3>("vec3",
            sol::call_constructor, sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z,
            sol::meta_function::addition, [](const glm::vec3& a, const glm::vec3& b) { return a + b; },
            sol::meta_function::subtraction, [](const glm::vec3& a, const glm::vec3& b) { return a - b; },
            sol::meta_function::multiplication, [](const glm::vec3& a, float b) { return a * b; }
        );

        m_lua.new_usertype<glm::quat>("quat",
            sol::call_constructor, sol::constructors<glm::quat(), glm::quat(glm::vec3)>(),
            "w", &glm::quat::w,
            "x", &glm::quat::x,
            "y", &glm::quat::y,
            "z", &glm::quat::z,
            "euler", [](float x, float y, float z) { return glm::quat(glm::vec3(x, y, z)); },
            sol::meta_function::multiplication, [](const glm::quat& a, const glm::quat& b) { return a * b; }
        );

        // --- COMPONENT BINDINGS ---
        m_lua.new_usertype<TransformComponent>("Transform",
            "position", &TransformComponent::position,
            "rotation", &TransformComponent::rotation,
            "scale", &TransformComponent::scale
        );
        
        m_lua.new_usertype<PointLightComponent>("PointLight",
            "color", &PointLightComponent::color,
            "intensity", &PointLightComponent::intensity
        );
        
        m_lua.new_usertype<CameraComponent>("Camera",
            "fov", &CameraComponent::fov,
            "nearPlane", &CameraComponent::nearPlane,
            "farPlane", &CameraComponent::farPlane,
            "primary", &CameraComponent::primary
        );

        m_lua.new_usertype<Material>("Material",
            "color", &Material::color,
            "albedo", &Material::albedo,
            "metallic", &Material::metallic,
            "roughness", &Material::roughness,
            "ao", &Material::ao,
            "isPBR", &Material::isPBR,
            "isTransparent", &Material::isTransparent
        );

        // --- ECS BINDINGS ---
        m_lua.new_usertype<entt::registry>("Registry",
            "create", [](entt::registry& reg) { return reg.create(); },
            "destroy", [](entt::registry& reg, entt::entity e) { reg.destroy(e); },
            "add_transform", [](entt::registry& reg, entt::entity e) -> TransformComponent& { return reg.emplace<TransformComponent>(e); },
            "add_point_light", [](entt::registry& reg, entt::entity e) -> PointLightComponent& { return reg.emplace<PointLightComponent>(e); },
            "get_transform", [](entt::registry& reg, entt::entity e) -> TransformComponent& { return reg.get<TransformComponent>(e); },
            "get_mesh", [](entt::registry& reg, entt::entity e) -> MeshComponent& { return reg.get<MeshComponent>(e); },
            "add_mesh", [](entt::registry& reg, entt::entity e) -> MeshComponent& { return reg.emplace<MeshComponent>(e); },
            "add_camera", [](entt::registry& reg, entt::entity e) -> CameraComponent& { return reg.emplace<CameraComponent>(e); },
            "has_transform", [](entt::registry& reg, entt::entity e) { return reg.all_of<TransformComponent>(e); }
        );
        
        m_lua.new_usertype<MeshComponent>("MeshComponent",
            "mesh", &MeshComponent::mesh,
            "material", &MeshComponent::material,
            "isVisible", &MeshComponent::isVisible
        );

        // --- AUDIO BINDINGS ---
        m_lua.new_usertype<AudioSystem>("Audio",
            "play_note", &AudioSystem::PlayNote,
            "play_fm_note", &AudioSystem::PlayFMNote,
            "play_effect", &AudioSystem::PlayEffect
        );

        // --- INPUT BINDINGS ---
        m_lua.new_usertype<InputSystem>("Input",
            "is_pressed", &InputSystem::IsActionPressed,
            "is_just_pressed", &InputSystem::IsActionJustPressed
        );

        // --- ENGINE GLOBALS ---
        m_lua.new_usertype<Renderer>("Renderer",
            "get_cube_mesh", &Renderer::GetCubeMesh,
            "get_camera_transform", &Renderer::GetCameraTransform,
            "cloud_coverage", &Renderer::m_cloudCoverage,
            "cloud_density", &Renderer::m_cloudDensity
        );

        m_lua.new_usertype<Engine>("EngineClass",
            sol::no_constructor,
            "get_registry", [](Engine* e) -> entt::registry* { 
                auto scene = e->GetSceneStack().Active();
                if(!scene) return nullptr;
                return &scene->GetRegistry(); 
            },
            "get_input", [](Engine* e) { return &e->GetInput(); },
            "get_audio", [](Engine* e) { return &e->GetAudio(); },
            "get_time", [](Engine* e) { return &e->GetTime(); },
            "get_renderer", [](Engine* e) { return &e->GetRenderer(); }
        );

        m_lua["Engine"] = &Engine::Get();

        m_lua.new_usertype<EngineTime>("EngineTime",
            "dt", &EngineTime::deltaTime,
            "total", &EngineTime::totalTime
        );

        m_lua.set_function("load_scene", [](const std::string& path) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (scene) SceneSerializer::LoadFromFile(*scene, path);
        });

        // --- CONVENIENCE WRAPPERS ---
        m_lua.script(R"(
            Entity = {
                Create = function()
                    local e = Engine:get_registry():create()
                    Engine:get_registry():add_transform(e)
                    return {
                        id = e,
                        GetTransform = function(self) return Engine:get_registry():get_transform(self.id) end,
                        GetMesh = function(self) 
                            local m = Engine:get_registry():add_mesh(self.id)
                            m.material.isPBR = true
                            m.mesh = Engine:get_renderer():get_cube_mesh()
                            return m.material
                        end,
                        GetLight = function(self) return Engine:get_registry():add_point_light(self.id) end
                    }
                end
            }
            Camera = {
                GetPrimary = function()
                    -- Return a dummy with transform access for the primary camera
                    return {
                        GetTransform = function() 
                            -- In this engine, we'll just look for an entity with CameraComponent
                            -- For the showcase, we'll return a direct way to move the camera
                            return Engine:get_renderer():get_camera_transform()
                        end
                    }
                end
            }
        )");
    }

    void ScriptSystem::ExecuteFile(const std::string& path) {
        auto result = m_lua.safe_script_file(path, sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            Log::Error("Lua Error in " + path + ": " + std::string(err.what()));
        }
    }

    void ScriptSystem::Update(float dt) {
        // Handle OnStart (once)
        static bool started = false;
        if (!started) {
            sol::protected_function startFunc = m_lua["OnStart"];
            if (startFunc.valid()) {
                auto result = startFunc();
                if (!result.valid()) {
                    sol::error err = result;
                    Log::Error("Lua OnStart Error: " + std::string(err.what()));
                }
            }
            started = true;
        }

        sol::protected_function updateFunc = m_lua["OnUpdate"];
        if (updateFunc.valid()) {
            auto result = updateFunc(dt);
            if (!result.valid()) {
                sol::error err = result;
                Log::Error("Lua OnUpdate Error: " + std::string(err.what()));
            }
        }
    }
}
