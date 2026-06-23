#include "EditorSystem.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Log.hpp"
#include "CoreMinimal.hpp"
#include "Components.hpp"
#include "SpriteAnimationComponent.hpp"
#include "SceneSerializer.hpp"
#include "FileWatcher.hpp"
#include "ScriptSystem.hpp"
#include <json.hpp>
#include <imgui.h>
#include <filesystem>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>
#include <deque>
#include <algorithm>

namespace fs = std::filesystem;

namespace starlight {
namespace {

struct ConsoleEntry {
    LogLevel level;
    std::string message;
};

std::deque<ConsoleEntry> s_consoleBuffer;
constexpr size_t MAX_CONSOLE_LINES = 500;

nlohmann::json s_sandboxSnapshot;

void ConsoleSink(LogLevel level, const std::string& msg) {
    s_consoleBuffer.push_back({level, msg});
    if (s_consoleBuffer.size() > MAX_CONSOLE_LINES) {
        s_consoleBuffer.pop_front();
    }
}

void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float speed = 0.1f) {
    ImGui::PushID(label.c_str());
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 80);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));
    float lineHeight = ImGui::GetFontSize() + 6;
    ImVec2 buttonSize = ImVec2(lineHeight + 3, lineHeight);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
    if (ImGui::Button("X", buttonSize)) { values.x = resetValue; }
    ImGui::PopStyleColor(2);
    ImGui::SameLine();
    ImGui::DragFloat("##x", &values.x, speed, 0.0f, 0.0f, "%.2f");
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.8f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.9f, 0.2f, 1.0f));
    if (ImGui::Button("Y", buttonSize)) { values.y = resetValue; }
    ImGui::PopStyleColor(2);
    ImGui::SameLine();
    ImGui::DragFloat("##y", &values.y, speed, 0.0f, 0.0f, "%.2f");
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.9f, 1.0f));
    if (ImGui::Button("Z", buttonSize)) { values.z = resetValue; }
    ImGui::PopStyleColor(2);
    ImGui::SameLine();
    ImGui::DragFloat("##z", &values.z, speed, 0.0f, 0.0f, "%.2f");

    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();
}

void DrawFloatControl(const std::string& label, float& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f) {
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 80);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();
    if (min != max) {
        ImGui::DragFloat(("##" + label).c_str(), &value, speed, min, max);
    } else {
        ImGui::DragFloat(("##" + label).c_str(), &value, speed);
    }
    ImGui::Columns(1);
}

void DrawColorControl(const std::string& label, glm::vec4& color) {
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 80);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();
    ImGui::ColorEdit4(("##" + label).c_str(), &color.x);
    ImGui::Columns(1);
}

void DrawBoolControl(const std::string& label, bool& value) {
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 80);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();
    ImGui::Checkbox(("##" + label).c_str(), &value);
    ImGui::Columns(1);
}

void DrawIntControl(const std::string& label, int& value, int speed = 1) {
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 80);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();
    ImGui::DragInt(("##" + label).c_str(), &value, (float)speed);
    ImGui::Columns(1);
}

} // anonymous namespace

EditorSystem::~EditorSystem() {
    DestroyViewportFBO();
}

bool EditorSystem::OnInitialize(const EngineContext& context) {
    Log::Info("EditorSystem: Initializing editor framework");

    ApplyDarkTheme();

    int w = context.window->GetWidth();
    int h = context.window->GetHeight();
    CreateViewportFBO(w, h);

    auto* renderer = context.engine->GetSystem<Renderer>();
    if (renderer) {
        renderer->SetViewportOverride(m_viewportFBO, m_viewportWidth, m_viewportHeight);
    }

    SetupLuaHotReload();

    return true;
}

void EditorSystem::OnShutdown() {
    DestroyViewportFBO();
}

void EditorSystem::OnUpdate(float dt) {
    (void)dt;
}

