#include "ScriptSystem.hpp"
#include "PathResolver.hpp"
#include "VFSSystem.hpp"
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include "FightingSystem.hpp"
#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>
#include "AssetLoader.hpp"
#include "AudioSystem.hpp"
#include "Components.hpp"
#include "DashboardSystem.hpp"
#include "Engine.hpp"
#include "InputSystem.hpp"
#include "Log.hpp"
#include "PhysicsSystem.hpp"
#include "Renderer.hpp"
#include "Renderer2D.hpp"
#include "VFXSystem.hpp"
#include "imgui.h"
#include "CVarSystem.hpp"
#include "EventBroker.hpp"
#include "ConfigSystem.hpp"
#include "PrefabSystem.hpp"
#include "Tween.hpp"
#include "PluginSystem.hpp"
#include "EditorSystem.hpp"
#include "Noise.hpp"
namespace starlight {

// Helper: Get active scene's registry safely (reduces boilerplate in all bindings)
static inline entt::registry* GetActiveReg() {
    auto scene = Engine::Get().GetSceneStack().Active();
    return scene ? &scene->GetRegistry() : nullptr;
}

static inline bool ValidEntity(entt::registry* reg, uint32_t e) {
    return reg && reg->valid((entt::entity)e);
}

ScriptSystem::ScriptSystem() {
    m_lua.open_libraries(
        sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::package, sol::lib::debug, sol::lib::io);
}

ScriptSystem::~ScriptSystem() {}

bool ScriptSystem::OnInitialize(const EngineContext& context) {
    (void)context;

    // Set standard error handler
    m_lua.set_exception_handler(
        [](lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) {
            (void)maybe_exception;
            Log::Error("[Lua Exception] {}", description);
            return sol::stack::push(L, description);
        });
    // --- MATH TYPES ---
    m_lua.new_usertype<glm::vec2>(
        "vec2", sol::constructors<glm::vec2(float, float)>(), "x", &glm::vec2::x, "y", &glm::vec2::y);
    m_lua.new_usertype<glm::vec3>("vec3",
        sol::constructors<glm::vec3(float, float, float)>(),
        "x",
        &glm::vec3::x,
        "y",
        &glm::vec3::y,
        "z",
        &glm::vec3::z);
    m_lua.new_usertype<glm::quat>("quat",
        sol::constructors<glm::quat(float, float, float, float)>(),
        "x",
        &glm::quat::x,
        "y",
        &glm::quat::y,
        "z",
        &glm::quat::z,
        "w",
        &glm::quat::w);

    // --- NOISE GENERATION ---
    m_lua.new_usertype<Noise>("Noise",
        sol::constructors<Noise(), Noise(unsigned int)>(),
        "perlin", &Noise::Perlin,
        "octave_perlin", &Noise::OctavePerlin,
        "simplex", sol::overload(
            [](const Noise& n, float x, float y) { return n.Simplex(x, y); },
            [](const Noise& n, float x, float y, float z) { return n.Simplex(x, y, z); }
        ),
        "cellular", sol::overload(
            [](const Noise& n, float x, float y) { return n.Cellular(x, y); },
            [](const Noise& n, float x, float y, float z) { return n.Cellular(x, y, z); }
        ),
        "value", sol::overload(
            [](const Noise& n, float x, float y) { return n.Value(x, y); },
            [](const Noise& n, float x, float y, float z) { return n.Value(x, y, z); }
        ),
        "get_noise", sol::overload(
            [](const Noise& n, float x, float y) { return n.GetNoise(x, y); },
            [](const Noise& n, float x, float y, float z) { return n.GetNoise(x, y, z); }
        ),
        "set_seed", &Noise::SetSeed,
        "set_frequency", &Noise::SetFrequency,
        "set_fractal_type", &Noise::SetFractalType,
        "set_fractal_octaves", &Noise::SetFractalOctaves,
        "set_fractal_gain", &Noise::SetFractalGain,
        "set_fractal_lacunarity", &Noise::SetFractalLacunarity
    );

    // --- THE NUCLEAR ENGINE TABLE ---
    // Instead of usertypes for Core, we use a direct function table for 100% stability.
    auto engine = m_lua.create_table("Engine");

    engine["report_active_bt_node"] = [](const std::string& name) {
        if (!Engine::IsInitialized()) return;
        auto editor = Engine::Get().GetSystem<EditorSystem>();
        if (editor) {
            editor->ReportActiveBtNode(name);
        }
    };

    // Registry
    engine["instantiate_prefab"] = [](const std::string& path) {
        return (uint32_t)PrefabSystem::Instantiate(path);
    };

    engine["tween_position"] = [](uint32_t e, float startX, float startY, float startZ, float endX, float endY, float endZ, float duration, const std::string& easeName) {
        if (!Engine::IsInitialized()) return;
        auto tweenSys = Engine::Get().GetSystem<TweenSystem>();
        if (!tweenSys) return;

        EcsTween et;
        et.Start(
            (entt::entity)e,
            EcsTween::Type::Position,
            glm::vec3(startX, startY, startZ),
            glm::vec3(endX, endY, endZ),
            duration,
            Easing::GetEaseFunc(easeName)
        );
        tweenSys->AddEcsTween(et);
    };

    engine["tween_scale"] = [](uint32_t e, float startX, float startY, float startZ, float endX, float endY, float endZ, float duration, const std::string& easeName) {
        if (!Engine::IsInitialized()) return;
        auto tweenSys = Engine::Get().GetSystem<TweenSystem>();
        if (!tweenSys) return;

        EcsTween et;
        et.Start(
            (entt::entity)e,
            EcsTween::Type::Scale,
            glm::vec3(startX, startY, startZ),
            glm::vec3(endX, endY, endZ),
            duration,
            Easing::GetEaseFunc(easeName)
        );
        tweenSys->AddEcsTween(et);
    };

    engine["spawn"] = [](const std::string& tag) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene) {
            Log::Error("Script Error: Attempted to spawn '{}' but no active scene exists!", tag);
            return (uint32_t)0;
        }
        auto& reg = scene->GetRegistry();
        auto e = reg.create();
        reg.emplace<TagComponent>(e, tag);
        reg.emplace<TransformComponent>(e);
        auto& m = reg.emplace<MeshComponent>(e);
        m.material.isPBR = true;
        m.mesh = Engine::Get().GetRenderer().GetCubeMesh();
        return (uint32_t)e;
    };
    engine["set_pos"] = [](uint32_t e, float x, float y, float z) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<TransformComponent>((entt::entity)e))
            return;
        auto& t = reg.get<TransformComponent>((entt::entity)e);
        t.position = {x, y, z};
    };
    engine["spawn_light"] = [](float x, float y, float z, float r, float g, float b, float intensity) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return (uint32_t)0;
        auto& reg = scene->GetRegistry();
        auto e = reg.create();
        auto& t = reg.emplace<TransformComponent>(e);
        t.position = {x, y, z};
        auto& l = reg.emplace<PointLightComponent>(e);
        l.color = {r, g, b};
        l.intensity = intensity;
        return (uint32_t)e;
    };
    engine["rotate"] = [](uint32_t e, float x, float y, float z) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<TransformComponent>((entt::entity)e))
            return;
        auto& t = reg.get<TransformComponent>((entt::entity)e);
        t.rotation = t.rotation * glm::quat(glm::vec3(x, y, z));
    };
    engine["set_scale"] = [](uint32_t e, float x, float y, float z) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<TransformComponent>((entt::entity)e))
            return;
        auto& t = reg.get<TransformComponent>((entt::entity)e);
        t.scale = {x, y, z};
    };
    engine["set_color"] = [](uint32_t e, float r, float g, float b) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<MeshComponent>((entt::entity)e))
            return;
        auto& m = reg.get<MeshComponent>((entt::entity)e);
        m.material.color = {r, g, b};
    };
    engine["set_material"] = [](uint32_t e, float m, float r) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<MeshComponent>((entt::entity)e))
            return;
        auto& mesh = reg.get<MeshComponent>((entt::entity)e);
        mesh.material.metallic = m;
        mesh.material.roughness = r;
    };

    // Plugins
    engine["load_plugin"] = [](const std::string& path) {
        if (!Engine::IsInitialized()) return false;
        auto pluginSys = Engine::Get().GetSystem<PluginSystem>();
        return pluginSys ? pluginSys->LoadPlugin(path) : false;
    };
    engine["unload_plugin"] = []() {
        if (!Engine::IsInitialized()) return;
        auto pluginSys = Engine::Get().GetSystem<PluginSystem>();
        if (pluginSys) pluginSys->UnloadPlugin();
    };
    engine["hot_reload_plugin"] = []() {
        if (!Engine::IsInitialized()) return;
        auto pluginSys = Engine::Get().GetSystem<PluginSystem>();
        if (pluginSys) pluginSys->HotReload();
    };

    // Gameplay Controllers (Character, Foot IK, Nav Agent)
    engine["set_char_velocity"] = [](uint32_t e, float x, float y, float z) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene) return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<CharacterControllerComponent>((entt::entity)e)) return;
        auto& comp = reg.get<CharacterControllerComponent>((entt::entity)e);
        comp.velocity = {x, y, z};
    };
    engine["is_char_grounded"] = [](uint32_t e) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene) return false;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<CharacterControllerComponent>((entt::entity)e)) return false;
        return reg.get<CharacterControllerComponent>((entt::entity)e).isGrounded;
    };
    engine["set_foot_ik_enabled"] = [](uint32_t e, bool enabled) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene) return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<FootIKComponent>((entt::entity)e)) return;
        reg.get<FootIKComponent>((entt::entity)e).enabled = enabled;
    };
    engine["set_agent_target"] = [](uint32_t e, float x, float y, float z) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene) return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<NavAgentComponent>((entt::entity)e)) return;
        auto& comp = reg.get<NavAgentComponent>((entt::entity)e);
        comp.target = {x, y, z};
    };
    engine["set_agent_speed"] = [](uint32_t e, float speed) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene) return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<NavAgentComponent>((entt::entity)e)) return;
        reg.get<NavAgentComponent>((entt::entity)e).maxSpeed = speed;
    };

    // Input
    engine["is_down"] = [](const std::string& key) { return Engine::Get().GetInput().IsActionPressed(key); };

    // Time
    engine["get_dt"] = []() { return Engine::Get().GetTime().deltaTime; };
    engine["get_time"] = []() { return Engine::Get().GetTime().totalTime; };

    // --- NEW LIFE CYCLE, WINDOW & ECS BINDINGS ---
    engine["quit"] = []() { Engine::Get().RequestQuit(); };
    engine["get_fps"] = []() { return Engine::Get().GetTime().fps; };
    engine["set_pause"] = [](bool pause) { Engine::Get().SetPaused(pause); };
    engine["is_paused"] = []() { return Engine::Get().IsPaused(); };
    engine["set_title"] = [](const std::string& title) { Engine::Get().GetWindow().SetTitle(title); };
    engine["set_fullscreen"] = [](bool fullscreen) { Engine::Get().GetWindow().SetFullscreen(fullscreen); };
    engine["toggle_fullscreen"] = []() { Engine::Get().GetWindow().ToggleFullscreen(); };
    engine["get_entity_count"] = []() {
        auto scene = Engine::Get().GetSceneStack().Active();
        return scene ? (uint32_t)scene->GetRegistry().storage<entt::entity>().size() : 0;
    };
    engine["find_by_tag"] = [](const std::string& tag) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (scene) {
            auto view = scene->GetRegistry().view<TagComponent>();
            for (auto e : view) {
                if (view.get<TagComponent>(e).tag == tag) {
                    return (uint32_t)e;
                }
            }
        }
        return (uint32_t)0;
    };
    engine["set_rotation"] = [](uint32_t e, float rx, float ry, float rz) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<TransformComponent>((entt::entity)e))
            return;
        auto& t = reg.get<TransformComponent>((entt::entity)e);
        t.rotation = glm::quat(glm::vec3(rx, ry, rz));
    };
    engine["get_scale"] = [](uint32_t e, sol::this_state s) -> sol::variadic_results {
        sol::variadic_results res;
        sol::state_view lua(s);
        auto scene = Engine::Get().GetSceneStack().Active();
        if (scene) {
            auto& reg = scene->GetRegistry();
            if (reg.valid((entt::entity)e) && reg.all_of<TransformComponent>((entt::entity)e)) {
                auto& t = reg.get<TransformComponent>((entt::entity)e);
                res.push_back({lua, sol::in_place, t.scale.x});
                res.push_back({lua, sol::in_place, t.scale.y});
                res.push_back({lua, sol::in_place, t.scale.z});
                return res;
            }
        }
        return res;
    };
    engine["set_visible"] = [](uint32_t e, bool visible) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e))
            return;
        if (reg.all_of<MeshComponent>((entt::entity)e)) {
            reg.get<MeshComponent>((entt::entity)e).isVisible = visible;
        }
        if (reg.all_of<SpriteComponent>((entt::entity)e)) {
            reg.get<SpriteComponent>((entt::entity)e).visible = visible;
        }
    };

    engine["set_bloom"] = [](float threshold, int steps) {
        auto& r = Engine::Get().GetRenderer();
        r.SetBloomThreshold(threshold);
        r.SetBloomBlurSteps(steps);
    };
    engine["set_exposure"] = [](float exposure, float gamma) {
        auto& r = Engine::Get().GetRenderer();
        r.SetExposure(exposure);
        r.SetGamma(gamma);
    };
    engine["set_graphics_preset"] = [](int preset) {
        Engine::Get().GetRenderer().SetGraphicsPreset(preset);
    };
    engine["get_graphics_preset"] = []() {
        return Engine::Get().GetRenderer().GetGraphicsPreset();
    };

    // Graphics
    engine["set_camera_pos"] = [](float x, float y, float z) {
        Engine::Get().GetRenderer().GetCameraTransform().position = {x, y, z};
    };
    engine["look_at"] = [](float x, float y, float z) {
        auto& t = Engine::Get().GetRenderer().GetCameraTransform();
        glm::mat4 view = glm::lookAt(t.position, glm::vec3(x, y, z), glm::vec3(0, 1, 0));
        glm::mat4 invView = glm::inverse(view);
        t.rotation = glm::quat_cast(invView);
    };

    // --- 3D MOUSE RAYCASTING API ---
    engine["get_mouse_hit"] = [](float planeY, sol::this_state s) -> sol::variadic_results {
        sol::variadic_results res;
        sol::state_view lua(s);

        auto& window = Engine::Get().GetWindow();
        int width = window.GetWidth();
        int height = window.GetHeight();

        auto mPos = Engine::Get().GetInput().GetMousePosition();
        float mx = mPos.x;
        float my = mPos.y;

        // Normalize device coordinates
        float x = (2.0f * mx) / width - 1.0f;
        float y = 1.0f - (2.0f * my) / height;

        auto& renderer = Engine::Get().GetRenderer();
        glm::mat4 proj = renderer.GetProjectionMatrix();
        glm::mat4 view = renderer.GetViewMatrix();
        glm::mat4 invVP = glm::inverse(proj * view);

        glm::vec4 rayClipNear(x, y, -1.0f, 1.0f);
        glm::vec4 rayClipFar(x, y, 1.0f, 1.0f);

        glm::vec4 rayWorldNear = invVP * rayClipNear;
        rayWorldNear /= rayWorldNear.w;

        glm::vec4 rayWorldFar = invVP * rayClipFar;
        rayWorldFar /= rayWorldFar.w;

        glm::vec3 rayOrigin = glm::vec3(rayWorldNear);
        glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorldFar) - rayOrigin);

        // Intersect with plane Y = planeY
        if (std::abs(rayDir.y) > 0.0001f) {
            float t = (planeY - rayOrigin.y) / rayDir.y;
            if (t >= 0.0f) {
                glm::vec3 hit = rayOrigin + rayDir * t;
                res.push_back({lua, sol::in_place, hit.x});
                res.push_back({lua, sol::in_place, hit.z});
                return res;
            }
        }
        return res;
    };

    // Audio
    engine["play_sound"] = [](const std::string& path) { Engine::Get().GetAudio().PlayEffect(path); };
    engine["play_music"] = [](const std::string& path, bool loop, float volume) {
        Engine::Get().GetAudio().PlayMusic(path, loop, volume);
    };
    engine["stop_music"] = []() {
        Engine::Get().GetAudio().StopMusic();
    };
    engine["set_music_volume"] = [](float volume) {
        Engine::Get().GetAudio().SetMusicVolume(volume);
    };
    engine["set_effects_volume"] = [](float volume) {
        Engine::Get().GetAudio().SetEffectsVolume(volume);
    };
    engine["play_note"] = [](float freq, float duration, int type) {
        Engine::Get().GetAudio().PlayNote(freq, duration, static_cast<WaveType>(type));
    };

    // --- PHYSICS API (Phase 11) ---
    engine["apply_force"] = [](uint32_t e, float fx, float fy, float fz) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        Engine::Get().GetPhysics().ApplyForce((entt::entity)e, glm::vec3(fx, fy, fz));
    };
    engine["apply_impulse"] = [](uint32_t e, float ix, float iy, float iz) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        Engine::Get().GetPhysics().ApplyImpulse((entt::entity)e, glm::vec3(ix, iy, iz));
    };
    engine["set_velocity"] = [](uint32_t e, float vx, float vy, float vz) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        Engine::Get().GetPhysics().SetVelocity((entt::entity)e, glm::vec3(vx, vy, vz));
    };

    // --- ENTITY MANAGEMENT (Phase 11) ---
    engine["destroy"] = [](uint32_t e) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        auto& reg = scene->GetRegistry();
        if (reg.valid((entt::entity)e)) {
            reg.destroy((entt::entity)e);
        }
    };
    engine["get_pos"] = [](uint32_t e, sol::this_state s) -> sol::variadic_results {
        sol::variadic_results res;
        sol::state_view lua(s);
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return res;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<TransformComponent>((entt::entity)e))
            return res;
        auto& t = reg.get<TransformComponent>((entt::entity)e);
        res.push_back({lua, sol::in_place, t.position.x});
        res.push_back({lua, sol::in_place, t.position.y});
        res.push_back({lua, sol::in_place, t.position.z});
        return res;
    };

    engine["set_light_color"] = [](uint32_t e, float r, float g, float b) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<PointLightComponent>((entt::entity)e))
            return;
        auto& l = reg.get<PointLightComponent>((entt::entity)e);
        l.color = {r, g, b};
    };

    engine["set_light_intensity"] = [](uint32_t e, float intensity) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<PointLightComponent>((entt::entity)e))
            return;
        auto& l = reg.get<PointLightComponent>((entt::entity)e);
        l.intensity = intensity;
    };

    engine["get_window_size"] = [](sol::this_state s) -> sol::variadic_results {
        sol::variadic_results res;
        sol::state_view lua(s);
        auto& window = Engine::Get().GetWindow();
        res.push_back({lua, sol::in_place, window.GetWidth()});
        res.push_back({lua, sol::in_place, window.GetHeight()});
        return res;
    };

    // --- PBR MATERIAL CONTROL (Phase 11) ---
    engine["set_pbr"] = [](uint32_t e, float met, float rough, float ambientOcc) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e))
            return;
        if (!reg.all_of<MeshComponent>((entt::entity)e))
            return;
        auto& m = reg.get<MeshComponent>((entt::entity)e);
        m.material.metallic = met;
        m.material.roughness = rough;
        m.material.ao = ambientOcc;
    };

    // --- REVERB ZONE CONTROL ---
    engine["add_reverb_zone"] = [](uint32_t e, float minDistance, float maxDistance, float factor) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene) return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e)) return;
        
        ReverbZoneComponent rzc;
        rzc.minDistance = minDistance;
        rzc.maxDistance = maxDistance;
        rzc.reverbFactor = factor;
        rzc.active = true;
        
        reg.emplace_or_replace<ReverbZoneComponent>((entt::entity)e, rzc);
    };

    engine["remove_reverb_zone"] = [](uint32_t e) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene) return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e)) return;
        if (reg.all_of<ReverbZoneComponent>((entt::entity)e)) {
            reg.remove<ReverbZoneComponent>((entt::entity)e);
        }
    };

    engine["set_reverb_zone_factor"] = [](uint32_t e, float factor) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene) return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e)) return;
        if (reg.all_of<ReverbZoneComponent>((entt::entity)e)) {
            auto& rzc = reg.get<ReverbZoneComponent>((entt::entity)e);
            rzc.reverbFactor = factor;
        }
    };

    engine["set_reverb_zone_bounds"] = [](uint32_t e, float minDistance, float maxDistance) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene) return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e)) return;
        if (reg.all_of<ReverbZoneComponent>((entt::entity)e)) {
            auto& rzc = reg.get<ReverbZoneComponent>((entt::entity)e);
            rzc.minDistance = minDistance;
            rzc.maxDistance = maxDistance;
        }
    };

    engine["has_reverb_zone"] = [](uint32_t e) -> bool {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene) return false;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e)) return false;
        return reg.all_of<ReverbZoneComponent>((entt::entity)e);
    };

    engine["get_reverb_zone"] = [](uint32_t e, sol::this_state s) -> sol::variadic_results {
        sol::variadic_results res;
        sol::state_view lua(s);
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene) return res;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e)) return res;
        if (reg.all_of<ReverbZoneComponent>((entt::entity)e)) {
            auto& rzc = reg.get<ReverbZoneComponent>((entt::entity)e);
            res.push_back({lua, sol::in_place, rzc.minDistance});
            res.push_back({lua, sol::in_place, rzc.maxDistance});
            res.push_back({lua, sol::in_place, rzc.reverbFactor});
            res.push_back({lua, sol::in_place, rzc.active});
        }
        return res;
    };

    // --- LOGGING (Phase 11) ---
    engine["log"] = [](const std::string& msg) { Log::Info("[Lua] {}", msg); };
    engine["log_warn"] = [](const std::string& msg) { Log::Warn("[Lua] {}", msg); };
    engine["log_error"] = [](const std::string& msg) { Log::Error("[Lua] {}", msg); };

    // --- CVAR SYSTEM LUA API ---
    auto cvar = m_lua.create_table("cvar");
    cvar["get"] = [](const std::string& name) -> sol::object {
        auto sys = Engine::Get().GetSystem<CVarSystem>();
        if (!sys || !sys->Exists(name)) return sol::nil;
        CVarType t = sys->GetType(name);
        sol::state_view lua(Engine::Get().GetScripting().GetLua());
        if (t == CVarType::Int) return sol::make_object(lua, sys->GetInt(name));
        else if (t == CVarType::Float) return sol::make_object(lua, sys->GetFloat(name));
        else if (t == CVarType::Bool) return sol::make_object(lua, sys->GetBool(name));
        else if (t == CVarType::String) return sol::make_object(lua, sys->GetString(name));
        return sol::nil;
    };
    cvar["set"] = [](const std::string& name, sol::object value) {
        auto sys = Engine::Get().GetSystem<CVarSystem>();
        if (!sys || !sys->Exists(name)) return;
        CVarType t = sys->GetType(name);
        if (t == CVarType::Int && value.is<int>()) {
            sys->SetInt(name, value.as<int>());
        } else if (t == CVarType::Float && value.is<float>()) {
            sys->SetFloat(name, value.as<float>());
        } else if (t == CVarType::Bool && value.is<bool>()) {
            sys->SetBool(name, value.as<bool>());
        } else if (t == CVarType::String && value.is<std::string>()) {
            sys->SetString(name, value.as<std::string>());
        } else {
            sys->SetFromString(name, value.as<std::string>());
        }
    };
    cvar["list"] = []() -> sol::table {
        sol::state_view lua(Engine::Get().GetScripting().GetLua());
        sol::table t = lua.create_table();
        auto sys = Engine::Get().GetSystem<CVarSystem>();
        if (sys) {
            for (const auto& cv : sys->GetCVars()) {
                sol::table entry = lua.create_table();
                entry["description"] = cv.description;
                if (cv.type == CVarType::Int) {
                    entry["type"] = "Int";
                    entry["value"] = std::get<int>(cv.value);
                } else if (cv.type == CVarType::Float) {
                    entry["type"] = "Float";
                    entry["value"] = std::get<float>(cv.value);
                } else if (cv.type == CVarType::Bool) {
                    entry["type"] = "Bool";
                    entry["value"] = std::get<bool>(cv.value);
                } else if (cv.type == CVarType::String) {
                    entry["type"] = "String";
                    entry["value"] = std::get<std::string>(cv.value);
                }
                t[cv.name] = entry;
            }
        }
        return t;
    };

    // Metatable for direct indexing of CVars: cvar.r_exposure = 1.5
    sol::table cvar_mt = m_lua.create_table();
    cvar_mt["__index"] = [](sol::table t, const std::string& key) -> sol::object {
        (void)t;
        auto sys = Engine::Get().GetSystem<CVarSystem>();
        if (!sys || !sys->Exists(key)) return sol::nil;
        CVarType type = sys->GetType(key);
        sol::state_view lua(Engine::Get().GetScripting().GetLua());
        if (type == CVarType::Int) return sol::make_object(lua, sys->GetInt(key));
        else if (type == CVarType::Float) return sol::make_object(lua, sys->GetFloat(key));
        else if (type == CVarType::Bool) return sol::make_object(lua, sys->GetBool(key));
        else if (type == CVarType::String) return sol::make_object(lua, sys->GetString(key));
        return sol::nil;
    };
    cvar_mt["__newindex"] = [](sol::table t, const std::string& key, sol::object value) {
        (void)t;
        auto sys = Engine::Get().GetSystem<CVarSystem>();
        if (!sys || !sys->Exists(key)) return;
        CVarType type = sys->GetType(key);
        if (type == CVarType::Int && value.is<int>()) {
            sys->SetInt(key, value.as<int>());
        } else if (type == CVarType::Float && value.is<float>()) {
            sys->SetFloat(key, value.as<float>());
        } else if (type == CVarType::Bool && value.is<bool>()) {
            sys->SetBool(key, value.as<bool>());
        } else if (type == CVarType::String && value.is<std::string>()) {
            sys->SetString(key, value.as<std::string>());
        } else {
            sys->SetFromString(key, value.as<std::string>());
        }
    };
    cvar[sol::metatable_key] = cvar_mt;

    // --- EVENT BROKER LUA API ---
    auto events = m_lua.create_table("events");
    events["subscribe"] = [](const std::string& eventName, sol::protected_function callback) {
        auto sys = Engine::Get().GetSystem<EventBroker>();
        if (sys) {
            sys->SubscribeLua(eventName, callback);
        }
    };
    events["publish"] = [](const std::string& eventName, sol::table data) {
        auto sys = Engine::Get().GetSystem<EventBroker>();
        if (sys) {
            sys->Publish(eventName, data);
        }
    };

    // --- CONSOLE CUSTOM LOGS API ---
    auto consoleTable = m_lua.create_table("console");
    consoleTable["add_log"] = [](const std::string& msg, const std::string& type) {
        if (type == "warn") {
            Log::Warn("{}", msg);
        } else if (type == "error") {
            Log::Error("{}", msg);
        } else if (type == "debug") {
            Log::Debug("{}", msg);
        } else {
            Log::Info("{}", msg);
        }
    };

    // --- CONFIG SYSTEM LUA API ---
    auto configTable = m_lua.create_table("config");
    configTable["get_string"] = [](const std::string& section, const std::string& key, const std::string& defaultVal) {
        auto sys = Engine::Get().GetSystem<ConfigSystem>();
        return sys ? sys->GetString(section, key, defaultVal) : defaultVal;
    };
    configTable["get_int"] = [](const std::string& section, const std::string& key, int defaultVal) {
        auto sys = Engine::Get().GetSystem<ConfigSystem>();
        return sys ? sys->GetInt(section, key, defaultVal) : defaultVal;
    };
    configTable["get_float"] = [](const std::string& section, const std::string& key, float defaultVal) {
        auto sys = Engine::Get().GetSystem<ConfigSystem>();
        return sys ? sys->GetFloat(section, key, defaultVal) : defaultVal;
    };
    configTable["get_bool"] = [](const std::string& section, const std::string& key, bool defaultVal) {
        auto sys = Engine::Get().GetSystem<ConfigSystem>();
        return sys ? sys->GetBool(section, key, defaultVal) : defaultVal;
    };
    configTable["set_string"] = [](const std::string& section, const std::string& key, const std::string& value) {
        auto sys = Engine::Get().GetSystem<ConfigSystem>();
        if (sys) sys->SetString(section, key, value);
    };
    configTable["set_int"] = [](const std::string& section, const std::string& key, int value) {
        auto sys = Engine::Get().GetSystem<ConfigSystem>();
        if (sys) sys->SetInt(section, key, value);
    };
    configTable["set_float"] = [](const std::string& section, const std::string& key, float value) {
        auto sys = Engine::Get().GetSystem<ConfigSystem>();
        if (sys) sys->SetFloat(section, key, value);
    };
    configTable["set_bool"] = [](const std::string& section, const std::string& key, bool value) {
        auto sys = Engine::Get().GetSystem<ConfigSystem>();
        if (sys) sys->SetBool(section, key, value);
    };
    configTable["save"] = [](const std::string& filepath) {
        auto sys = Engine::Get().GetSystem<ConfigSystem>();
        return sys ? sys->Save(filepath) : false;
    };

    // --- IMGUI ---
    auto imgui = m_lua.create_table("imgui");
    imgui["text"] = [](float x, float y, float r, float g, float b, const std::string& msg) {
        ImGui::GetForegroundDrawList()->AddText({x, y}, ImColor(r, g, b), msg.c_str());
    };
    imgui["rect"] = [](float x, float y, float w, float h, float r, float g, float b) {
        ImGui::GetForegroundDrawList()->AddRectFilled({x, y}, {x + w, y + h}, ImColor(r, g, b));
    };

    // --- DASHBOARD UI (Non-ImGui) ---
    auto ui = m_lua.create_table("ui");
    ui["begin"] = [](int w, int h) {
        auto dash = Engine::Get().GetSystem<DashboardSystem>();
        if (dash)
            dash->Begin(w, h);
    };
    ui["panel"] = [](float x, float y, float w, float h, float r, float g, float b, float a) {
        auto dash = Engine::Get().GetSystem<DashboardSystem>();
        if (dash)
            dash->Panel(x, y, w, h, {r, g, b, a});
    };
    ui["label"] = [](const std::string& text, float x, float y, float r, float g, float b, float a) {
        auto dash = Engine::Get().GetSystem<DashboardSystem>();
        if (dash)
            dash->Label(text, x, y, {r, g, b, a});
    };
    ui["button"] = [](const std::string& label, float x, float y, float w, float h) {
        auto dash = Engine::Get().GetSystem<DashboardSystem>();
        if (dash)
            return dash->Button(label, x, y, w, h);
        return false;
    };
    ui["finish"] = []() {
        auto dash = Engine::Get().GetSystem<DashboardSystem>();
        if (dash)
            dash->End(Engine::Get().GetRenderer());
    };

    // --- ENGINE 2D RENDERER (Renderer2D batched quads) ---
    auto gfx = m_lua.create_table("gfx");
    gfx["draw_quad"] = [](float x, float y, float w, float h, float r, float g, float b, float a) {
        Renderer2D::DrawQuad({x, y}, {w, h}, {r, g, b, a});
    };
    gfx["draw_sprite"] = [](float x, float y, float w, float h, uint32_t texID, float r, float g, float b, float a) {
        Renderer2D::DrawQuad({x, y}, {w, h}, texID, {r, g, b, a});
    };
    gfx["draw_sprite_region"] = [](float x, float y, float w, float h, uint32_t texID,
                                   float srcX, float srcY, float srcW, float srcH,
                                   float texW, float texH,
                                   float r, float g, float b, float a) {
        float u0 = srcX / texW;
        float v0 = srcY / texH;
        float u1 = (srcX + srcW) / texW;
        float v1 = srcY / texH;
        float u2 = (srcX + srcW) / texW;
        float v2 = (srcY + srcH) / texH;
        float u3 = srcX / texW;
        float v3 = (srcY + srcH) / texH;
        Renderer2D::DrawQuad({x, y}, {w, h}, texID, {u0, v0}, {u1, v1}, {u2, v2}, {u3, v3}, {r, g, b, a});
    };
    gfx["draw_iso_tile"] = [](float x, float y, float w, float h, uint32_t texID, float r, float g, float b, float a) {
        Renderer2D::DrawQuad({x, y}, {w, h}, texID, {r, g, b, a});
    };
    gfx["draw_sprite_clean"] =
        [](float x, float y, float w, float h, uint32_t texID, float r, float g, float b, float a) {
            Renderer2D::DrawQuad({x, y}, {w, h}, texID, {r, g, b, a});
        };
    gfx["get_stats"] = [this]() {
        auto stats = Renderer2D::GetStats();
        sol::table s = m_lua.create_table();
        s["drawCalls"] = stats.drawCalls;
        s["quadCount"] = stats.quadCount;
        return s;
    };
    gfx["draw_rect"] = [](float x, float y, float w, float h, float r, float g, float b, sol::optional<float> a) {
        Renderer2D::DrawQuad({x, y}, {w, h}, {r, g, b, a.value_or(1.0f)});
    };
    gfx["draw_rect_alpha"] = [](float x, float y, float w, float h, float r, float g, float b, float a) {
        Renderer2D::DrawQuad({x, y}, {w, h}, {r, g, b, a});
    };
    gfx["draw_text"] = [](const std::string& text, float x, float y, float scale, float r, float g, float b, float a, sol::optional<bool> isSDF) {
        Renderer2D::DrawString(text, {x, y}, scale, {r, g, b, a}, isSDF.value_or(false));
    };
    // Line: drawn as a thin quad between two points
    gfx["draw_line"] =
        [](float x1, float y1, float x2, float y2, float thickness, float r, float g, float b, sol::optional<float> a) {
            float dx = x2 - x1, dy = y2 - y1;
            float len = std::sqrt(dx * dx + dy * dy);
            if (len < 0.001f)
                return;
            float cx = (x1 + x2) * 0.5f, cy = (y1 + y2) * 0.5f;
            Renderer2D::DrawQuad(
                {cx - len * 0.5f, cy - thickness * 0.5f}, {len, thickness}, {r, g, b, a.value_or(1.0f)});
        };
    // Rect outline: 4 thin quads
    gfx["draw_rect_outline"] =
        [](float x, float y, float w, float h, float t, float r, float g, float b, sol::optional<float> a) {
            glm::vec4 c = {r, g, b, a.value_or(1.0f)};
            Renderer2D::DrawQuad({x, y}, {w, t}, c);                      // top
            Renderer2D::DrawQuad({x, y + h - t}, {w, t}, c);              // bottom
            Renderer2D::DrawQuad({x, y + t}, {t, h - 2 * t}, c);          // left
            Renderer2D::DrawQuad({x + w - t, y + t}, {t, h - 2 * t}, c);  // right
        };
    // Circle: approximated with N quads around center
    gfx["draw_circle"] = [](float cx,
                             float cy,
                             float radius,
                             float r,
                             float g,
                             float b,
                             sol::optional<int> segments,
                             sol::optional<float> a) {
        int seg = segments.value_or(24);
        float step = 2.0f * 3.14159265f / (float)seg;
        float thickness = 2.0f;
        for (int i = 0; i < seg; i++) {
            float a1 = step * i, a2 = step * (i + 1);
            float x1 = cx + std::cos(a1) * radius, y1 = cy + std::sin(a1) * radius;
            float x2 = cx + std::cos(a2) * radius, y2 = cy + std::sin(a2) * radius;
            float mx = (x1 + x2) * 0.5f - thickness * 0.5f;
            float my = (y1 + y2) * 0.5f - thickness * 0.5f;
            float dx = x2 - x1, dy = y2 - y1;
            float len = std::sqrt(dx * dx + dy * dy);
            Renderer2D::DrawQuad({mx, my}, {len, thickness}, {r, g, b, a.value_or(1.0f)});
        }
    };
    // Circle filled: concentric rings of quads
    gfx["draw_circle_filled"] =
        [](float cx, float cy, float radius, float r, float g, float b, sol::optional<float> a) {
            int seg = 24;
            float step = 2.0f * 3.14159265f / (float)seg;
            for (int i = 0; i < seg; i++) {
                float a1 = step * i, a2 = step * (i + 1);
                float x1 = cx + std::cos(a1) * radius;
                float y1 = cy + std::sin(a1) * radius;
                float x2 = cx + std::cos(a2) * radius;
                float y2 = cy + std::sin(a2) * radius;
                // Triangle fan via thin quad from center to edge
                float mx = std::min({cx, x1, x2}), my = std::min({cy, y1, y2});
                float Mx = std::max({cx, x1, x2}), My = std::max({cy, y1, y2});
                Renderer2D::DrawQuad({mx, my}, {Mx - mx, My - my}, {r, g, b, a.value_or(0.5f)});
            }
        };
    // Screen dimensions shortcut
    gfx["screen_width"] = []() { return (float)Engine::Get().GetWindow().GetWidth(); };
    gfx["screen_height"] = []() { return (float)Engine::Get().GetWindow().GetHeight(); };

    gfx["set_clear_color"] = [](float r, float g, float b) { Engine::Get().GetRenderer().SetClearColor({r, g, b}); };
    gfx["draw_triangle"] =
        [](float x1, float y1, float x2, float y2, float x3, float y3, float r, float g, float b, float a) {
            Renderer2D::DrawTriangle({x1, y1}, {x2, y2}, {x3, y3}, {r, g, b, a});
        };
    gfx["draw_rounded_rect"] =
        [](float x, float y, float w, float h, float radius, float r, float g, float b, float a) {
            Renderer2D::DrawRoundedRect({x, y}, {w, h}, radius, {r, g, b, a});
        };
    gfx["set_clip_rect"] = [](float x, float y, float w, float h) { Renderer2D::SetClipRect({x, y, w, h}); };
    gfx["clear_clip_rect"] = []() { Renderer2D::ClearClipRect(); };
    gfx["set_camera_2d"] = [](float x, float y, float zoom) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        auto& reg = scene->GetRegistry();
        auto view = reg.view<Camera2DComponent>();
        entt::entity camEnt = entt::null;
        for (auto entity : view) {
            if (view.get<Camera2DComponent>(entity).primary) {
                camEnt = entity;
                break;
            }
        }
        if (camEnt == entt::null) {
            camEnt = reg.create();
            reg.emplace<TransformComponent>(camEnt);
            reg.emplace<Camera2DComponent>(camEnt, Camera2DComponent{zoom, 0.0f, true});
            reg.emplace<TagComponent>(camEnt, TagComponent{"Camera2D"});
        }
        auto& trans = reg.get<TransformComponent>(camEnt);
        trans.position = {x, y, 0.0f};
        auto& cam = reg.get<Camera2DComponent>(camEnt);
        cam.zoom = zoom;
    };

    gfx["draw_pbr_cube"] = [](float x, float y, float z, float s, float r, float g, float b, float metal, float rough) {
        RenderCommand cmd;
        auto& renderer = Engine::Get().GetRenderer();
        cmd.mesh = renderer.GetCubeMesh();
        cmd.shader = renderer.GetPBRShader();
        cmd.transform = glm::translate(glm::mat4(1.0f), {x, y, z}) * glm::scale(glm::mat4(1.0f), {s, s, s});
        cmd.albedo = {r, g, b};
        cmd.metallic = metal;
        cmd.roughness = rough;
        cmd.ao = 1.0f;
        renderer.Submit(cmd);
    };

    gfx["draw_custom_shader_quad"] = [](float x, float y, float w, float h, float time) {
        static std::shared_ptr<Shader> s_customShader = nullptr;
        static std::filesystem::file_time_type s_lastShaderWriteTime;
        
        std::string shaderPath = PathResolver::ExeDir() + "/assets/shaders/custom_node_shader.frag";
        if (std::filesystem::exists(shaderPath)) {
            try {
                auto writeTime = std::filesystem::last_write_time(shaderPath);
                if (!s_customShader || writeTime != s_lastShaderWriteTime) {
                    s_lastShaderWriteTime = writeTime;
                    s_customShader = Shader::LoadFromFile("assets/shaders/basic.vert", "assets/shaders/custom_node_shader.frag");
                    if (s_customShader) {
                        Log::Info("[Shader Hot-Reload] Successfully loaded custom shader for Lua viewport!");
                    } else {
                        Log::Error("[Shader Hot-Reload] Failed to compile custom shader!");
                    }
                }
            } catch (...) {
                // File might be locked during writing, ignore this frame
            }
        }
        
        if (s_customShader) {
            // Save current OpenGL state to avoid breaking Renderer2D batching
            int last_program = 0;
            int last_vao = 0;
            int last_vbo = 0;
            int last_ebo = 0;
            glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vao);
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_vbo);
            glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_ebo);

            s_customShader->Use();
            auto& window = Engine::Get().GetWindow();
            
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 projection = glm::ortho(0.0f, (float)window.GetWidth(), (float)window.GetHeight(), 0.0f, -1.0f, 1.0f);
            
            s_customShader->SetMat4U("model", model);
            s_customShader->SetMat4U("view", view);
            s_customShader->SetMat4U("projection", projection);
            s_customShader->SetFloatU("uTime", time);
            
            float vertices[] = {
                x,     y + h, 0.0f, 0.0f, 0.0f,
                x + w, y + h, 0.0f, 1.0f, 0.0f,
                x + w, y,     0.0f, 1.0f, 1.0f,
                x,     y,     0.0f, 0.0f, 1.0f
            };
            unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
            
            static unsigned int VAO = 0, VBO = 0, EBO = 0;
            if (VAO == 0) {
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                glGenBuffers(1, &EBO);
                
                glBindVertexArray(VAO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
                
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
                
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);
            } else {
                glBindVertexArray(VAO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            }
            
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // Restore previously bound OpenGL state
            glUseProgram(last_program);
            glBindVertexArray(last_vao);
            glBindBuffer(GL_ARRAY_BUFFER, last_vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_ebo);
        }
    };

    gfx["reload_shaders"] = []() { Engine::Get().GetRenderer().ReloadAllShaders(); };
    gfx["set_bloom_threshold"] = [](float t) { Engine::Get().GetRenderer().SetBloomThreshold(t); };
    gfx["get_bloom_threshold"] = []() { return Engine::Get().GetRenderer().GetBloomThreshold(); };
    gfx["set_exposure"] = [](float e) { Engine::Get().GetRenderer().SetExposure(e); };
    gfx["get_exposure"] = []() { return Engine::Get().GetRenderer().GetExposure(); };
    gfx["set_gamma"] = [](float g) { Engine::Get().GetRenderer().SetGamma(g); };
    gfx["get_gamma"] = []() { return Engine::Get().GetRenderer().GetGamma(); };

    // --- CAMERA API ---
    auto camera = m_lua.create_table("camera");
    camera["set_pos"] = [](float x, float y, float z) {
        Engine::Get().GetRenderer().GetCameraTransform().position = {x, y, z};
    };
    camera["look_at"] = [](float x, float y, float z) { Engine::Get().GetRenderer().SetCameraLookAt({x, y, z}); };
    camera["set_fov"] = [](float fov) {
        auto& renderer = Engine::Get().GetRenderer();
        renderer.UpdateProjection(fov,
            (float)Engine::Get().GetWindow().GetWidth() / (float)Engine::Get().GetWindow().GetHeight(),
            0.1f,
            1000.0f);
    };
    camera["get_pos"] = [](sol::this_state s) -> sol::variadic_results {
        sol::variadic_results res;
        sol::state_view lua(s);
        auto pos = Engine::Get().GetRenderer().GetCameraTransform().position;
        res.push_back({lua, sol::in_place, pos.x});
        res.push_back({lua, sol::in_place, pos.y});
        res.push_back({lua, sol::in_place, pos.z});
        return res;
    };
    camera["get_rotation"] = [](sol::this_state s) -> sol::variadic_results {
        sol::variadic_results res;
        sol::state_view lua(s);
        auto rot = Engine::Get().GetRenderer().GetCameraTransform().rotation;
        res.push_back({lua, sol::in_place, rot.x});
        res.push_back({lua, sol::in_place, rot.y});
        res.push_back({lua, sol::in_place, rot.z});
        res.push_back({lua, sol::in_place, rot.w});
        return res;
    };

    // --- ASSET SYSTEM ---
    auto assets = m_lua.create_table("assets");
    assets["load_texture"] = [](const std::string& path, sol::optional<bool> removeCheckered) {
        return AssetLoader::LoadTexture(path, removeCheckered.value_or(false));
    };
    assets["create_iso_tile"] = [](float r, float g, float b) {
        return AssetLoader::CreateProceduralIsometricTile({r, g, b});
    };
    assets["create_building"] = [](float r, float g, float b) {
        return AssetLoader::CreateProceduralBuilding({r, g, b});
    };

    // --- WINDOW API ---
    auto window = m_lua.create_table("window");
    window["get_width"] = []() { return Engine::Get().GetWindow().GetWidth(); };
    window["get_height"] = []() { return Engine::Get().GetWindow().GetHeight(); };

    // --- AUDIO API ---
    auto audio = m_lua.create_table("audio");
    audio["play_sound"] = [](const std::string& path) { Engine::Get().GetAudio().PlayEffect(path); };
    audio["play_music"] = [](const std::string& path, sol::optional<bool> loop, sol::optional<float> vol) {
        Engine::Get().GetAudio().PlayMusic(path, loop.value_or(true), vol.value_or(1.0f));
    };
    audio["stop_music"] = []() { Engine::Get().GetAudio().StopMusic(); };
    audio["set_music_volume"] = [](float vol) { Engine::Get().GetAudio().SetMusicVolume(vol); };
    audio["is_music_playing"] = []() { return Engine::Get().GetAudio().IsMusicPlaying(); };

    // VFX
    auto vfx = m_lua.create_table("vfx");
    vfx["emit"] =
        [](float x, float y, float z, float vx, float vy, float vz, float r, float g, float b, int count, float size) {
            auto vfxSys = Engine::Get().GetSystem<VFXSystem>();
            if (vfxSys) {
                vfxSys->Emit({x, y, z}, {vx, vy, vz}, {r, g, b, 1.0f}, count, size);
            }
        };
    audio["play_3d"] = [](const std::string& path, float x, float y, float z) {
        Engine::Get().GetAudio().Play3DEffect(path, x, y, z);
    };
    audio["set_volume"] = [](float vol) { Engine::Get().GetAudio().SetMasterVolume(vol); };
    audio["beep"] = [](float freq, float duration, sol::optional<int> type) {
        Engine::Get().GetAudio().PlayNote(freq, duration, (WaveType)type.value_or(0));
    };
    audio["play_note"] = [](float freq, float duration, sol::optional<int> type) {
        Engine::Get().GetAudio().PlayNote(freq, duration, (WaveType)type.value_or(0));
    };
    audio["beep3d"] = [](float freq, float duration, float x, float y, float z, sol::optional<int> type) {
        Engine::Get().GetAudio().Play3DNote(freq, duration, x, y, z, (WaveType)type.value_or(0));
    };
    audio["play_note_3d"] = [](float freq, float duration, float x, float y, float z, sol::optional<int> type) {
        Engine::Get().GetAudio().Play3DNote(freq, duration, x, y, z, (WaveType)type.value_or(0));
    };
    audio["fm_note"] = [](float freq, float duration, sol::optional<int> algo) {
        Engine::Get().GetAudio().PlayFMNote(freq, duration, algo.value_or(0));
    };
    audio["set_low_pass"] = [](float cutoff) { Engine::Get().GetAudio().SetLowPassCutoff(cutoff); };
    audio["set_envelope"] = [](float attack, float decay, float sustain, float release) {
        Engine::Get().GetAudio().SetEnvelope(attack, decay, sustain, release);
    };

    // --- INPUT API ---
    auto input = m_lua.create_table("input");
    input["get_mouse_pos"] = []() { return Engine::Get().GetInput().GetMousePosition(); };
    input["get_mouse_x"] = []() { return Engine::Get().GetInput().GetMousePosition().x; };
    input["get_mouse_y"] = []() { return Engine::Get().GetInput().GetMousePosition().y; };
    input["is_down"] = [](const std::string& key) { return Engine::Get().GetInput().IsActionPressed(key); };
    input["is_just_pressed"] = [](const std::string& key) { return Engine::Get().GetInput().IsActionJustPressed(key); };
    input["get_axis"] = [](const std::string& axis) { return Engine::Get().GetInput().GetAxis(axis); };
    input["is_gamepad_down"] = [](const std::string& btn) {
        return Engine::Get().GetInput().IsGamepadButtonPressed(btn);
    };
    input["vibrate"] = [](float left, float right, uint32_t ms) { Engine::Get().GetInput().Vibrate(left, right, ms); };
    input["is_mouse_down"] = [](int button) {
        Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
        return (mouseState & SDL_BUTTON(button)) != 0;
    };
    input["get_scroll"] = []() { return Engine::Get().GetWindow().GetScrollDelta(); };

    // --- FILE API (Hardened for Industrial Security) ---
    auto file = m_lua.create_table("file");
    auto isPathSafe = [](const std::string& path) {
        // Simple check: don't allow ".." or absolute paths to system dirs
        if (path.find("..") != std::string::npos)
            return false;
        if (path.find(":") != std::string::npos)
            return false;  // No C:\ absolute
        return true;
    };

    file["read"] = [isPathSafe](const std::string& path) -> std::string {
        if (!isPathSafe(path)) {
            Log::Error("Security: Blocked attempt to read unsafe path: {}", path);
            return "";
        }
        auto bytes = VFSSystem::Get().ReadFile(path);
        if (bytes.empty())
            return "";
        return std::string(bytes.begin(), bytes.end());
    };

    file["write"] = [isPathSafe](const std::string& path, const std::string& content) {
        if (!isPathSafe(path)) {
            Log::Error("Security: Blocked attempt to write to unsafe path: {}", path);
            return;
        }
        std::ofstream f(path);
        if (f.is_open()) {
            f << content;
        }
    };

    // --- TIME API ---
    auto time = m_lua.create_table("time");
    time["get_fps"] = []() { return 1.0f / Engine::Get().GetTime().deltaTime; };
    time["get_dt"] = []() { return Engine::Get().GetTime().deltaTime; };
    time["get_time"] = []() { return Engine::Get().GetTime().totalTime; };
    time["set_scale"] = [](float scale) { Engine::Get().SetTimeScale(scale); };
    time["get_scale"] = []() { return Engine::Get().GetTime().timeScale; };

    // --- NEW PHYSICS API ---
    auto physics = m_lua.create_table("physics");
    physics["on_collision"] = [this](sol::protected_function callback) {
        m_lua["physics"]["_collision_callback"] = callback;
    };
    physics["create_box"] = [](uint32_t e, float w, float h, float d, bool isStatic) {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene)
            return;
        auto& reg = scene->GetRegistry();
        if (!reg.valid((entt::entity)e) || !reg.all_of<TransformComponent>((entt::entity)e))
            return;
        auto& t = reg.get<TransformComponent>((entt::entity)e);

        JPH::EMotionType motionType = isStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic;
        uint8_t layer = isStatic ? PhysicsLayers::NON_MOVING : PhysicsLayers::MOVING;

        Engine::Get().GetPhysics().CreateBody((entt::entity)e,
            JPH::Vec3(t.position.x, t.position.y, t.position.z),
            JPH::Quat(t.rotation.x, t.rotation.y, t.rotation.z, t.rotation.w),
            JPH::Vec3(w * 0.5f, h * 0.5f, d * 0.5f),
            motionType,
            layer);
    };
    physics["raycast"] =
        [](float ox, float oy, float oz, float dx, float dy, float dz, float maxDist, sol::this_state s)
        -> sol::variadic_results {
        sol::variadic_results res;
        sol::state_view lua(s);

        auto* physics = Engine::Get().GetPhysics().GetSystem();
        if (!physics) {
            res.push_back({lua, sol::in_place, false});
            return res;
        }

        glm::vec3 origin(ox, oy, oz);
        glm::vec3 direction(dx, dy, dz);
        float len = glm::length(direction);
        if (len > 0.0001f)
            direction = glm::normalize(direction);
        glm::vec3 rayVec = direction * maxDist;

        JPH::RRayCast ray{JPH::Vec3(origin.x, origin.y, origin.z), JPH::Vec3(rayVec.x, rayVec.y, rayVec.z)};

        JPH::RayCastResult result;
        bool hit = physics->GetNarrowPhaseQuery().CastRay(ray, result);

        res.push_back({lua, sol::in_place, hit});
        if (hit) {
            uint32_t hitEntity = 0;
            auto scene = Engine::Get().GetSceneStack().Active();
            if (scene) {
                auto& reg = scene->GetRegistry();
                auto view = reg.view<PhysicsComponent>();
                for (auto entity : view) {
                    if (view.get<PhysicsComponent>(entity).bodyID == result.mBodyID) {
                        hitEntity = (uint32_t)entity;
                        break;
                    }
                }
            }

            float hitDistance = result.mFraction * maxDist;
            glm::vec3 hitPos = origin + direction * hitDistance;

            res.push_back({lua, sol::in_place, hitEntity});
            res.push_back({lua, sol::in_place, hitDistance});
            res.push_back({lua, sol::in_place, hitPos.x});
            res.push_back({lua, sol::in_place, hitPos.y});
            res.push_back({lua, sol::in_place, hitPos.z});
        }
        return res;
    };

    // Helper helpers for FightingSystem bindings
    static auto FighterStateToString = [](FighterStateType state) -> const char* {
        switch (state) {
            case FighterStateType::IDLE: return "idle";
            case FighterStateType::WALK_FORWARD: return "walk_fwd";
            case FighterStateType::WALK_BACKWARD: return "walk_back";
            case FighterStateType::CROUCH: return "crouch";
            case FighterStateType::CROUCH_TRANSITION: return "crouch";
            case FighterStateType::JUMP_STARTUP: return "jump_startup";
            case FighterStateType::JUMP: return "jump";
            case FighterStateType::JUMP_LAND: return "jump_land";
            case FighterStateType::ATTACK: return "attack";
            case FighterStateType::SPECIAL: return "special";
            case FighterStateType::BLOCKSTUN_STAND: return "blockstun_stand";
            case FighterStateType::BLOCKSTUN_CROUCH: return "blockstun_crouch";
            case FighterStateType::HITSTUN_STAND: return "hitstun_stand";
            case FighterStateType::HITSTUN_CROUCH: return "hitstun_crouch";
            case FighterStateType::KNOCKDOWN: return "knockdown";
            case FighterStateType::GETUP: return "getup";
            case FighterStateType::THROW_STARTUP: return "throw_startup";
            case FighterStateType::THROW_ACTIVE: return "throw_active";
            case FighterStateType::THROWN: return "thrown";
            case FighterStateType::KO: return "ko";
            case FighterStateType::INTRO: return "intro";
            case FighterStateType::WIN: return "win";
            default: return "idle";
        }
    };

    static auto GetFighterBoxesLua = [](const Fighter& f, sol::this_state s) -> sol::table {
        sol::state_view lua(s);
        sol::table list = lua.create_table();
        int idx = 1;
        
        // Push box
        {
            FightBox box = f.GetPushBox();
            glm::vec2 wpos = f.GetBoxWorldPos(box);
            sol::table b = lua.create_table();
            b["type"] = "push";
            b["x"] = wpos.x;
            b["y"] = wpos.y;
            b["w"] = box.size.x;
            b["h"] = box.size.y;
            list[idx++] = b;
        }
        
        // Hurt box
        {
            FightBox box = f.GetHurtBox();
            glm::vec2 wpos = f.GetBoxWorldPos(box);
            sol::table b = lua.create_table();
            b["type"] = "hurt";
            b["x"] = wpos.x;
            b["y"] = wpos.y;
            b["w"] = box.size.x;
            b["h"] = box.size.y;
            list[idx++] = b;
        }
        
        // Hit box
        {
            FightBox box = f.GetHitBox();
            if (box.active) {
                glm::vec2 wpos = f.GetBoxWorldPos(box);
                sol::table b = lua.create_table();
                b["type"] = "hit";
                b["x"] = wpos.x;
                b["y"] = wpos.y;
                b["w"] = box.size.x;
                b["h"] = box.size.y;
                list[idx++] = b;
            }
        }
        return list;
    };

    static auto GetProjectilesLua = [](const std::vector<FightingSystem::Projectile>& projs, sol::this_state s) -> sol::table {
        sol::state_view lua(s);
        sol::table list = lua.create_table();
        int idx = 1;
        for (const auto& proj : projs) {
            if (proj.active) {
                sol::table p = lua.create_table();
                p["x"] = proj.position.x;
                p["y"] = proj.position.y;
                p["w"] = proj.size.x;
                p["h"] = proj.size.y;
                p["owner"] = proj.ownerIndex;
                p["facing"] = proj.facing;
                list[idx++] = p;
            }
        }
        return list;
    };

    static auto GetFighterInputsLua = [](const Fighter& f, sol::this_state s) -> sol::table {
        sol::state_view lua(s);
        sol::table list = lua.create_table();
        int idx = 1;
        int curr = (f.inputBufferIndex - 1 + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
        for (int i = 0; i < 15; ++i) {
            const auto& frame = f.inputBuffer[curr];
            sol::table inputFrame = lua.create_table();
            inputFrame["dir"] = static_cast<int>(frame.direction);
            inputFrame["buttons"] = frame.buttons;
            list[idx++] = inputFrame;
            curr = (curr - 1 + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
        }
        return list;
    };

    // --- FIGHTING SYSTEM BINDINGS ---
    auto fight = m_lua.create_table("fight");
    fight["start_match"] = [](int p1Char, int p2Char, int stage) {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (fs) fs->StartMatch(p1Char, p2Char, stage);
    };
    fight["set_round_count"] = [](int rounds) {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (fs) fs->SetRoundCount(rounds);
    };
    fight["set_ai_difficulty"] = [](int difficulty) {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (fs) fs->SetAIDifficulty(difficulty);
    };
    fight["is_active"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->IsActive() : false;
    };
    fight["set_active"] = [](bool active) {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (fs) fs->SetActive(active);
    };
    fight["get_p1_super"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP1().superMeter : 0.0f;
    };
    fight["get_p2_super"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP2().superMeter : 0.0f;
    };
    fight["get_p1_ex"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP1().isEX : false;
    };
    fight["get_p2_ex"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP2().isEX : false;
    };
    fight["get_p1_super_active"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP1().isSuper : false;
    };
    fight["get_p2_super_active"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP2().isSuper : false;
    };
    fight["get_p1_counter"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP1().isCounterFlash : false;
    };
    fight["get_p2_counter"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP2().isCounterFlash : false;
    };
    fight["get_p1_parry"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP1().isParryFlash : false;
    };
    fight["get_p2_parry"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP2().isParryFlash : false;
    };
    fight["get_jumps_remaining"] = [](int playerIndex) {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) return 0;
        return (playerIndex == 0) ? fs->GetP1().jumpsRemaining : fs->GetP2().jumpsRemaining;
    };
    fight["get_p1_health"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP1().currentHealth : 0;
    };
    fight["get_p2_health"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP2().currentHealth : 0;
    };
    fight["get_p1_max_health"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP1().def.health : 1000;
    };
    fight["get_p2_max_health"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP2().def.health : 1000;
    };
    fight["get_timer"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetMatchState().timer : 0.0f;
    };
    fight["get_round"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetMatchState().currentRound : 1;
    };
    fight["get_p1_score"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetMatchState().p1Score : 0;
    };
    fight["get_p2_score"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetMatchState().p2Score : 0;
    };
    fight["get_state"] = []() -> const char* {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) return "none";
        switch (fs->GetMatchState().phase) {
            case MatchPhase::INTRO: return "intro";
            case MatchPhase::COUNTDOWN: return "countdown";
            case MatchPhase::FIGHTING: return "fighting";
            case MatchPhase::HIT_PAUSE: return "fighting";
            case MatchPhase::KO: return "ko";
            case MatchPhase::ROUND_END: return "round_end";
            case MatchPhase::MATCH_END: return "match_end";
            case MatchPhase::PAUSED: return "paused";
            default: return "none";
        }
    };
    fight["get_p1_pos"] = []() -> std::tuple<float, float> {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) return {0.0f, 0.0f};
        return {fs->GetP1().position.x, fs->GetP1().position.y};
    };
    fight["get_p2_pos"] = []() -> std::tuple<float, float> {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) return {0.0f, 0.0f};
        return {fs->GetP2().position.x, fs->GetP2().position.y};
    };
    fight["get_p1_facing"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP1().facing : 1;
    };
    fight["get_p2_facing"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP2().facing : -1;
    };
    fight["get_p1_state"] = []() -> const char* {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? FighterStateToString(fs->GetP1().state) : "idle";
    };
    fight["get_p2_state"] = []() -> const char* {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? FighterStateToString(fs->GetP2().state) : "idle";
    };
    fight["get_p1_state_frame"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP1().stateFrame : 0;
    };
    fight["get_p2_state_frame"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP2().stateFrame : 0;
    };
    fight["get_p1_combo"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetMatchState().comboCountP1 : 0;
    };
    fight["get_p2_combo"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetMatchState().comboCountP2 : 0;
    };
    fight["get_p1_combo_damage"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP1().comboDamage : 0;
    };
    fight["get_p2_combo_damage"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP2().comboDamage : 0;
    };
    fight["get_p1_cancel"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP1().isCancelFlash : false;
    };
    fight["get_p2_cancel"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP2().isCancelFlash : false;
    };
    fight["get_hit_pause_frames"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetMatchState().hitPauseFrames : 0;
    };
    fight["get_p1_name"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP1().def.name.c_str() : "KAITO";
    };
    fight["get_p2_name"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetP2().def.name.c_str() : "GOROU";
    };
    fight["get_p1_colors"] = [](sol::this_state s) -> sol::variadic_results {
        sol::variadic_results res;
        sol::state_view lua(s);
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (fs) {
            auto bc = fs->GetP1().def.bodyColor;
            auto ac = fs->GetP1().def.accentColor;
            res.push_back({lua, sol::in_place, bc.x});
            res.push_back({lua, sol::in_place, bc.y});
            res.push_back({lua, sol::in_place, bc.z});
            res.push_back({lua, sol::in_place, ac.x});
            res.push_back({lua, sol::in_place, ac.y});
            res.push_back({lua, sol::in_place, ac.z});
        } else {
            for (int i = 0; i < 6; ++i) res.push_back({lua, sol::in_place, 0.5f});
        }
        return res;
    };
    fight["get_p2_colors"] = [](sol::this_state s) -> sol::variadic_results {
        sol::variadic_results res;
        sol::state_view lua(s);
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (fs) {
            auto bc = fs->GetP2().def.bodyColor;
            auto ac = fs->GetP2().def.accentColor;
            res.push_back({lua, sol::in_place, bc.x});
            res.push_back({lua, sol::in_place, bc.y});
            res.push_back({lua, sol::in_place, bc.z});
            res.push_back({lua, sol::in_place, ac.x});
            res.push_back({lua, sol::in_place, ac.y});
            res.push_back({lua, sol::in_place, ac.z});
        } else {
            for (int i = 0; i < 6; ++i) res.push_back({lua, sol::in_place, 0.5f});
        }
        return res;
    };
    fight["get_p1_boxes"] = [](sol::this_state s) {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) {
            sol::state_view lua(s);
            return lua.create_table();
        }
        return GetFighterBoxesLua(fs->GetP1(), s);
    };
    fight["get_p2_boxes"] = [](sol::this_state s) {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) {
            sol::state_view lua(s);
            return lua.create_table();
        }
        return GetFighterBoxesLua(fs->GetP2(), s);
    };
    fight["get_projectiles"] = [](sol::this_state s) {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) {
            sol::state_view lua(s);
            return lua.create_table();
        }
        return GetProjectilesLua(fs->GetProjectiles(), s);
    };
    fight["get_p1_inputs"] = [](sol::this_state s) {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) {
            sol::state_view lua(s);
            return lua.create_table();
        }
        return GetFighterInputsLua(fs->GetP1(), s);
    };
    fight["get_p2_inputs"] = [](sol::this_state s) {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) {
            sol::state_view lua(s);
            return lua.create_table();
        }
        return GetFighterInputsLua(fs->GetP2(), s);
    };
    fight["get_p1_pushbox"] = [](sol::this_state s) -> sol::table {
        sol::state_view lua(s);
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) return lua.create_table();
        const auto& f = fs->GetP1();
        FightBox box = f.GetPushBox();
        glm::vec2 wpos = f.GetBoxWorldPos(box);
        sol::table b = lua.create_table();
        b["x"] = wpos.x;
        b["y"] = wpos.y;
        b["w"] = box.size.x;
        b["h"] = box.size.y;
        return b;
    };
    fight["get_p1_hurtbox"] = [](sol::this_state s) -> sol::table {
        sol::state_view lua(s);
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) return lua.create_table();
        const auto& f = fs->GetP1();
        FightBox box = f.GetHurtBox();
        glm::vec2 wpos = f.GetBoxWorldPos(box);
        sol::table b = lua.create_table();
        b["x"] = wpos.x;
        b["y"] = wpos.y;
        b["w"] = box.size.x;
        b["h"] = box.size.y;
        return b;
    };
    fight["get_p1_hitbox"] = [](sol::this_state s) -> sol::optional<sol::table> {
        sol::state_view lua(s);
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) return sol::nullopt;
        const auto& f = fs->GetP1();
        FightBox box = f.GetHitBox();
        if (!box.active) return sol::nullopt;
        glm::vec2 wpos = f.GetBoxWorldPos(box);
        sol::table b = lua.create_table();
        b["x"] = wpos.x;
        b["y"] = wpos.y;
        b["w"] = box.size.x;
        b["h"] = box.size.y;
        return b;
    };
    fight["get_p2_pushbox"] = [](sol::this_state s) -> sol::table {
        sol::state_view lua(s);
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) return lua.create_table();
        const auto& f = fs->GetP2();
        FightBox box = f.GetPushBox();
        glm::vec2 wpos = f.GetBoxWorldPos(box);
        sol::table b = lua.create_table();
        b["x"] = wpos.x;
        b["y"] = wpos.y;
        b["w"] = box.size.x;
        b["h"] = box.size.y;
        return b;
    };
    fight["get_p2_hurtbox"] = [](sol::this_state s) -> sol::table {
        sol::state_view lua(s);
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) return lua.create_table();
        const auto& f = fs->GetP2();
        FightBox box = f.GetHurtBox();
        glm::vec2 wpos = f.GetBoxWorldPos(box);
        sol::table b = lua.create_table();
        b["x"] = wpos.x;
        b["y"] = wpos.y;
        b["w"] = box.size.x;
        b["h"] = box.size.y;
        return b;
    };
    fight["get_p2_hitbox"] = [](sol::this_state s) -> sol::optional<sol::table> {
        sol::state_view lua(s);
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (!fs) return sol::nullopt;
        const auto& f = fs->GetP2();
        FightBox box = f.GetHitBox();
        if (!box.active) return sol::nullopt;
        glm::vec2 wpos = f.GetBoxWorldPos(box);
        sol::table b = lua.create_table();
        b["x"] = wpos.x;
        b["y"] = wpos.y;
        b["w"] = box.size.x;
        b["h"] = box.size.y;
        return b;
    };
    fight["save_replay"] = [](const std::string& path) {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (fs) fs->SaveReplay(path);
    };
    fight["load_replay"] = [](const std::string& path) {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->LoadReplay(path) : false;
    };
    fight["play_replay"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (fs) fs->PlayReplay();
    };
    fight["stop_replay"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        if (fs) fs->StopReplay();
    };
    fight["is_replaying"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->IsReplaying() : false;
    };
    fight["get_replay_p1_char"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetMatchState().p1CharIndex : 0;
    };
    fight["get_replay_p2_char"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetMatchState().p2CharIndex : 1;
    };
    fight["get_replay_stage"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetMatchState().stageIndex : 0;
    };
    fight["get_replay_rounds"] = []() {
        auto* fs = Engine::Get().GetSystem<FightingSystem>();
        return fs ? fs->GetMatchState().roundsToWin : 2;
    };

    Log::Info("ScriptSystem: Phase 13 - CSM + Gameplay Lua API + Renderer2D + FightingSystem Ready.");
    return true;
}

