#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>
#include "Shader.hpp"

namespace starlight {

struct Vertex2D {
    glm::vec2 position;
    glm::vec2 texCoord;
    glm::vec4 color;
    float textureIndex;
    float flags;
};

struct QuadCommand {
    glm::vec2 positions[4];
    glm::vec2 texCoords[4];
    glm::vec4 color;
    float textureIndex;
    float flags;
    int layer;
    int orderInLayer;
    bool clipEnabled;
    glm::vec4 clipRect;
    uint32_t submissionID;
};

class Renderer2D {
public:
    static void Initialize();
    static void Shutdown();

    static void BeginBatch();
    static void EndBatch();
    static void Flush();

    // Basic quad
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, int layer = 0);
    static void DrawQuad(const glm::vec2& position,
        const glm::vec2& size,
        uint32_t textureID,
        const glm::vec4& color = glm::vec4(1.0f),
        int layer = 0);

    // Rotated quad with pivot
    static void DrawQuad(
        const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color, int layer = 0);
    static void DrawQuad(const glm::vec2& position,
        const glm::vec2& size,
        float rotation,
        uint32_t textureID,
        const glm::vec4& color = glm::vec4(1.0f),
        int layer = 0);
    static void DrawQuad(const glm::vec2& position,
        const glm::vec2& size,
        float rotation,
        const glm::vec2& pivot,
        uint32_t textureID,
        const glm::vec4& color = glm::vec4(1.0f),
        int layer = 0,
        bool flipX = false,
        bool flipY = false);

    // Quad with custom UV (for texture atlas)
    static void DrawQuad(const glm::vec2& position,
        const glm::vec2& size,
        uint32_t textureID,
        const glm::vec2& uv0,
        const glm::vec2& uv1,
        const glm::vec2& uv2,
        const glm::vec2& uv3,
        const glm::vec4& color = glm::vec4(1.0f),
        int layer = 0);

    // Triangle
    static void DrawTriangle(
        const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color, int layer = 0);

    // Rounded Rect
    static void DrawRoundedRect(
        const glm::vec2& position, const glm::vec2& size, float radius, const glm::vec4& color, int layer = 0);

    // Line
    static void DrawLine(
        const glm::vec2& p0, const glm::vec2& p1, float thickness, const glm::vec4& color, int layer = 0);

    // Circle
    static void DrawCircle(
        const glm::vec2& center, float radius, const glm::vec4& color, int layer = 0);

    // Clip / Scissor rect
    static void SetClipRect(const glm::vec4& clipRect);
    static void ClearClipRect();

    // Text
    static void DrawString(
        const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color = glm::vec4(1.0f), bool isSDF = false);

    struct Statistics {
        uint32_t drawCalls = 0;
        uint32_t quadCount = 0;
        uint32_t total2DObjects = 0;
        uint32_t visible2DObjects = 0;
    };
    static Statistics GetStats();
    static void ResetStats();
    static void Increment2DStats(uint32_t total, uint32_t visible);

    static uint32_t GetWhiteTexture();
    static uint32_t GetFontTexture();

private:
    struct Renderer2DData;
    static Renderer2DData s_Data;

    static void StartBatch();
    static void FlushInternal();
    static void AddQuad(const glm::vec2 positions[4],
        const glm::vec2 texCoords[4],
        uint32_t textureID,
        const glm::vec4& color,
        float flags,
        int layer,
        int orderInLayer);
    static float GetOrCacheTextureSlot(uint32_t textureID);
};

}  // namespace starlight
