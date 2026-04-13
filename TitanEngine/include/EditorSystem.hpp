#pragma once
#include <entt/entt.hpp>
#include <string>
#include <vector>

namespace titan {

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

    private:
        EditorSystem() = default;
        bool m_active = false;
        entt::entity m_selectedEntity = entt::null;
        
        float m_sidebarWidth = 300.0f;
        float m_browserHeight = 220.0f;

        void DrawHierarchy();
        void DrawInspector();
        void DrawAssetBrowser();
    };

}