void EditorSystem::DrawMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
                ImGui::OpenPopup("Save Scene As");
            }
            if (ImGui::MenuItem("Load Scene", "Ctrl+O")) {
                ImGui::OpenPopup("Load Scene");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                SDL_Event quit;
                quit.type = SDL_QUIT;
                SDL_PushEvent(&quit);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Hierarchy", "", &m_showHierarchy);
            ImGui::MenuItem("Inspector", "", &m_showInspector);
            ImGui::MenuItem("Content Browser", "", &m_showContentBrowser);
            ImGui::MenuItem("Console", "", &m_showConsole);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // Toolbar
    ImGui::Separator();
    bool canPlay = (m_mode == EditorMode::Editing);
    bool canStop = (m_mode != EditorMode::Editing);

    ImGui::BeginDisabled(!canPlay);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 0.8f));
    if (ImGui::Button("Play")) { OnPlay(); }
    ImGui::PopStyleColor(1);
    ImGui::EndDisabled();
    ImGui::SameLine();

    bool canPause = (m_mode == EditorMode::Playing || m_mode == EditorMode::Paused);
    ImGui::BeginDisabled(!canPause);
    const char* pauseLabel = (m_mode == EditorMode::Paused) ? "Resume" : "Pause";
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.7f, 0.2f, 0.8f));
    if (ImGui::Button(pauseLabel)) { OnPauseResume(); }
    ImGui::PopStyleColor(1);
    ImGui::EndDisabled();
    ImGui::SameLine();

    ImGui::BeginDisabled(!canStop);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 0.8f));
    if (ImGui::Button("Stop")) { OnStop(); }
    ImGui::PopStyleColor(1);
    ImGui::EndDisabled();

    ImGui::SameLine();
    const char* modeLabels[] = { "Editing", "Playing", "Paused" };
    int modeIdx = static_cast<int>(m_mode);
    ImVec4 modeColor;
    switch (m_mode) {
        case EditorMode::Editing: modeColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); break;
        case EditorMode::Playing: modeColor = ImVec4(0.2f, 0.8f, 0.2f, 1.0f); break;
        case EditorMode::Paused:  modeColor = ImVec4(0.8f, 0.8f, 0.2f, 1.0f); break;
    }
    ImGui::SameLine();
    ImGui::Text("  ");
    ImGui::SameLine();
    ImGui::TextColored(modeColor, "%s", modeLabels[modeIdx]);
}

void EditorSystem::OnPlay() {
    if (m_mode == EditorMode::Playing) return;

    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (!activeScene) return;

    auto& registry = activeScene->GetRegistry();
    s_sandboxSnapshot.clear();
    SceneSerializer::SaveToJson(registry, s_sandboxSnapshot);

    m_mode = EditorMode::Playing;
    Log::Info("Sandbox: Play mode started");
}

void EditorSystem::OnPauseResume() {
    if (m_mode == EditorMode::Playing) {
        m_mode = EditorMode::Paused;
        Engine::Get().SetTimeScale(0.0f);
        Log::Info("Sandbox: Paused");
    } else if (m_mode == EditorMode::Paused) {
        m_mode = EditorMode::Playing;
        Engine::Get().SetTimeScale(1.0f);
        Log::Info("Sandbox: Resumed");
    }
}

void EditorSystem::OnStop() {
    if (m_mode == EditorMode::Editing) return;

    m_mode = EditorMode::Editing;
    Engine::Get().SetTimeScale(1.0f);

    auto activeScene = Engine::Get().GetSceneStack().Active();
    if (activeScene && !s_sandboxSnapshot.empty()) {
        auto& registry = activeScene->GetRegistry();
        registry.clear();
        SceneSerializer::LoadFromJson(registry, s_sandboxSnapshot);
        s_sandboxSnapshot.clear();
        Log::Info("Sandbox: Stopped, snapshot restored");
    }

    m_selectedEntity = entt::null;
}

