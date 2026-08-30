#pragma once
#include "EditorPanel.hpp"
#include "core/ECS/Entity.hpp"
#include "core/ECS/CoreComponents.hpp"

namespace starlight::editor {

class PropertiesPanel : public IEditorPanel {
public:
    PropertiesPanel() = default;

    void SetSelectedEntity(ecs::Entity entity) {
        m_selectedEntity = entity;
    }

    ecs::Entity GetSelectedEntity() const {
        return m_selectedEntity;
    }

    void OnImGuiRender() override {
        // UI rendering hook for ImGui component inspector
    }

    const char* GetName() const override { return "Properties"; }

private:
    ecs::Entity m_selectedEntity;
};

} // namespace starlight::editor
