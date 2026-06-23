#pragma once
#include "EngineSystem.hpp"
#include "GLResource.hpp"
#include <entt/entt.hpp>
#include <imgui.h>
#include <imgui_node_editor.h>
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
    bool IsMainThreadOnly() const override { return true; }

    void ProcessEvent(const union SDL_Event* event);

    bool IsViewportHovered() const { return m_viewportHovered; }
    entt::entity GetSelectedEntity() const { return m_selectedEntity; }
    EditorMode GetMode() const { return m_mode; }

    void ResetLayout();

    void ReportActiveBtNode(const std::string& name);
    void ClearActiveBtNodes();
    bool IsBtNodeActive(const std::string& name) const;

private:
    void CreateViewportFBO(int width, int height);
    void DestroyViewportFBO();
    void DrawMenuBar();
    void OnPlay();
    void OnPauseResume();
    void OnStop();
    void DrawGizmos(entt::registry& registry, const ImVec2& viewportPos);
    void ApplyDarkTheme();
    void SetupLuaHotReload();
    void BuildDefaultDockLayout(ImGuiID dockspaceID);
    void ConfigureGlobalIniPath();
    void DrawShaderEditor();
    void DrawBehaviorTreeEditor();
    void DrawCVarEditor();
    void DrawProfilerWindow();

    GLFramebuffer m_viewportFBO;
    GLTexture m_viewportTex;
    GLRenderbuffer m_viewportDepth;
    int m_viewportWidth = 1280;
    int m_viewportHeight = 720;
    bool m_viewportHovered = false;
    bool m_viewportFocused = false;
    float m_viewportScreenPosX = 0.0f;
    float m_viewportScreenPosY = 0.0f;
    entt::entity m_selectedEntity = entt::null;

    bool m_showHierarchy = true;
    bool m_showInspector = true;
    bool m_showContentBrowser = true;
    bool m_showConsole = true;
    bool m_showShaderEditor = true;
    bool m_showBTEditor = true;
    bool m_showCVarEditor = true;
    bool m_showProfilerWindow = true;
    bool m_showModernNodeEditor = false;

    // Profiler stats history and min/max track
    float m_frameTimeHistory[120] = {0.0f};
    int m_frameTimeHistoryIdx = 0;
    
    float m_minRender = 9999.0f, m_maxRender = 0.0f;
    float m_minScript = 9999.0f, m_maxScript = 0.0f;
    float m_minPhysics = 9999.0f, m_maxPhysics = 0.0f;
    float m_minAudio = 9999.0f, m_maxAudio = 0.0f;
    float m_minUpdate = 9999.0f, m_maxUpdate = 0.0f;
    float m_minTotal = 9999.0f, m_maxTotal = 0.0f;

    EditorMode m_mode = EditorMode::Editing;
    std::string m_sceneFilePath = "scene.scene";
    std::vector<std::string> m_watchedLuaScripts;
    bool m_gizmoDragging = false;
    float m_gizmoDragStartX = 0.0f;
    float m_gizmoDragStartY = 0.0f;
    glm::vec3 m_gizmoEntityStartPos = {0.0f, 0.0f, 0.0f};

    bool m_layoutInitialized = false;

    // Search and filters
    char m_hierarchySearch[128] = "";
    char m_contentSearch[128] = "";

    // Graph editor pan & zoom
    ImVec2 m_shaderPan = {0.0f, 0.0f};
    float m_shaderZoom = 1.0f;
    ImVec2 m_btPan = {0.0f, 0.0f};
    float m_btZoom = 1.0f;

    // Link drafting
    int m_activePinDragNode = -1;
    bool m_activePinDragOutput = false;
    bool m_isBtDraft = false;

    // Unified Visual Scripting Workspace
    ax::NodeEditor::EditorContext* m_logicEditorContext = nullptr;
    ax::NodeEditor::EditorContext* m_btEditorContext = nullptr;
    ax::NodeEditor::EditorContext* m_shaderEditorContext = nullptr;
    bool m_showUnifiedVisualCoding = true;
    int m_activeVisualCodingTab = 0; // 0 = Logic, 1 = BT, 2 = Shader

    void DrawUnifiedVisualCodingWorkspace();
    void DrawLogicEditorCanvas();
    void DrawBtEditorCanvas();
    void DrawShaderEditorCanvas();
};

}
