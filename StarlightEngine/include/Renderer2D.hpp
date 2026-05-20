// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Shader.hpp"

namespace starlight {

    struct Vertex2D {
        glm::vec2 position;
        glm::vec2 texCoord;
        glm::vec4 color;
        float textureIndex;
        float flags; // 0: Normal, 1: Isometric Tile Mask
    };

    class Renderer2D {
    public:
        static void Initialize();
        static void Shutdown();

        static void BeginBatch();
        static void EndBatch();
        static void Flush();

        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec4& color = glm::vec4(1.0f));
        static void DrawIsometricTile(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec4& color = glm::vec4(1.0f));
        static void DrawSpriteClean(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec4& color = glm::vec4(1.0f));
        static void DrawString(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color = glm::vec4(1.0f));

        struct Statistics {
            uint32_t drawCalls = 0;
            uint32_t quadCount = 0;
        };
        static Statistics GetStats();

    private:
        static void StartBatch();
        static void DrawQuadEx(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec4& color, float flags);
    };

}
