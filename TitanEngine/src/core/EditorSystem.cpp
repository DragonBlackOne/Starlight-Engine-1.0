#include "EditorSystem.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "InputSystem.hpp"
#include "Components.hpp"
#include "PickerSystem.hpp"
#include "Log.hpp"
#include <SDL2/SDL.h>

namespace titan {

    EditorSystem& EditorSystem::Get() {
        static EditorSystem instance;
        return instance;
    }

    void EditorSystem::Initialize() {
        Log::Info("EditorSystem: Titan Studio Framework active. Press F2 to toggle.");
    }

    void EditorSystem::Update(float dt) {
        if (InputSystem::IsKeyPressed(SDL_SCANCODE_F2)) {
            Toggle();
            SDL_SetRelativeMouseMode(m_active ? SDL_FALSE : SDL_TRUE);
            Log::Info(m_active ? "EditorSystem: Studio Mode Active" : "EditorSystem: Game Mode Active");
        }

        if (m_active) {
            int mx, my;
            uint32_t m_state = SDL_GetMouseState(&mx, &my);
            
            // On Left Click: Pick Entity
            static bool wasPressed = false;
            bool isPressed = (m_state & SDL_BUTTON(SDL_BUTTON_LEFT));
            
            // Check if mouse is inside the viewport (not over sidebars)
            int sw = Engine::Get().GetWindow().GetWidth();
            int sh = Engine::Get().GetWindow().GetHeight();
            bool insideViewport = (mx > m_sidebarWidth && mx < (sw - m_sidebarWidth) && my < (sh - m_browserHeight));

            if (isPressed && !wasPressed && insideViewport) {
                auto scene = Engine::Get().GetSceneStack().Active();
                if (scene) {
                    auto& reg = scene->GetRegistry();
                    
                    // Get Camera Matrices
                    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)sw/(float)sh, 0.1f, 1000.0f);
                    glm::mat4 view = glm::mat4(1.0f);
                    
                    auto camView = reg.view<TransformComponent, CameraComponent>();
                    for(auto entity : camView) {
                        if(camView.get<CameraComponent>(entity).primary) {
                            view = glm::inverse(camView.get<TransformComponent>(entity).GetMatrix());
                            break;
                        }
                    }

                    Ray ray = PickerSystem::ScreenPointToRay(mx, my, sw, sh, view, projection);
                    m_selectedEntity = PickerSystem::Pick(ray, reg, Engine::Get().GetRenderer().GetOctree());
                }
            }
            wasPressed = isPressed;
        }
    }

    void EditorSystem::RenderUI() {
        if (!m_active) return;

        auto& renderer = Engine::Get().GetRenderer();
        auto& dashboard = renderer.GetDashboard();
        int sw = Engine::Get().GetWindow().GetWidth();
        int sh = Engine::Get().GetWindow().GetHeight();

        // 1. DOCKING FRAME (Background panels)
        dashboard.Panel(0, 0, m_sidebarWidth, sh - m_browserHeight, glm::vec4(0.1f, 0.1f, 0.12f, 0.98f)); // Left
        dashboard.Panel(sw - m_sidebarWidth, 0, m_sidebarWidth, sh, glm::vec4(0.1f, 0.1f, 0.12f, 0.98f)); // Right
        dashboard.Panel(0, sh - m_browserHeight, sw - m_sidebarWidth, m_browserHeight, glm::vec4(0.08f, 0.08f, 0.1f, 0.98f)); // Bottom

        DrawHierarchy();
        DrawInspector();
        DrawAssetBrowser();
    }

    void EditorSystem::DrawHierarchy() {
        auto& dashboard = Engine::Get().GetRenderer().GetDashboard();
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene) return;

        dashboard.Label("SCENE HIERARCHY", 20, 30, glm::vec4(0.5f, 0.5f, 0.6f, 1.0f));

        auto& registry = scene->GetRegistry();
        int y = 70;
        
        auto view = registry.view<TagComponent>();
        for (auto entity : view) {
            auto& tag = view.get<TagComponent>(entity);
            std::string label = "[" + std::to_string((uint32_t)entity) + "] " + tag.tag;
            
            if (dashboard.Button(label, 10, y, m_sidebarWidth - 20, 25)) {
                m_selectedEntity = entity;
            }
            y += 30;
            if (y > Engine::Get().GetWindow().GetHeight() - m_browserHeight - 40) break;
        }
    }

    void EditorSystem::DrawInspector() {
        auto& dashboard = Engine::Get().GetRenderer().GetDashboard();
        int sw = Engine::Get().GetWindow().GetWidth();
        float x = sw - m_sidebarWidth + 20;

        dashboard.Label("INSPECTOR", x, 30, glm::vec4(0.5f, 0.5f, 0.6f, 1.0f));

        if (m_selectedEntity == entt::null) {
            dashboard.Label("No Entity Selected", x, 70, glm::vec4(1, 1, 1, 0.3f));
            return;
        }

        auto scene = Engine::Get().GetSceneStack().Active();
        auto& registry = scene->GetRegistry();

        if (registry.all_of<TagComponent>(m_selectedEntity)) {
            auto& tag = registry.get<TagComponent>(m_selectedEntity);
            dashboard.Label("Tag: " + tag.tag, x, 70, glm::vec4(1, 1, 0, 1));
        }

        if (registry.all_of<TransformComponent>(m_selectedEntity)) {
            auto& t = registry.get<TransformComponent>(m_selectedEntity);
            dashboard.Label("Transform", x, 110, glm::vec4(0, 1, 1, 1));
            
            char buf[64];
            sprintf(buf, "Pos: %.2f, %.2f, %.2f", t.position.x, t.position.y, t.position.z);
            dashboard.Label(buf, x + 10, 135, glm::vec4(1));
            
            if (dashboard.Button("Move UP", x + 10, 160, 100, 25)) t.position.y += 1.0f;
            if (dashboard.Button("Move DOWN", x + 120, 160, 100, 25)) t.position.y -= 1.0f;
        }

        if (dashboard.Button("DELETE ENTITY", x, 600, m_sidebarWidth - 40, 40)) {
            registry.destroy(m_selectedEntity);
            m_selectedEntity = entt::null;
        }
    }

    void EditorSystem::DrawAssetBrowser() {
        auto& dashboard = Engine::Get().GetRenderer().GetDashboard();
        int sh = Engine::Get().GetWindow().GetHeight();
        float y = sh - m_browserHeight + 30;

        dashboard.Label("ASSET BROWSER", 20, y, glm::vec4(0.5f, 0.5f, 0.6f, 1.0f));
        
        // Mock Asset Grid
        float x = 20;
        std::vector<std::string> assets = {"player.glb", "world.json", "techno.png", "engine.lua", "phys_debug.log"};
        for (const auto& asset : assets) {
            dashboard.Button(asset, x, y + 40, 120, 120);
            x += 140;
        }
    }

}