static void SafeSetErrorHandler(sol::protected_function& func, sol::state& lua) {
    sol::object dbgObj = lua["debug"];
    if (dbgObj.valid() && dbgObj.is<sol::table>()) {
        sol::table dbg = dbgObj;
        sol::object tbObj = dbg["traceback"];
        if (tbObj.valid() && tbObj.is<sol::function>()) {
            func.set_error_handler(tbObj);
        }
    }
}

void ScriptSystem::ExecuteFile(const std::string& path) {
    // Always load core.lua first (unless we ARE core.lua)
    if (path != "assets/scripts/core.lua") {
        if (!m_coreLoaded) {
            auto coreBytes = VFSSystem::Get().ReadFile("assets/scripts/core.lua");
            if (!coreBytes.empty()) {
                std::string coreCode(coreBytes.begin(), coreBytes.end());
                sol::load_result loadResult = m_lua.load(coreCode, "@assets/scripts/core.lua");
                if (!loadResult.valid()) {
                    sol::error err = loadResult;
                    Log::Error("Core Library Compile Error: {}", err.what());
                } else {
                    sol::protected_function scriptFunc = loadResult;
                    SafeSetErrorHandler(scriptFunc, m_lua);
                    auto result = scriptFunc();
                    if (!result.valid()) {
                        sol::error err = result;
                        Log::Error("Core Library Runtime Error: {}", err.what());
                    } else {
                        m_coreLoaded = true;
                    }
                }
            }
        }
        if (!m_bridgeLoaded) {
            auto bridgeBytes = VFSSystem::Get().ReadFile("assets/scripts/sba_bridge.lua");
            if (!bridgeBytes.empty()) {
                std::string bridgeCode(bridgeBytes.begin(), bridgeBytes.end());
                sol::load_result loadResult = m_lua.load(bridgeCode, "@assets/scripts/sba_bridge.lua");
                if (!loadResult.valid()) {
                    sol::error err = loadResult;
                    Log::Error("Bridge Library Compile Error: {}", err.what());
                } else {
                    sol::protected_function scriptFunc = loadResult;
                    SafeSetErrorHandler(scriptFunc, m_lua);
                    auto result = scriptFunc();
                    if (!result.valid()) {
                        sol::error err = result;
                        Log::Error("Bridge Library Runtime Error: {}", err.what());
                    } else {
                        m_bridgeLoaded = true;
                    }
                }
            }
        }
    }

    auto bytes = VFSSystem::Get().ReadFile(path);
    if (bytes.empty()) {
        Log::Error("Lua Error: cannot open {}: No such file or directory", path);
        return;
    }
    std::string scriptCode(bytes.begin(), bytes.end());

    sol::load_result loadResult = m_lua.load(scriptCode, "@" + path);
    if (!loadResult.valid()) {
        sol::error err = loadResult;
        Log::Error("Lua Compile Error in {}: {}", path, err.what());
        return;
    }
    sol::protected_function scriptFunc = loadResult;
    SafeSetErrorHandler(scriptFunc, m_lua);
    auto result = scriptFunc();
    if (!result.valid()) {
        sol::error err = result;
        Log::Error("Lua Runtime Error in {}: {}", path, err.what());
    }
}

