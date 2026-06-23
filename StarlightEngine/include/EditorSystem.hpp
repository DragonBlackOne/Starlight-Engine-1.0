#pragma once
#include "EngineSystem.hpp"
#include <glad/glad.h>
#include <entt/entt.hpp>
#include <string>
#include <vector>

namespace starlight {

enum class EditorMode { Editing, Playing, Paused };

class EditorSystem : public ISystem {
public:
    EditorSystem() = default;
    ~EditorSystem();

    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnRender() override;
    void OnUpdate(float dt) override;
    const char* GetName() const override { return "EditorSystem"; }

    bool IsViewportHovered() const { return m_viewportHovered; }
    entt::entity GetSelectedEntity() const { return m_selectedEntity; }
    EditorMode GetMode() const { return m_mode; }

private:
    void CreateViewportFBO(int width, int height);
    void DestroyViewportFBO();
    void DrawMenuBar();
    void OnPlay();
    void OnPauseResume();
    void OnStop();
    void DrawGizmos(entt::registry& registry);
    void ApplyDarkTheme();
    void SetupLuaHotReload();

    uint32_t m_viewportFBO = 0;
    uint32_t m_viewportTex = 0;
    uint32_t m_viewportDepth = 0;
    int m_viewportWidth = 1280;
    int m_viewportHeight = 720;
    bool m_viewportHovered = false;
    bool m_viewportFocused = false;
    entt::entity m_selectedEntity = entt::null;

    bool m_showHierarchy = true;
    bool m_showInspector = true;
    bool m_showContentBrowser = true;
    bool m_showConsole = true;

    EditorMode m_mode = EditorMode::Editing;
    std::string m_sceneFilePath = "scene.scene";
    std::vector<std::string> m_watchedLuaScripts;
    bool m_gizmoDragging = false;
    float m_gizmoDragStartX = 0.0f;
    float m_gizmoDragStartY = 0.0f;
    glm::vec3 m_gizmoEntityStartPos = {0.0f, 0.0f, 0.0f};
};

}