void EditorSystem::ApplyDarkTheme() {
    ImGui::StyleColorsDark();
    auto& style = ImGui::GetStyle();
    auto& colors = style.Colors;

    colors[ImGuiCol_WindowBg]          = ImVec4(0.13f, 0.13f, 0.14f, 1.00f);
    colors[ImGuiCol_MenuBarBg]         = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_TitleBg]           = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    colors[ImGuiCol_TitleBgActive]     = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_FrameBg]           = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]    = ImVec4(0.24f, 0.24f, 0.26f, 1.00f);
    colors[ImGuiCol_FrameBgActive]     = ImVec4(0.28f, 0.28f, 0.30f, 1.00f);
    colors[ImGuiCol_Header]            = ImVec4(0.22f, 0.22f, 0.24f, 1.00f);
    colors[ImGuiCol_HeaderHovered]     = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
    colors[ImGuiCol_HeaderActive]      = ImVec4(0.35f, 0.35f, 0.37f, 1.00f);
    colors[ImGuiCol_Button]            = ImVec4(0.22f, 0.22f, 0.24f, 1.00f);
    colors[ImGuiCol_ButtonHovered]     = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
    colors[ImGuiCol_ButtonActive]      = ImVec4(0.35f, 0.35f, 0.37f, 1.00f);
    colors[ImGuiCol_CheckMark]         = ImVec4(0.30f, 0.60f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab]        = ImVec4(0.30f, 0.60f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]  = ImVec4(0.40f, 0.70f, 1.00f, 1.00f);
    colors[ImGuiCol_Separator]         = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
    colors[ImGuiCol_Tab]               = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TabHovered]        = ImVec4(0.24f, 0.24f, 0.26f, 1.00f);
    colors[ImGuiCol_TabActive]         = ImVec4(0.20f, 0.45f, 0.80f, 1.00f);
    colors[ImGuiCol_DockingPreview]    = ImVec4(0.30f, 0.60f, 1.00f, 0.50f);
    colors[ImGuiCol_TextSelectedBg]    = ImVec4(0.30f, 0.60f, 1.00f, 0.35f);
    colors[ImGuiCol_NavHighlight]      = ImVec4(0.30f, 0.60f, 1.00f, 1.00f);

    style.WindowRounding = 4.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.WindowMenuButtonPosition = ImGuiDir_None;
}

void EditorSystem::SetupLuaHotReload() {
    auto* fileWatcher = Engine::Get().GetSystem<FileWatcher>();
    if (!fileWatcher) return;

    auto watchDir = std::filesystem::current_path() / "assets" / "scripts";
    if (!std::filesystem::exists(watchDir)) return;

    for (auto& entry : std::filesystem::directory_iterator(watchDir)) {
        if (entry.path().extension() == ".lua") {
            std::string path = entry.path().string();
            m_watchedLuaScripts.push_back(path);
            fileWatcher->AddWatch(path, [](const std::string& changedPath) {
                Log::Info("Hot-Reload: Lua script changed, re-executing: " + changedPath);
                auto* scripting = Engine::Get().GetSystem<ScriptSystem>();
                if (scripting) {
                    scripting->ExecuteFile(changedPath);
                }
            });
        }
    }

    if (!m_watchedLuaScripts.empty()) {
        Log::Info("EditorSystem: Lua hot-reload active for " + std::to_string(m_watchedLuaScripts.size()) + " scripts");
    }
}