void ScriptSystem::OnUpdate(float dt) {
    if (Engine::IsInitialized()) {
        auto editor = Engine::Get().GetSystem<EditorSystem>();
        if (editor) {
            editor->ClearActiveBtNodes();
        }
    }

    // Automatic Timer System Update
    sol::protected_function timerUpdate = m_lua["Timer"]["update"];
    if (timerUpdate.valid()) {
        SafeSetErrorHandler(timerUpdate, m_lua);
        auto result = timerUpdate(dt);
        if (!result.valid()) {
            sol::error err = result;
            Log::Error("Lua Timer Update Error: {}", err.what());
        }
    }

    // Dispatch Jolt collision events to Lua
    auto scene = Engine::Get().GetSceneStack().Active();
    if (scene && m_lua["physics"] && m_lua["physics"]["_collision_callback"]) {
        sol::protected_function cb = m_lua["physics"]["_collision_callback"];
        auto& physSys = Engine::Get().GetPhysics();
        auto& events = physSys.GetCollisionEvents();
        auto& reg = scene->GetRegistry();

        for (auto& evt : events) {
            uint32_t e1 = 0;
            uint32_t e2 = 0;

            auto view = reg.view<PhysicsComponent>();
            for (auto entity : view) {
                if (view.get<PhysicsComponent>(entity).bodyID == evt.b1) {
                    e1 = (uint32_t)entity;
                }
                if (view.get<PhysicsComponent>(entity).bodyID == evt.b2) {
                    e2 = (uint32_t)entity;
                }
                if (e1 != 0 && e2 != 0)
                    break;
            }

            if (e1 != 0 && e2 != 0) {
                SafeSetErrorHandler(cb, m_lua);
                auto res = cb(e1, e2);
                if (!res.valid()) {
                    sol::error err = res;
                    Log::Error("Lua Collision Callback Error: {}", err.what());
                }
            }
        }
    }

    sol::protected_function updateFunc = m_lua["OnUpdate"];
    if (updateFunc.valid()) {
        SafeSetErrorHandler(updateFunc, m_lua);
        auto result = updateFunc(dt);
        if (!result.valid()) {
            sol::error err = result;
            Log::Error("Lua OnUpdate Error: {}", err.what());
        }
    }
}

