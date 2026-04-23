#include "EditorSystem.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "InputSystem.hpp"
#include "Components.hpp"
#include "PickerSystem.hpp"
#include "Log.hpp"
#include <SDL2/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <filesystem>

namespace titan {

    EditorSystem& EditorSystem::Get() {
        static EditorSystem instance;
        return instance;
    }

    void EditorSystem::Initialize() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        
        SetDarkTheme();

        SDL_Window* window = Engine::Get().GetWindow().GetNativeWindow();
        SDL_GLContext gl_context = Engine::Get().GetWindow().GetGLContext();
        
        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init("#version 450 core");

        Log::Info("EditorSystem: ImGui Studio Framework active. Press F2 to toggle.");
    }

    void EditorSystem::Update(float dt) {
        (void)dt;
        if (InputSystem::IsKeyPressed(SDL_SCANCODE_F2)) {
            Toggle();
            SDL_SetRelativeMouseMode(m_active ? SDL_FALSE : SDL_TRUE);
            Log::Info(m_active ? "EditorSystem: Studio Mode Active" : "EditorSystem: Game Mode Active");
        }
        // ... Pick entity logic moved to ImGui Viewport logic or removed if relying on ImGui clicks ...

        if (m_active) {
            int mx, my;
            uint32_t m_state = SDL_GetMouseState(&mx, &my);
            
            // On Left Click: Pick Entity
            static bool wasPressed = false;
            bool isPressed = (m_state & SDL_BUTTON(SDL_BUTTON_LEFT));
            
            // Check if mouse is inside the viewport
            int sw = (int)m_viewportSize.x;
            int sh = (int)m_viewportSize.y;
            
            ImVec2 imguiMouse = ImGui::GetMousePos();
            float relX = imguiMouse.x - m_viewportBounds[0].x;
            float relY = imguiMouse.y - m_viewportBounds[0].y;
            
            bool insideViewport = (sw > 0 && sh > 0 && relX >= 0 && relX <= sw && relY >= 0 && relY <= sh);

            // Mouse Look (Free Camera in Editor)
            static bool s_isMouseLooking = false;
            if (isPressed && insideViewport && !m_playMode) {
                // If RMB is pressed (actually SDL_BUTTON_RIGHT)
                // Let's check RMB specifically
            }
            bool isRMB = (m_state & SDL_BUTTON(SDL_BUTTON_RIGHT));
            if (isRMB && insideViewport) {
                if (!s_isMouseLooking) {
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                    s_isMouseLooking = true;
                }
                
                auto scene = Engine::Get().GetSceneStack().Active();
                if (scene) {
                    auto camView = scene->GetRegistry().view<TransformComponent, CameraComponent>();
                    for(auto entity : camView) {
                        if(camView.get<CameraComponent>(entity).primary) {
                            auto& t = camView.get<TransformComponent>(entity);
                            
                            // Mouse Look Logic
                            int dx, dy;
                            SDL_GetRelativeMouseState(&dx, &dy);
                            
                            glm::vec3 euler = glm::eulerAngles(t.rotation);
                            float pitch = euler.x - dy * 0.005f;
                            float yaw = euler.y - dx * 0.005f;
                            pitch = glm::clamp(pitch, -1.5f, 1.5f);
                            t.rotation = glm::quat(glm::vec3(pitch, yaw, 0.0f));
                            
                            // WASD movement
                            glm::vec3 forward = t.rotation * glm::vec3(0, 0, -1);
                            glm::vec3 right = t.rotation * glm::vec3(1, 0, 0);
                            float speed = 10.0f * dt;
                            if (InputSystem::IsKeyPressed(SDL_SCANCODE_W)) t.position += forward * speed;
                            if (InputSystem::IsKeyPressed(SDL_SCANCODE_S)) t.position -= forward * speed;
                            if (InputSystem::IsKeyPressed(SDL_SCANCODE_A)) t.position -= right * speed;
                            if (InputSystem::IsKeyPressed(SDL_SCANCODE_D)) t.position += right * speed;
                            break;
                        }
                    }
                }
            } else if (s_isMouseLooking) {
                SDL_SetRelativeMouseMode(SDL_FALSE);
                s_isMouseLooking = false;
            }

            if (isPressed && !wasPressed && insideViewport && !isRMB) {
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

                    Ray ray = PickerSystem::ScreenPointToRay((int)relX, (int)relY, sw, sh, view, projection);
                    m_selectedEntity = PickerSystem::Pick(ray, reg, Engine::Get().GetRenderer().GetOctree());
                }
            }
            wasPressed = isPressed;
        }
    }

    void EditorSystem::RenderUI() {
        if (!m_active) return;

        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        DrawMenuBar();
        DrawToolbar();
        DrawViewport();
        DrawHierarchy();
        DrawInspector();
        DrawAssetBrowser();
        DrawConsole();
        DrawPostProcessing();
        DrawProfiler();
    }
    
    void EditorSystem::DrawViewport() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport");
        
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
        
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        
        uint32_t textureID = Engine::Get().GetRenderer().GetSceneTexture();
        ImGui::Image((void*)(intptr_t)textureID, ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        
        static glm::vec2 lastViewportSize = m_viewportSize;
        if (m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f && 
            (lastViewportSize.x != m_viewportSize.x || lastViewportSize.y != m_viewportSize.y)) {
            
            Engine::Get().GetRenderer().ResizeSceneFBO((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
            Engine::Get().GetRenderer().UpdateProjection(60.0f, m_viewportSize.x / m_viewportSize.y, 0.1f, 1000.0f);
            lastViewportSize = m_viewportSize;
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorSystem::DrawHierarchy() {
        auto scene = Engine::Get().GetSceneStack().Active();
        if (!scene) return;

        ImGui::Begin("Hierarchy");

        auto& registry = scene->GetRegistry();
        auto view = registry.view<TagComponent>();
        for (auto entity : view) {
            auto& tag = view.get<TagComponent>(entity);
            std::string label = "[" + std::to_string((uint32_t)entity) + "] " + tag.tag;
            
            if (ImGui::Selectable(label.c_str(), m_selectedEntity == entity)) {
                m_selectedEntity = entity;
            }
        }
        ImGui::End();
    }

    void EditorSystem::DrawInspector() {
        ImGui::Begin("Inspector");

        if (m_selectedEntity == entt::null) {
            ImGui::TextDisabled("No Entity Selected");
            ImGui::End();
            return;
        }

        auto scene = Engine::Get().GetSceneStack().Active();
        auto& registry = scene->GetRegistry();

        if (registry.all_of<TagComponent>(m_selectedEntity)) {
            auto& tag = registry.get<TagComponent>(m_selectedEntity);
            char buf[256];
            strcpy_s(buf, tag.tag.c_str());
            if (ImGui::InputText("Tag", buf, sizeof(buf))) {
                tag.tag = buf;
            }
        }

        if (registry.all_of<TransformComponent>(m_selectedEntity)) {
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto& t = registry.get<TransformComponent>(m_selectedEntity);
                ImGui::DragFloat3("Position", &t.position.x, 0.1f);
                
                glm::vec3 euler = glm::degrees(glm::eulerAngles(t.rotation));
                if (ImGui::DragFloat3("Rotation", &euler.x, 1.0f)) {
                    t.rotation = glm::quat(glm::radians(euler));
                }
                
                ImGui::DragFloat3("Scale", &t.scale.x, 0.1f);
            }
        }

        if (registry.all_of<CameraComponent>(m_selectedEntity)) {
            if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto& cam = registry.get<CameraComponent>(m_selectedEntity);
                ImGui::DragFloat("FOV", &cam.fov, 1.0f, 10.0f, 150.0f);
                ImGui::DragFloat("Near Plane", &cam.nearPlane, 0.01f);
                ImGui::DragFloat("Far Plane", &cam.farPlane, 1.0f);
                ImGui::Checkbox("Primary", &cam.primary);
            }
        }

        if (registry.all_of<PointLightComponent>(m_selectedEntity)) {
            if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto& light = registry.get<PointLightComponent>(m_selectedEntity);
                ImGui::ColorEdit3("Color", &light.color.x);
                ImGui::DragFloat("Intensity", &light.intensity, 1.0f, 0.0f, 10000.0f);
            }
        }

        if (registry.all_of<MeshComponent>(m_selectedEntity)) {
            if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto& mc = registry.get<MeshComponent>(m_selectedEntity);
                ImGui::TextDisabled("Mesh Loaded");
                ImGui::Separator();
                ImGui::Text("Material");
                ImGui::ColorEdit3("Color", &mc.material.color.x);
                ImGui::Checkbox("Use PBR", &mc.material.isPBR);
                if (mc.material.isPBR) {
                    ImGui::ColorEdit3("Albedo", &mc.material.albedo.x);
                    ImGui::SliderFloat("Metallic", &mc.material.metallic, 0.0f, 1.0f);
                    ImGui::SliderFloat("Roughness", &mc.material.roughness, 0.0f, 1.0f);
                    ImGui::SliderFloat("AO", &mc.material.ao, 0.0f, 1.0f);
                }
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Delete Entity", ImVec2(-1, 0))) {
            registry.destroy(m_selectedEntity);
            m_selectedEntity = entt::null;
        }

        ImGui::End();
    }

    void EditorSystem::DrawAssetBrowser() {
        ImGui::Begin("Asset Browser");
        
        if (m_currentAssetDirectory != "assets") {
            if (ImGui::Button("<- Back")) {
                m_currentAssetDirectory = std::filesystem::path(m_currentAssetDirectory).parent_path().string();
            }
            ImGui::Separator();
        }

        float padding = 16.0f;
        float thumbnailSize = 64.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1) columnCount = 1;

        ImGui::Columns(columnCount, 0, false);

        for (auto& directoryEntry : std::filesystem::directory_iterator(m_currentAssetDirectory)) {
            const auto& path = directoryEntry.path();
            auto relativePath = std::filesystem::relative(path, std::filesystem::current_path());
            std::string filenameString = relativePath.filename().string();

            ImGui::PushID(filenameString.c_str());
            
            if (directoryEntry.is_directory()) {
                if (ImGui::Button(filenameString.c_str(), {thumbnailSize, thumbnailSize})) {
                    m_currentAssetDirectory = (std::filesystem::path(m_currentAssetDirectory) / path.filename()).string();
                }
            } else {
                ImGui::Button(filenameString.c_str(), {thumbnailSize, thumbnailSize});
            }

            ImGui::TextWrapped("%s", filenameString.c_str());
            ImGui::NextColumn();
            ImGui::PopID();
        }

        ImGui::Columns(1);
        ImGui::End();
    }

    void EditorSystem::DrawConsole() {
        ImGui::Begin("Console");
        if (ImGui::Button("Clear")) {
            // Not possible directly without adding clear to Log, but we can just leave it
        }
        ImGui::Separator();

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        for (const auto& log : Log::GetHistory()) {
            ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            if (log.find("[WARN]") != std::string::npos) color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
            else if (log.find("[ERROR]") != std::string::npos) color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            else if (log.find("[FATAL]") != std::string::npos) color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            else if (log.find("[INFO]") != std::string::npos) color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
            
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(log.c_str());
            ImGui::PopStyleColor();
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();
        ImGui::End();
    }

    void EditorSystem::DrawMenuBar() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Scene", "Ctrl+N")) {}
                if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {}
                if (ImGui::MenuItem("Exit", "Alt+F4")) { Engine::Get().GetWindow().Shutdown(); }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Show Grid", nullptr, true);
                ImGui::MenuItem("Show Colliders");
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void EditorSystem::DrawToolbar() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        
        auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

        ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        
        float size = ImGui::GetWindowHeight() - 4.0f;
        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 1.5f));
        
        if (m_playMode) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 0.6f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        }
        
        if (ImGui::Button("PLAY", ImVec2(size * 3.0f, size))) {
            m_playMode = !m_playMode;
            Log::Info(m_playMode ? "Editor: PLAY MODE ACTIVE" : "Editor: PLAY MODE STOPPED");
        }
        ImGui::PopStyleColor();

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
        ImGui::End();
    }

    void EditorSystem::SetDarkTheme() {
        auto& colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };
        colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
        colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
        colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    }

    void EditorSystem::DrawPostProcessing() {
        ImGui::Begin("Post-Processing");
        auto& renderer = Engine::Get().GetRenderer();
        
        if (ImGui::CollapsingHeader("Bloom", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat("Threshold", &renderer.m_bloomThreshold, 0.01f, 0.0f, 10.0f);
            ImGui::DragInt("Blur Steps", &renderer.m_bloomBlurSteps, 1, 0, 30);
        }
        
        if (ImGui::CollapsingHeader("Tone Mapping", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat("Exposure", &renderer.m_exposure, 0.01f, 0.1f, 10.0f);
            ImGui::DragFloat("Gamma", &renderer.m_gamma, 0.01f, 0.1f, 5.0f);
        }
        ImGui::End();
    }

    void EditorSystem::DrawProfiler() {
        ImGui::Begin("Profiler");
        ImGui::Text("Application Average: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Separator();
        
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (activeScene) {
            auto& reg = activeScene->GetRegistry();
            int entityCount = (int)reg.storage<entt::entity>().size();
            ImGui::Text("Active Entities: %d", entityCount);
        }
        
        ImGui::Text("Engine Status: %s", m_playMode ? "PLAY MODE" : "EDIT MODE");
        ImGui::End();
    }

}
