// Este projeto ÃƒÂ© feito por IA e sÃƒÂ³ o prompt ÃƒÂ© feito por um humano.
#include "ScriptSystem.hpp"
#include "Log.hpp"
#include "Engine.hpp"
#include "InputSystem.hpp"
#include "Renderer.hpp"
#include "AudioSystem.hpp"
#include "Components.hpp"
#include "PhysicsSystem.hpp"
#include "VFXSystem.hpp"
#include "Renderer2D.hpp"
#include "AssetLoader.hpp"
#include "DashboardSystem.hpp"
#include "imgui.h"
#include <fstream>
#include <sstream>
#include <random>
#include <filesystem>
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
        m_lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table, sol::lib::package);
    }

    ScriptSystem::~ScriptSystem() {}

    bool ScriptSystem::OnInitialize(const EngineContext& context) {
        (void)context;
        
        // Set standard error handler
        m_lua.set_exception_handler([](lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) {
            Log::Error("[Lua Exception] {}", description);
            return sol::stack::push(L, description);
        });
        // --- MATH TYPES ---
        m_lua.new_usertype<glm::vec2>("vec2",
            sol::constructors<glm::vec2(float, float)>(),
            "x", &glm::vec2::x, "y", &glm::vec2::y
        );
        m_lua.new_usertype<glm::vec3>("vec3",
            sol::constructors<glm::vec3(float, float, float)>(),
            "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z
        );
        m_lua.new_usertype<glm::quat>("quat",
            sol::constructors<glm::quat(float, float, float, float)>(),
            "x", &glm::quat::x, "y", &glm::quat::y, "z", &glm::quat::z, "w", &glm::quat::w
        );

        // --- THE NUCLEAR ENGINE TABLE ---
        // Instead of usertypes for Core, we use a direct function table for 100% stability.
        auto engine = m_lua.create_table("Engine");
        
        // Registry
        engine["spawn"] = [](const std::string& tag) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (!scene) {
                Log::Error("Script Error: Attempted to spawn '{}' but no active scene exists!", tag);
                return (uint32_t)0;
            }
            auto& reg = scene->GetRegistry();
            auto e = reg.create();
            reg.emplace<TransformComponent>(e);
            auto& m = reg.emplace<MeshComponent>(e);
            m.material.isPBR = true;
            m.mesh = Engine::Get().GetRenderer().GetCubeMesh();
            return (uint32_t)e;
        };
        engine["set_pos"] = [](uint32_t e, float x, float y, float z) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (!scene) return;
            auto& reg = scene->GetRegistry();
            if (!reg.valid((entt::entity)e)) return;
            auto& t = reg.get<TransformComponent>((entt::entity)e);
            t.position = {x, y, z};
        };
        engine["spawn_light"] = [](float x, float y, float z, float r, float g, float b, float intensity) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (!scene) return (uint32_t)0;
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
            if (!scene) return;
            auto& reg = scene->GetRegistry();
            if (!reg.valid((entt::entity)e)) return;
            auto& t = reg.get<TransformComponent>((entt::entity)e);
            t.rotation = t.rotation * glm::quat(glm::vec3(x, y, z));
        };
        engine["set_scale"] = [](uint32_t e, float x, float y, float z) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (!scene) return;
            auto& reg = scene->GetRegistry();
            if (!reg.valid((entt::entity)e)) return;
            auto& t = reg.get<TransformComponent>((entt::entity)e);
            t.scale = {x, y, z};
        };
        engine["set_color"] = [](uint32_t e, float r, float g, float b) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (!scene) return;
            auto& reg = scene->GetRegistry();
            if (!reg.valid((entt::entity)e)) return;
            auto& m = reg.get<MeshComponent>((entt::entity)e);
            m.material.color = {r, g, b};
        };
        engine["set_material"] = [](uint32_t e, float m, float r) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (!scene) return;
            auto& reg = scene->GetRegistry();
            if (!reg.valid((entt::entity)e)) return;
            auto& mesh = reg.get<MeshComponent>((entt::entity)e);
            mesh.material.metallic = m;
            mesh.material.roughness = r;
        };
        
        // Input
        engine["is_down"] = [](const std::string& key) {
            return Engine::Get().GetInput().IsActionPressed(key);
        };
        
        // Time
        engine["get_dt"] = []() { return Engine::Get().GetTime().deltaTime; };
        engine["get_time"] = []() { return Engine::Get().GetTime().totalTime; };
        
        engine["set_bloom"] = [](float threshold, int steps) {
            auto& r = Engine::Get().GetRenderer();
            r.m_bloomThreshold = threshold;
            r.m_bloomBlurSteps = steps;
        };
        engine["set_exposure"] = [](float exposure, float gamma) {
            auto& r = Engine::Get().GetRenderer();
            r.m_exposure = exposure;
            r.m_gamma = gamma;
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
                    res.push_back({ lua, sol::in_place, hit.x });
                    res.push_back({ lua, sol::in_place, hit.z });
                    return res;
                }
            }
            return res;
        };
        
        // Audio
        engine["play_sound"] = [](const std::string& path) {
            Engine::Get().GetAudio().PlayEffect(path);
        };

        // --- PHYSICS API (Phase 11) ---
        engine["apply_force"] = [](uint32_t e, float fx, float fy, float fz) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (!scene) return;
            Engine::Get().GetPhysics().ApplyForce((entt::entity)e, glm::vec3(fx, fy, fz));
        };
        engine["apply_impulse"] = [](uint32_t e, float ix, float iy, float iz) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (!scene) return;
            Engine::Get().GetPhysics().ApplyImpulse((entt::entity)e, glm::vec3(ix, iy, iz));
        };
        engine["set_velocity"] = [](uint32_t e, float vx, float vy, float vz) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (!scene) return;
            Engine::Get().GetPhysics().SetVelocity((entt::entity)e, glm::vec3(vx, vy, vz));
        };

        // --- ENTITY MANAGEMENT (Phase 11) ---
        engine["destroy"] = [](uint32_t e) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (!scene) return;
            auto& reg = scene->GetRegistry();
            if (reg.valid((entt::entity)e)) {
                reg.destroy((entt::entity)e);
            }
        };
        engine["get_pos"] = [](uint32_t e, sol::this_state s) -> sol::variadic_results {
            sol::variadic_results res;
            sol::state_view lua(s);
            auto scene = Engine::Get().GetSceneStack().Active();
            if (!scene) return res;
            auto& reg = scene->GetRegistry();
            if (!reg.valid((entt::entity)e)) return res;
            auto& t = reg.get<TransformComponent>((entt::entity)e);
            res.push_back({ lua, sol::in_place, t.position.x });
            res.push_back({ lua, sol::in_place, t.position.y });
            res.push_back({ lua, sol::in_place, t.position.z });
            return res;
        };

        engine["set_light_color"] = [](uint32_t e, float r, float g, float b) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (!scene) return;
            auto& reg = scene->GetRegistry();
            if (!reg.valid((entt::entity)e) || !reg.all_of<PointLightComponent>((entt::entity)e)) return;
            auto& l = reg.get<PointLightComponent>((entt::entity)e);
            l.color = {r, g, b};
        };
        
        engine["set_light_intensity"] = [](uint32_t e, float intensity) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (!scene) return;
            auto& reg = scene->GetRegistry();
            if (!reg.valid((entt::entity)e) || !reg.all_of<PointLightComponent>((entt::entity)e)) return;
            auto& l = reg.get<PointLightComponent>((entt::entity)e);
            l.intensity = intensity;
        };
        
        engine["get_window_size"] = [](sol::this_state s) -> sol::variadic_results {
            sol::variadic_results res;
            sol::state_view lua(s);
            auto& window = Engine::Get().GetWindow();
            res.push_back({ lua, sol::in_place, window.GetWidth() });
            res.push_back({ lua, sol::in_place, window.GetHeight() });
            return res;
        };

        // --- PBR MATERIAL CONTROL (Phase 11) ---
        engine["set_pbr"] = [](uint32_t e, float met, float rough, float ambientOcc) {
            auto scene = Engine::Get().GetSceneStack().Active();
            if (!scene) return;
            auto& reg = scene->GetRegistry();
            if (!reg.valid((entt::entity)e)) return;
            if (!reg.all_of<MeshComponent>((entt::entity)e)) return;
            auto& m = reg.get<MeshComponent>((entt::entity)e);
            m.material.metallic = met;
            m.material.roughness = rough;
            m.material.ao = ambientOcc;
        };

        // --- LOGGING (Phase 11) ---
        engine["log"] = [](const std::string& msg) {
            Log::Info("[Lua] {}", msg);
        };
        engine["log_warn"] = [](const std::string& msg) {
            Log::Warn("[Lua] {}", msg);
        };
        engine["log_error"] = [](const std::string& msg) {
            Log::Error("[Lua] {}", msg);
        };

        // --- IMGUI ---
        auto imgui = m_lua.create_table("imgui");
        imgui["text"] = [](float x, float y, float r, float g, float b, const std::string& msg) {
            ImGui::GetForegroundDrawList()->AddText({x, y}, ImColor(r, g, b), msg.c_str());
        };
        imgui["rect"] = [](float x, float y, float w, float h, float r, float g, float b) {
            ImGui::GetForegroundDrawList()->AddRectFilled({x, y}, {x+w, y+h}, ImColor(r, g, b));
        };

        // --- DASHBOARD UI (Non-ImGui) ---
        auto ui = m_lua.create_table("ui");
        ui["begin"] = [](int w, int h) {
            auto dash = Engine::Get().GetSystem<DashboardSystem>();
            if (dash) dash->Begin(w, h);
        };
        ui["panel"] = [](float x, float y, float w, float h, float r, float g, float b, float a) {
            auto dash = Engine::Get().GetSystem<DashboardSystem>();
            if (dash) dash->Panel(x, y, w, h, {r, g, b, a});
        };
        ui["label"] = [](const std::string& text, float x, float y, float r, float g, float b, float a) {
            auto dash = Engine::Get().GetSystem<DashboardSystem>();
            if (dash) dash->Label(text, x, y, {r, g, b, a});
        };
        ui["button"] = [](const std::string& label, float x, float y, float w, float h) {
            auto dash = Engine::Get().GetSystem<DashboardSystem>();
            if (dash) return dash->Button(label, x, y, w, h);
            return false;
        };
        ui["finish"] = []() {
            auto dash = Engine::Get().GetSystem<DashboardSystem>();
            if (dash) dash->End(Engine::Get().GetRenderer());
        };

        // --- ENGINE 2D RENDERER (Renderer2D batched quads) ---
        auto gfx = m_lua.create_table("gfx");
        gfx["draw_quad"] = [](float x, float y, float w, float h, float r, float g, float b, float a) {
            Renderer2D::DrawQuad({x, y}, {w, h}, {r, g, b, a});
        };
        gfx["draw_sprite"] = [](float x, float y, float w, float h, uint32_t texID, float r, float g, float b, float a) {
            Renderer2D::DrawQuad({x, y}, {w, h}, texID, {r, g, b, a});
        };
        gfx["draw_iso_tile"] = [](float x, float y, float w, float h, uint32_t texID, float r, float g, float b, float a) {
            Renderer2D::DrawIsometricTile({x, y}, {w, h}, texID, {r, g, b, a});
        };
        gfx["draw_sprite_clean"] = [](float x, float y, float w, float h, uint32_t texID, float r, float g, float b, float a) {
            Renderer2D::DrawSpriteClean({x, y}, {w, h}, texID, {r, g, b, a});
        };
        gfx["draw_sprite"] = [](float x, float y, float w, float h, uint32_t texID, float r, float g, float b, float a) {
            Renderer2D::DrawSpriteClean({x, y}, {w, h}, texID, {r, g, b, a});
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
        gfx["draw_text"] = [](const std::string& text, float x, float y, float scale, float r, float g, float b, float a) {
            auto dash = Engine::Get().GetSystem<DashboardSystem>();
            if (dash) dash->Label(text, x, y, {r, g, b, a});
            else Renderer2D::DrawString(text, {x, y}, scale, {r, g, b, a});
        };
        // Line: drawn as a thin quad between two points
        gfx["draw_line"] = [](float x1, float y1, float x2, float y2, float thickness, float r, float g, float b, sol::optional<float> a) {
            float dx = x2 - x1, dy = y2 - y1;
            float len = std::sqrt(dx*dx + dy*dy);
            if (len < 0.001f) return;
            float cx = (x1+x2)*0.5f, cy = (y1+y2)*0.5f;
            Renderer2D::DrawQuad({cx - len*0.5f, cy - thickness*0.5f}, {len, thickness}, {r, g, b, a.value_or(1.0f)});
        };
        // Rect outline: 4 thin quads
        gfx["draw_rect_outline"] = [](float x, float y, float w, float h, float t, float r, float g, float b, sol::optional<float> a) {
            glm::vec4 c = {r, g, b, a.value_or(1.0f)};
            Renderer2D::DrawQuad({x, y}, {w, t}, c);          // top
            Renderer2D::DrawQuad({x, y+h-t}, {w, t}, c);      // bottom
            Renderer2D::DrawQuad({x, y+t}, {t, h-2*t}, c);    // left
            Renderer2D::DrawQuad({x+w-t, y+t}, {t, h-2*t}, c);// right
        };
        // Circle: approximated with N quads around center
        gfx["draw_circle"] = [](float cx, float cy, float radius, float r, float g, float b, sol::optional<int> segments, sol::optional<float> a) {
            int seg = segments.value_or(24);
            float step = 2.0f * 3.14159265f / (float)seg;
            float thickness = 2.0f;
            for (int i = 0; i < seg; i++) {
                float a1 = step * i, a2 = step * (i+1);
                float x1 = cx + std::cos(a1)*radius, y1 = cy + std::sin(a1)*radius;
                float x2 = cx + std::cos(a2)*radius, y2 = cy + std::sin(a2)*radius;
                float mx = (x1+x2)*0.5f - thickness*0.5f;
                float my = (y1+y2)*0.5f - thickness*0.5f;
                float dx = x2-x1, dy = y2-y1;
                float len = std::sqrt(dx*dx+dy*dy);
                Renderer2D::DrawQuad({mx, my}, {len, thickness}, {r, g, b, a.value_or(1.0f)});
            }
        };
        // Circle filled: concentric rings of quads
        gfx["draw_circle_filled"] = [](float cx, float cy, float radius, float r, float g, float b, sol::optional<float> a) {
            int seg = 24;
            float step = 2.0f * 3.14159265f / (float)seg;
            for (int i = 0; i < seg; i++) {
                float a1 = step * i, a2 = step * (i+1);
                float x1 = cx + std::cos(a1)*radius;
                float y1 = cy + std::sin(a1)*radius;
                float x2 = cx + std::cos(a2)*radius;
                float y2 = cy + std::sin(a2)*radius;
                // Triangle fan via thin quad from center to edge
                float mx = std::min({cx, x1, x2}), my = std::min({cy, y1, y2});
                float Mx = std::max({cx, x1, x2}), My = std::max({cy, y1, y2});
                Renderer2D::DrawQuad({mx, my}, {Mx-mx, My-my}, {r, g, b, a.value_or(0.5f)});
            }
        };
        // Screen dimensions shortcut
        gfx["screen_width"] = []() { return (float)Engine::Get().GetWindow().GetWidth(); };
        gfx["screen_height"] = []() { return (float)Engine::Get().GetWindow().GetHeight(); };
        
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

        // --- CAMERA API ---
        auto camera = m_lua.create_table("camera");
        camera["set_pos"] = [](float x, float y, float z) {
            Engine::Get().GetRenderer().GetCameraTransform().position = {x, y, z};
        };
        camera["look_at"] = [](float x, float y, float z) {
            Engine::Get().GetRenderer().SetCameraLookAt({x, y, z});
        };
        camera["set_fov"] = [](float fov) {
            auto& renderer = Engine::Get().GetRenderer();
            renderer.UpdateProjection(fov, (float)Engine::Get().GetWindow().GetWidth()/(float)Engine::Get().GetWindow().GetHeight(), 0.1f, 1000.0f);
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
        audio["play_sound"] = [](const std::string& path) { 
            Engine::Get().GetAudio().PlayEffect(path); 
        };
        audio["play_music"] = [](const std::string& path, sol::optional<bool> loop, sol::optional<float> vol) { 
            Engine::Get().GetAudio().PlayMusic(path, loop.value_or(true), vol.value_or(1.0f)); 
        };
        audio["stop_music"] = []() { 
            Engine::Get().GetAudio().StopMusic(); 
        };
        
        // VFX
        auto vfx = m_lua.create_table("vfx");
        vfx["emit"] = [](float x, float y, float z, float vx, float vy, float vz, float r, float g, float b, int count, float size) {
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
        audio["set_low_pass"] = [](float cutoff) {
            Engine::Get().GetAudio().m_lowPassCutoff = glm::clamp(cutoff, 0.0f, 1.0f);
        };
        audio["set_envelope"] = [](float attack, float decay, float sustain, float release) {
            for (auto& v : Engine::Get().GetAudio().m_voices) {
                v.attack = attack; v.decay = decay; v.sustain = sustain; v.release = release;
            }
        };

        // --- INPUT API ---
        auto input = m_lua.create_table("input");
        input["get_mouse_pos"] = []() { return Engine::Get().GetInput().GetMousePosition(); };
        input["get_mouse_x"] = []() { return Engine::Get().GetInput().GetMousePosition().x; };
        input["get_mouse_y"] = []() { return Engine::Get().GetInput().GetMousePosition().y; };
        input["is_down"] = [](const std::string& key) { return Engine::Get().GetInput().IsActionPressed(key); };
        input["is_just_pressed"] = [](const std::string& key) { return Engine::Get().GetInput().IsActionJustPressed(key); };
        input["get_axis"] = [](const std::string& axis) { return Engine::Get().GetInput().GetAxis(axis); };
        input["is_gamepad_down"] = [](const std::string& btn) { return Engine::Get().GetInput().IsGamepadButtonPressed(btn); };
        input["vibrate"] = [](float left, float right, uint32_t ms) { Engine::Get().GetInput().Vibrate(left, right, ms); };

        // --- FILE API (Hardened for Industrial Security) ---
        auto file = m_lua.create_table("file");
        auto isPathSafe = [](const std::string& path) {
            // Simple check: don't allow ".." or absolute paths to system dirs
            if (path.find("..") != std::string::npos) return false;
            if (path.find(":") != std::string::npos) return false; // No C:\ absolute
            return true;
        };

        file["read"] = [isPathSafe](const std::string& path) -> std::string {
            if (!isPathSafe(path)) {
                Log::Error("Security: Blocked attempt to read unsafe path: {}", path);
                return "";
            }
            std::ifstream f(path);
            if (!f.is_open()) return "";
            std::stringstream buffer;
            buffer << f.rdbuf();
            return buffer.str();
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

        Log::Info("ScriptSystem: Phase 11 Ã¢â‚¬â€ CSM + Gameplay Lua API + Renderer2D Ready.");
        return true;
    }

    void ScriptSystem::ExecuteFile(const std::string& path) {
        // Always load core.lua first (unless we ARE core.lua)
        if (path != "assets/scripts/core.lua") {
            auto coreResult = m_lua.script_file("assets/scripts/core.lua", sol::script_pass_on_error);
            if (!coreResult.valid()) {
                sol::error err = coreResult;
                Log::Error("Core Library Error: " + std::string(err.what()));
            }
        }
        
        // Check if file exists before attempting to load (prevents crash-level errors)
        if (!std::filesystem::exists(path)) {
            Log::Error("Lua Error: cannot open {}: No such file or directory", path);
            return;
        }
        
        auto result = m_lua.script_file(path, sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            Log::Error("Lua Error: " + std::string(err.what()));
        }
    }

    void ScriptSystem::OnUpdate(float dt) {
        // Automatic Timer System Update
        sol::protected_function timerUpdate = m_lua["Timer"]["update"];
        if (timerUpdate.valid()) {
            timerUpdate(dt);
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

    void ScriptSystem::OnRender() {
        sol::protected_function renderFunc = m_lua["OnRender"];
        if (renderFunc.valid()) {
            auto result = renderFunc();
            if (!result.valid()) {
                sol::error err = result;
                Log::Error("Lua OnRender Error: " + std::string(err.what()));
            }
        }
    }

    void ScriptSystem::OnUIRender() {
        // Begin Renderer2D batch for Lua 2D drawing
        Renderer2D::BeginBatch();

        sol::protected_function uiFunc = m_lua["OnRenderUI"];
        if (uiFunc.valid()) {
            auto result = uiFunc();
            if (!result.valid()) {
                sol::error err = result;
                Log::Error("Lua OnRenderUI Error: " + std::string(err.what()));
            }
        }

        // Flush all 2D quads drawn by Lua
        Renderer2D::EndBatch();
    }

}