void ScriptSystem::OnRender() {
    using render_clock = std::chrono::high_resolution_clock;
    auto start = render_clock::now();

    sol::protected_function renderFunc = m_lua["OnRender"];
    if (renderFunc.valid()) {
        SafeSetErrorHandler(renderFunc, m_lua);
        auto result = renderFunc();
        if (!result.valid()) {
            sol::error err = result;
            Log::Error("Lua OnRender Error: {}", err.what());
        }
    }

    auto end = render_clock::now();
    float elapsed = std::chrono::duration<float, std::milli>(end - start).count();
    Engine::Get().AccumulateScriptTime(elapsed);
}

void ScriptSystem::OnUIRender() {
    using ui_clock = std::chrono::high_resolution_clock;
    auto start = ui_clock::now();

    // Begin Renderer2D batch for Lua 2D drawing
    Renderer2D::BeginBatch();

    sol::protected_function uiFunc = m_lua["OnRenderUI"];
    if (uiFunc.valid()) {
        SafeSetErrorHandler(uiFunc, m_lua);
        auto result = uiFunc();
        if (!result.valid()) {
            sol::error err = result;
            Log::Error("Lua OnRenderUI Error: {}", err.what());
        }
    }

    // Flush all 2D quads drawn by Lua
    Renderer2D::EndBatch();

    auto end = ui_clock::now();
    float elapsed = std::chrono::duration<float, std::milli>(end - start).count();
    Engine::Get().AccumulateScriptTime(elapsed);
}

}  // namespace starlight
