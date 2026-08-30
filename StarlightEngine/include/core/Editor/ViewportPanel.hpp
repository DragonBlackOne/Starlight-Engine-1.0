#pragma once
#include "EditorPanel.hpp"
#include <glm/glm.hpp>

namespace starlight::editor {

enum class ViewportState {
    Edit = 0,
    Play,
    Pause,
    Simulate
};

class ViewportPanel : public IEditorPanel {
public:
    ViewportPanel() = default;

    void SetViewportSize(uint32_t width, uint32_t height) {
        m_width = width;
        m_height = height;
    }

    glm::uvec2 GetViewportSize() const { return { m_width, m_height }; }
    float GetAspectRatio() const {
        return m_height > 0 ? static_cast<float>(m_width) / static_cast<float>(m_height) : 1.0f;
    }

    bool IsHovered() const { return m_isHovered; }
    void SetHovered(bool hovered) { m_isHovered = hovered; }

    bool IsFocused() const { return m_isFocused; }
    void SetFocused(bool focused) { m_isFocused = focused; }

    ViewportState GetState() const { return m_state; }
    void SetState(ViewportState state) { m_state = state; }

    void OnImGuiRender() override {
        // UI rendering hook for ImGui Viewport
    }

    const char* GetName() const override { return "Viewport"; }

private:
    uint32_t m_width = 1280;
    uint32_t m_height = 720;
    bool m_isHovered = false;
    bool m_isFocused = false;
    ViewportState m_state = ViewportState::Edit;
};

} // namespace starlight::editor