void EditorSystem::DrawGizmos(entt::registry& registry) {
    if (m_mode != EditorMode::Editing) return;
    if (m_selectedEntity == entt::null) return;
    if (!registry.all_of<TransformComponent>(m_selectedEntity)) return;

    auto& tc = registry.get<TransformComponent>(m_selectedEntity);
    ImVec2 viewportMin = ImGui::GetWindowPos();
    ImVec2 viewportSize = ImGui::GetWindowSize();

    // Calculate camera info for world-to-screen mapping
    float camZoom = 1.0f;
    glm::vec2 camPos = {0.0f, 0.0f};
    auto camView = registry.view<Camera2DComponent, TransformComponent>();
    for (auto camEntity : camView) {
        auto& cam = camView.get<Camera2DComponent>(camEntity);
        if (cam.primary) {
            camZoom = cam.zoom;
            auto& camTransform = camView.get<TransformComponent>(camEntity);
            camPos = {camTransform.position.x, camTransform.position.y};
            break;
        }
    }

    float aspect = (float)m_viewportWidth / (float)m_viewportHeight;
    float halfWidth = (float)m_viewportWidth * 0.5f / camZoom;
    float halfHeight = (float)m_viewportHeight * 0.5f / camZoom;
    if (aspect > 1.0f) {
        halfWidth *= aspect;
    } else {
        halfHeight /= aspect;
    }

    // World -> Screen
    auto worldToScreen = [&](const glm::vec2& world) -> ImVec2 {
        float sx = (world.x - camPos.x + halfWidth) / (halfWidth * 2.0f) * (float)m_viewportWidth;
        float sy = (1.0f - (world.y - camPos.y + halfHeight) / (halfHeight * 2.0f)) * (float)m_viewportHeight;
        return ImVec2(viewportMin.x + sx, viewportMin.y + sy);
    };

    // Screen -> World
    auto screenToWorld = [&](const ImVec2& screen) -> glm::vec2 {
        float sx = screen.x - viewportMin.x;
        float sy = screen.y - viewportMin.y;
        float wx = (sx / (float)m_viewportWidth) * halfWidth * 2.0f - halfWidth + camPos.x;
        float wy = (1.0f - sy / (float)m_viewportHeight) * halfHeight * 2.0f - halfHeight + camPos.y;
        return {wx, wy};
    };

    glm::vec2 entityPos2D = {tc.position.x, tc.position.y};
    ImVec2 screenPos = worldToScreen(entityPos2D);

    // Clamp to viewport bounds
    if (screenPos.x < viewportMin.x || screenPos.x > viewportMin.x + (float)m_viewportWidth ||
        screenPos.y < viewportMin.y || screenPos.y > viewportMin.y + (float)m_viewportHeight) {
        return;
    }

    // Draw the gizmo handle
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float handleRadius = 7.0f;
    ImU32 handleColor = IM_COL32(255, 200, 50, 220);
    ImU32 handleColorHover = IM_COL32(255, 230, 100, 255);
    ImU32 handleOutline = IM_COL32(0, 0, 0, 180);

    ImVec2 handleMin(screenPos.x - handleRadius, screenPos.y - handleRadius);
    ImVec2 handleMax(screenPos.x + handleRadius, screenPos.y + handleRadius);

    bool hovered = ImGui::IsMouseHoveringRect(handleMin, handleMax) && m_viewportHovered;

    // Draw crosshair
    drawList->AddLine(
        ImVec2(screenPos.x - handleRadius * 1.5f, screenPos.y),
        ImVec2(screenPos.x + handleRadius * 1.5f, screenPos.y),
        hovered ? handleColorHover : handleColor, 2.0f
    );
    drawList->AddLine(
        ImVec2(screenPos.x, screenPos.y - handleRadius * 1.5f),
        ImVec2(screenPos.x, screenPos.y + handleRadius * 1.5f),
        hovered ? handleColorHover : handleColor, 2.0f
    );
    drawList->AddCircleFilled(screenPos, 4.0f, hovered ? handleColorHover : handleColor);
    drawList->AddCircle(screenPos, handleRadius, handleOutline, 0, 2.0f);

    // Dragging logic
    if (hovered && ImGui::IsMouseClicked(0)) {
        m_gizmoDragging = true;
        m_gizmoDragStartX = ImGui::GetMousePos().x;
        m_gizmoDragStartY = ImGui::GetMousePos().y;
        m_gizmoEntityStartPos = tc.position;
    }

    if (m_gizmoDragging) {
        if (ImGui::IsMouseDragging(0, 0.0f)) {
            ImVec2 currentMouse = ImGui::GetMousePos();
            ImVec2 deltaScreen(currentMouse.x - m_gizmoDragStartX, currentMouse.y - m_gizmoDragStartY);
            ImVec2 refScreen(m_gizmoDragStartX + deltaScreen.x, m_gizmoDragStartY + deltaScreen.y);
            glm::vec2 currentWorld = screenToWorld(refScreen);
            glm::vec2 startWorld = screenToWorld(ImVec2(m_gizmoDragStartX, m_gizmoDragStartY));
            glm::vec2 deltaWorld = currentWorld - startWorld;
            tc.position.x = m_gizmoEntityStartPos.x + deltaWorld.x;
            tc.position.y = m_gizmoEntityStartPos.y + deltaWorld.y;
        } else {
            m_gizmoDragging = false;
        }
    }

    if (hovered) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
}

