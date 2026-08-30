#pragma once
#include <string>

namespace starlight::editor {

class IEditorPanel {
public:
    virtual ~IEditorPanel() = default;

    virtual void OnImGuiRender() = 0;
    virtual const char* GetName() const = 0;

    bool IsOpen() const { return m_isOpen; }
    void SetOpen(bool open) { m_isOpen = open; }
    void ToggleOpen() { m_isOpen = !m_isOpen; }

protected:
    bool m_isOpen = true;
};

} // namespace starlight::editor
