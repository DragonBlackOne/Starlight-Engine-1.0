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

namespace titan {
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
            sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z,
            sol::meta_function::addition, [](const glm::vec3& a, const glm::vec3& b) { return a + b; },
            sol::meta_function::subtraction, [](const glm::vec3& a, const glm::vec3& b) { return a - b; },
            sol::meta_function::multiplication, [](const glm::vec3& a, float b) { return a * b; }
        );

        // --- COMPONENT BINDINGS ---
        m_lua.new_usertype<TransformComponent>("Transform",
            "position", &TransformComponent::position,
            "rotation", &TransformComponent::rotation,
            "scale", &TransformComponent::scale
        );

        // --- ECS BINDINGS ---
        m_lua.new_usertype<entt::registry>("Registry",
            "create", [](entt::registry& reg) { return reg.create(); },
            "destroy", [](entt::registry& reg, entt::entity e) { reg.destroy(e); },
            "add_transform", [](entt::registry& reg, entt::entity e) -> TransformComponent& { return reg.emplace<TransformComponent>(e); },
            "get_transform", [](entt::registry& reg, entt::entity e) -> TransformComponent& { return reg.get<TransformComponent>(e); },
            "has_transform", [](entt::registry& reg, entt::entity e) { return reg.all_of<TransformComponent>(e); }
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
        m_lua.new_usertype<Engine>("EngineClass",
            sol::no_constructor,
            "get_registry", [](Engine* e) -> entt::registry* { 
                auto scene = e->GetSceneStack().Active();
                if(!scene) return nullptr;
                return &scene->GetRegistry(); 
            },
            "get_input", [](Engine* e) { return &e->GetInput(); },
            "get_audio", [](Engine* e) { return &e->GetAudio(); },
            "get_time", [](Engine* e) { return &e->GetTime(); }
        );

        m_lua["Engine"] = &Engine::Get();

        m_lua.new_usertype<EngineTime>("EngineTime",
            "dt", &EngineTime::deltaTime,
            "total", &EngineTime::totalTime
        );

        // --- SCENE SERIALIZATION ---
        m_lua.set_function("save_scene", [](const std::string& path) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (scene) SceneSerializer::SaveToFile(*scene, path);
        });
        m_lua.set_function("load_scene", [](const std::string& path) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (scene) SceneSerializer::LoadFromFile(*scene, path);
        });
    }

    void ScriptSystem::ExecuteFile(const std::string& path) {
        auto result = m_lua.safe_script_file(path, sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            Log::Error("Lua Error in " + path + ": " + std::string(err.what()));
        }
    }

    void ScriptSystem::Update(float dt) {
        sol::protected_function updateFunc = m_lua["Update"];
        if (updateFunc.valid()) {
            auto result = updateFunc(dt);
            if (!result.valid()) {
                sol::error err = result;
                Log::Error("Lua Update Error: " + std::string(err.what()));
            }
        }
    }
}
