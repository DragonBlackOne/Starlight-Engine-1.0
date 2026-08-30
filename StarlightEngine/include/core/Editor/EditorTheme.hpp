#pragma once
#include <glm/glm.hpp>

namespace starlight::editor {

struct EditorTheme {
    // Outrun / Synthwave & Dark Slate Palette
    glm::vec4 backgroundDark{ 0.08f, 0.08f, 0.11f, 1.0f };
    glm::vec4 surfaceDark{ 0.12f, 0.12f, 0.16f, 1.0f };
    glm::vec4 surfaceHover{ 0.18f, 0.18f, 0.24f, 1.0f };
    glm::vec4 surfaceActive{ 0.22f, 0.22f, 0.30f, 1.0f };

    // Accents
    glm::vec4 cyberCyan{ 0.0f, 0.85f, 1.0f, 1.0f };
    glm::vec4 hotMagenta{ 1.0f, 0.0f, 0.55f, 1.0f };
    glm::vec4 neonViolet{ 0.6f, 0.1f, 1.0f, 1.0f };
    glm::vec4 sunsetOrange{ 1.0f, 0.45f, 0.0f, 1.0f };
    glm::vec4 goldAccent{ 1.0f, 0.8f, 0.0f, 1.0f };

    // Text & Controls
    glm::vec4 textPrimary{ 0.95f, 0.95f, 0.98f, 1.0f };
    glm::vec4 textSecondary{ 0.60f, 0.60f, 0.68f, 1.0f };
    glm::vec4 textDisabled{ 0.35f, 0.35f, 0.42f, 1.0f };

    // Metrics
    float windowRounding = 6.0f;
    float frameRounding = 4.0f;
    float popupRounding = 6.0f;
    float grabRounding = 3.0f;
    float tabRounding = 4.0f;
};

} // namespace starlight::editor
