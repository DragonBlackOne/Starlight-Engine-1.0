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
#include "imgui.h"

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
        m_lua.new_usertype<glm::vec2>("vec2",
            sol::call_constructor, sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>(),
            "new", sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>(),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y
        );

        m_lua.new_usertype<glm::vec3>("vec3",
            sol::call_constructor, sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
            "new", sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z,
            sol::meta_function::addition, [](const glm::vec3& a, const glm::vec3& b) { return a + b; },
            sol::meta_function::subtraction, [](const glm::vec3& a, const glm::vec3& b) { return a - b; },
            sol::meta_function::multiplication, [](const glm::vec3& a, float b) { return a * b; }
        );

        m_lua.new_usertype<glm::quat>("quat",
            sol::call_constructor, sol::constructors<glm::quat(), glm::quat(glm::vec3)>(),
            "new", sol::constructors<glm::quat(), glm::quat(glm::vec3)>(),
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

        // --- IMGUI BINDINGS ---
        auto imgui = m_lua.create_table("imgui");
        imgui.set_function("get_foreground_draw_list", []() { return ImGui::GetForegroundDrawList(); });
        imgui.set_function("color", [](float r, float g, float b, float a) { return ImColor(r, g, b, a); });
        imgui.set_function("begin_window", [](const std::string& name, bool* open, int flags) { return ImGui::Begin(name.c_str(), open, flags); });
        imgui.set_function("end_window", []() { ImGui::End(); });
        imgui.set_function("text", [](const std::string& text) { ImGui::Text("%s", text.c_str()); });
        imgui.set_function("text_colored", [](ImColor col, const std::string& text) { ImGui::TextColored(col, "%s", text.c_str()); });
        imgui.set_function("text_disabled", [](const std::string& text) { ImGui::TextDisabled("%s", text.c_str()); });
        imgui.set_function("separator", []() { ImGui::Separator(); });
        imgui.set_function("spacing", []() { ImGui::Spacing(); });
        imgui.set_function("button", [](const std::string& label, glm::vec2 size) { return ImGui::Button(label.c_str(), {size.x, size.y}); });
        imgui.set_function("set_next_window_pos", [](glm::vec2 pos) { ImGui::SetNextWindowPos({pos.x, pos.y}); });
        imgui.set_function("set_next_window_size", [](glm::vec2 size) { ImGui::SetNextWindowSize({size.x, size.y}); });

        m_lua.new_usertype<ImDrawList>("ImDrawList",
            "add_rect_filled", [](ImDrawList* dl, glm::vec2 min, glm::vec2 max, ImColor col) { dl->AddRectFilled({min.x, min.y}, {max.x, max.y}, col); },
            "add_circle_filled", [](ImDrawList* dl, glm::vec2 center, float rad, ImColor col) { dl->AddCircleFilled({center.x, center.y}, rad, col); },
            "add_line", [](ImDrawList* dl, glm::vec2 p1, glm::vec2 p2, ImColor col) { dl->AddLine({p1.x, p1.y}, {p2.x, p2.y}, col); },
            "add_text", [](ImDrawList* dl, glm::vec2 pos, ImColor col, const std::string& text) { dl->AddText({pos.x, pos.y}, col, text.c_str()); }
        );

        // --- CONVENIENCE WRAPPERS & BEGINNER API (SBA) ---
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

            -- Simple API for Beginners
            function Object(tag, x, y, z)
                local e = Entity.Create()
                SetPos(e, x, y, z)
                e:GetMesh() 
                e.tag = tag or "Object"
                return e
            end

            function SetPos(obj, x, y, z)
                obj:GetTransform().position = vec3(x or 0, y or 0, z or 0)
            end

            function Move(obj, dx, dy, dz)
                local t = obj:GetTransform()
                t.position = t.position + vec3(dx or 0, dy or 0, dz or 0)
            end

            function SetColor(obj, r, g, b)
                local m = Engine:get_registry():get_mesh(obj.id)
                m.material.albedo = vec3(r or 1, g or 1, b or 1)
            end

            function IsDown(key)
                local k = tostring(key):upper()
                local keys = { 
                    UP="W", DOWN="S", LEFT="A", RIGHT="D"
                }
                local action = keys[k] or k
                if action == "SPACE" then action = "Space" end
                if action == "ESCAPE" then action = "Escape" end
                return Engine:get_input():is_pressed(action)
            end

            function Sound(f, d)
                Engine:get_audio():play_note(f or 440, d or 0.1, 0)
            end

            function Distance(o1, o2)
                local p1 = o1:GetTransform().position
                local p2 = o2:GetTransform().position
                return math.sqrt((p1.x-p2.x)^2 + (p1.y-p2.y)^2 + (p1.z-p2.z)^2)
            end

            function SetScale(obj, x, y, z)
                obj:GetTransform().scale = vec3(x or 1, y or 1, z or 1)
            end

            function Rotate(obj, x, y, z)
                local t = obj:GetTransform()
                t.rotation = t.rotation * quat.euler(x or 0, y or 0, z or 0)
            end

            function Destroy(obj)
                Engine:get_registry():destroy(obj.id)
            end

            function GetPos(obj)
                local p = obj:GetTransform().position
                return p.x, p.y, p.z
            end

            function SetLight(obj, r, g, b, intensity)
                local l = obj:GetLight()
                l.color = vec3(r or 1, g or 1, b or 1)
                l.intensity = intensity or 100
            end

            -- 2D Primitive SBA (Uses ImGui)
            function Rect(x, y, w, h, r, g, b, a)
                local dl = imgui.get_foreground_draw_list()
                dl:add_rect_filled(vec2(x, y), vec2(x+w, y+h), imgui.color(r or 1, g or 1, b or 1, a or 1))
            end

            function Circle(x, y, rad, r, g, b, a)
                local dl = imgui.get_foreground_draw_list()
                dl:add_circle_filled(vec2(x, y), rad, imgui.color(r or 1, g or 1, b or 1, a or 1))
            end

            function Line(x1, y1, x2, y2, r, g, b, a)
                local dl = imgui.get_foreground_draw_list()
                dl:add_line(vec2(x1, y1), vec2(x2, y2), imgui.color(r or 1, g or 1, b or 1, a or 1))
            end

            function Say2D(x, y, msg, r, g, b)
                local dl = imgui.get_foreground_draw_list()
                dl:add_text(vec2(x, y), imgui.color(r or 1, g or 1, b or 1, 1), tostring(msg))
            end

            function Say(msg) Log.info(tostring(msg)) end
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

    void ScriptSystem::RenderUI() {
        sol::protected_function renderFunc = m_lua["OnRenderUI"];
        if (renderFunc.valid()) {
            auto result = renderFunc();
            if (!result.valid()) {
                sol::error err = result;
                Log::Error("Lua OnRenderUI Error: " + std::string(err.what()));
            }
        }
    }
}