void EditorSystem::CreateViewportFBO(int width, int height) {
    DestroyViewportFBO();

    width = (width < 1) ? 1 : width;
    height = (height < 1) ? 1 : height;

    glGenFramebuffers(1, &m_viewportFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_viewportFBO);

    glGenTextures(1, &m_viewportTex);
    glBindTexture(GL_TEXTURE_2D, m_viewportTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_viewportTex, 0);

    glGenRenderbuffers(1, &m_viewportDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_viewportDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_viewportDepth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        Log::Error("EditorSystem: Viewport FBO incomplete (status: {})", (int)status);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_viewportWidth = width;
    m_viewportHeight = height;
}

void EditorSystem::DestroyViewportFBO() {
    if (m_viewportFBO) {
        glDeleteFramebuffers(1, &m_viewportFBO);
        m_viewportFBO = 0;
    }
    if (m_viewportTex) {
        glDeleteTextures(1, &m_viewportTex);
        m_viewportTex = 0;
    }
    if (m_viewportDepth) {
        glDeleteRenderbuffers(1, &m_viewportDepth);
        m_viewportDepth = 0;
    }
}

void EditorSystem::OnRender() {
    auto* renderer = Engine::Get().GetSystem<Renderer>();

    // Main Dockspace
    ImGuiWindowFlags dockspaceFlags = ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("DockSpace", nullptr, dockspaceFlags);
    ImGui::PopStyleVar(2);

    ImGuiID dockspaceID = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspaceID);

    auto activeScene = Engine::Get().GetSceneStack().Active();
    entt::registry* registryPtr = activeScene ? &activeScene->GetRegistry() : nullptr;

    DrawMenuBar();

    // === Viewport Panel ===
    ImGui::Begin("Viewport");
    m_viewportHovered = ImGui::IsWindowHovered();
    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    if (contentSize.x > 0 && contentSize.y > 0) {
        int newW = (int)contentSize.x;
        int newH = (int)contentSize.y;
        if (newW != m_viewportWidth || newH != m_viewportHeight) {
            CreateViewportFBO(newW, newH);
            if (renderer) {
                renderer->SetViewportOverride(m_viewportFBO, m_viewportWidth, m_viewportHeight);
            }
        }

        if (m_viewportTex) {
            ImGui::Image((ImTextureID)(uint64_t)m_viewportTex, contentSize, ImVec2(0, 1), ImVec2(1, 0));
            if (registryPtr) {
                DrawGizmos(*registryPtr);
            }
        }
    }
    ImGui::End();

    // === Hierarchy Panel ===
    if (m_showHierarchy) {
        ImGui::Begin("Hierarchy", &m_showHierarchy);
        if (registryPtr) {
            auto& entityStorage = registryPtr->storage<entt::entity>();
            for (auto entityIter = entityStorage.begin(); entityIter != entityStorage.end(); ++entityIter) {
                entt::entity e = *entityIter;
                bool isSelected = (e == m_selectedEntity);
                std::string name = "Entity " + std::to_string((uint32_t)e);
                if (registryPtr->all_of<TagComponent>(e)) {
                    name = registryPtr->get<TagComponent>(e).tag;
                }
                ImGui::PushID((void*)(uint64_t)(uint32_t)e);
                if (ImGui::Selectable(name.c_str(), &isSelected)) {
                    m_selectedEntity = e;
                }
                // Right-click context menu (only in Editing mode)
                bool isEditing = (m_mode == EditorMode::Editing);
                if (isEditing && ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Delete Entity")) {
                        registryPtr->destroy(e);
                        if (m_selectedEntity == e) m_selectedEntity = entt::null;
                    }
                    ImGui::EndPopup();
                }
                ImGui::PopID();
            }

            // Right-click on empty space (only in Editing mode)
            if (m_mode == EditorMode::Editing && ImGui::BeginPopupContextWindow("HierarchyContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
                if (ImGui::MenuItem("Create Empty Entity")) {
                    entt::entity newEntity = registryPtr->create();
                    registryPtr->emplace<TransformComponent>(newEntity);
                    registryPtr->emplace<TagComponent>(newEntity);
                    m_selectedEntity = newEntity;
                }
                if (ImGui::MenuItem("Create Sprite Entity")) {
                    entt::entity newEntity = registryPtr->create();
                    registryPtr->emplace<TransformComponent>(newEntity);
                    registryPtr->emplace<SpriteComponent>(newEntity);
                    registryPtr->emplace<TagComponent>(newEntity, "Sprite");
                    m_selectedEntity = newEntity;
                }
                if (ImGui::MenuItem("Create Camera 2D")) {
                    entt::entity newEntity = registryPtr->create();
                    registryPtr->emplace<TransformComponent>(newEntity);
                    registryPtr->emplace<Camera2DComponent>(newEntity);
                    registryPtr->emplace<TagComponent>(newEntity, "Camera2D");
                    m_selectedEntity = newEntity;
                }
                if (ImGui::MenuItem("Create Animated Sprite")) {
                    entt::entity newEntity = registryPtr->create();
                    registryPtr->emplace<TransformComponent>(newEntity);
                    registryPtr->emplace<SpriteComponent>(newEntity);
                    registryPtr->emplace<SpriteAnimationComponent>(newEntity);
                    registryPtr->emplace<TagComponent>(newEntity, "AnimatedSprite");
                    m_selectedEntity = newEntity;
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }

    // === Inspector Panel ===
    if (m_showInspector && registryPtr && m_selectedEntity != entt::null) {
        ImGui::Begin("Inspector", &m_showInspector);
        entt::entity selected = m_selectedEntity;

        bool isReadOnly = (m_mode != EditorMode::Editing);

        // Tag
        if (registryPtr->all_of<TagComponent>(selected)) {
            auto& tag = registryPtr->get<TagComponent>(selected).tag;
            char buf[256];
            strcpy_s(buf, sizeof(buf), tag.c_str());
            if (isReadOnly) {
                ImGui::Text("Tag: %s", tag.c_str());
            } else if (ImGui::InputText("Tag", buf, sizeof(buf))) {
                tag = buf;
            }
        }

        ImGui::Separator();
        ImGui::Text("Components");
        ImGui::Separator();

        ImGui::BeginDisabled(isReadOnly);

        // TransformComponent
        if (registryPtr->all_of<TransformComponent>(selected)) {
            auto& tc = registryPtr->get<TransformComponent>(selected);
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawVec3Control("Position", tc.position, 0.0f);
                glm::vec3 euler = glm::eulerAngles(tc.rotation);
                glm::vec3 eulerDeg = glm::degrees(euler);
                DrawVec3Control("Rotation", eulerDeg, 0.0f);
                tc.rotation = glm::quat(glm::radians(eulerDeg));
                DrawVec3Control("Scale", tc.scale, 1.0f);
                ImGui::Unindent(8);
            }
        }

        // SpriteComponent
        if (registryPtr->all_of<SpriteComponent>(selected)) {
            auto& sc = registryPtr->get<SpriteComponent>(selected);
            if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawColorControl("Color", sc.color);
                DrawFloatControl("Pivot X", sc.pivot.x, 0.01f, 0.0f, 1.0f);
                DrawFloatControl("Pivot Y", sc.pivot.y, 0.01f, 0.0f, 1.0f);
                DrawIntControl("Layer", sc.layer);
                DrawIntControl("Order", sc.orderInLayer);
                DrawBoolControl("Flip X", sc.flipX);
                DrawBoolControl("Flip Y", sc.flipY);
                DrawBoolControl("Visible", sc.visible);
                ImGui::Unindent(8);
            }
        }

        // Camera2DComponent
        if (registryPtr->all_of<Camera2DComponent>(selected)) {
            auto& cc = registryPtr->get<Camera2DComponent>(selected);
            if (ImGui::CollapsingHeader("Camera 2D", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                DrawFloatControl("Zoom", cc.zoom, 0.1f, 0.01f, 100.0f);
                DrawFloatControl("Rotation", cc.rotation, 0.5f);
                DrawBoolControl("Primary", cc.primary);
                ImGui::Unindent(8);
            }
        }

        // SpriteAnimationComponent
        if (registryPtr->all_of<SpriteAnimationComponent>(selected)) {
            auto& ac = registryPtr->get<SpriteAnimationComponent>(selected);
            if (ImGui::CollapsingHeader("Sprite Animation", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                ImGui::Text("Frame: %d / %d", ac.currentFrame + 1, (int)ac.frames.size());
                DrawFloatControl("FPS", ac.fps, 0.5f, 0.1f, 120.0f);
                DrawBoolControl("Playing", ac.playing);
                DrawBoolControl("Looping", ac.looping);
                if (ImGui::Button("Reset")) {
                    ac.currentFrame = 0;
                    ac.timer = 0.0f;
                }
                ImGui::Unindent(8);
            }
        }

        // TilemapComponent
        if (registryPtr->all_of<TilemapComponent>(selected)) {
            auto& tc = registryPtr->get<TilemapComponent>(selected);
            if (ImGui::CollapsingHeader("Tilemap", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(8);
                ImGui::Text("Map: %d x %d", tc.mapWidth, tc.mapHeight);
                DrawIntControl("Tile Size", tc.tileSize);
                DrawIntControl("Tileset Cols", tc.tilesetColumns);
                DrawIntControl("Tileset Rows", tc.tilesetRows);
                DrawColorControl("Color", tc.color);
                DrawIntControl("Layer", tc.layer);
                ImGui::Unindent(8);
            }
        }

        ImGui::EndDisabled();
        ImGui::End();
    }

    // === Content Browser Panel ===
    if (m_showContentBrowser) {
        ImGui::Begin("Content Browser", &m_showContentBrowser);
        static fs::path currentPath = fs::current_path();

        if (ImGui::Button("< ..")) {
            currentPath = currentPath.parent_path();
        }
        ImGui::SameLine();
        ImGui::Text("%s", currentPath.string().c_str());
        ImGui::Separator();

        try {
            for (auto& entry : fs::directory_iterator(currentPath)) {
                auto filename = entry.path().filename().string();
                if (entry.is_directory()) {
                    if (ImGui::Selectable(("[" + filename + "]").c_str())) {
                        currentPath = entry.path();
                    }
                } else {
                    ImGui::Selectable(filename.c_str());
                }
            }
        } catch (...) {
            ImGui::Text("(error reading directory)");
        }
        ImGui::End();
    }

    // === Console Panel ===
    if (m_showConsole) {
        ImGui::Begin("Console", &m_showConsole);
        if (ImGui::Button("Clear")) {
            s_consoleBuffer.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("Copy All")) {
            std::string all;
            for (auto& entry : s_consoleBuffer) {
                all += entry.message + "\n";
            }
            ImGui::SetClipboardText(all.c_str());
        }
        ImGui::Separator();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        for (auto& entry : s_consoleBuffer) {
            ImVec4 color;
            switch (entry.level) {
                case LogLevel::Info:  color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); break;
                case LogLevel::Warn:  color = ImVec4(1.0f, 0.8f, 0.0f, 1.0f); break;
                case LogLevel::Error: color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f); break;
                default:              color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); break;
            }
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(entry.message.c_str());
            ImGui::PopStyleColor();
        }

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
        ImGui::End();
    }

    // === Save Scene As dialog ===
    if (ImGui::BeginPopupModal("Save Scene As", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char pathBuf[512] = "scene.scene";
        ImGui::InputText("Path", pathBuf, sizeof(pathBuf));
        ImGui::Text("Save in assets/scenes/ relative to project");
        if (ImGui::Button("Save")) {
            auto sceneForSave = Engine::Get().GetSceneStack().Active();
            if (sceneForSave) {
                SceneSerializer::SaveToFile(*sceneForSave, pathBuf);
                m_sceneFilePath = pathBuf;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // === Load Scene dialog ===
    if (ImGui::BeginPopupModal("Load Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char pathBuf[512] = "scene.scene";
        ImGui::InputText("Path", pathBuf, sizeof(pathBuf));
        if (ImGui::Button("Load")) {
            auto sceneForLoad = Engine::Get().GetSceneStack().Active();
            if (sceneForLoad) {
                if (m_mode != EditorMode::Editing) {
                    OnStop();
                }
                SceneSerializer::LoadFromFile(*sceneForLoad, pathBuf);
                m_sceneFilePath = pathBuf;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End(); // DockSpace

    // Restore default framebuffer for ImGui final render
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
