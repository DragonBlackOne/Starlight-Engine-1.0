// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <entt/entt.hpp>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace starlight {

    class EditorSystem {
    public:
        static EditorSystem& Get();

        void Initialize();
        void Update(float dt);
        void RenderUI();

        bool IsActive() const { return m_active; }
        void Toggle() { m_active = !m_active; }

        entt::entity GetSelectedEntity() const { return m_selectedEntity; }
        void SetSelectedEntity(entt::entity entity) { m_selectedEntity = entity; }

        bool IsPlayMode() const { return m_playMode; }
        void SetPlayMode(bool play) { m_playMode = play; }

        glm::vec2 GetViewportMin() const { return m_viewportBounds[0]; }
        glm::vec2 GetViewportMax() const { return m_viewportBounds[1]; }
        glm::vec2 GetViewportSize() const { return m_viewportSize; }

    private:
        EditorSystem() = default;
        bool m_active = true;
        bool m_playMode = false;
        entt::entity m_selectedEntity = entt::null;
        
        float m_sidebarWidth = 300.0f;
        float m_browserHeight = 220.0f;
        std::string m_currentAssetDirectory = "assets";
        glm::vec2 m_viewportBounds[2];
        glm::vec2 m_viewportSize = {1280.0f, 720.0f};

        void DrawMenuBar();
        void DrawToolbar();
        void DrawViewport();
        void DrawHierarchy();
        void DrawInspector();
        void DrawAssetBrowser();
        void DrawConsole();
        void DrawPostProcessing();
        void DrawProfiler();
        void SetDarkTheme();
    };

}
