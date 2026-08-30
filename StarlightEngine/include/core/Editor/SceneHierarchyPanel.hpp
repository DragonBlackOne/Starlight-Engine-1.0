#pragma once
#include "EditorPanel.hpp"
#include "core/Scene/Scene.hpp"
#include "core/ECS/Entity.hpp"

namespace starlight::editor {

class SceneHierarchyPanel : public IEditorPanel {
public:
    SceneHierarchyPanel() = default;
    explicit SceneHierarchyPanel(std::shared_ptr<ecs::Scene> context)
        : m_context(std::move(context)) {}

    void SetContext(std::shared_ptr<ecs::Scene> context) {
        m_context = context;
        m_selectedEntity = ecs::Entity();
    }

    void SetSelectedEntity(ecs::Entity entity) {
        m_selectedEntity = entity;
    }

    ecs::Entity GetSelectedEntity() const {
        return m_selectedEntity;
    }

    void OnImGuiRender() override {
        // UI rendering hook for ImGui viewport dock
    }

    const char* GetName() const override { return "Scene Hierarchy"; }

private:
    std::shared_ptr<ecs::Scene> m_context;
    ecs::Entity m_selectedEntity;
};

} // namespace starlight::editor
