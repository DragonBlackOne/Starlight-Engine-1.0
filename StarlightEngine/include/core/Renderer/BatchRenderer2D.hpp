#pragma once
#include "RendererCommon.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <array>
#include <functional>

namespace starlight::renderer {

struct QuadVertex {
    glm::vec3 position{ 0.0f };
    glm::vec4 color{ 1.0f };
    glm::vec2 texCoord{ 0.0f };
    float texIndex = 0.0f;
    float tilingFactor = 1.0f;
};

struct Batch2DConfig {
    uint32_t maxQuads = 20000;
    uint32_t maxTextureSlots = 16;
};

class BatchRenderer2D {
public:
    explicit BatchRenderer2D(const Batch2DConfig& config = Batch2DConfig{})
        : m_config(config),
          m_maxVertices(config.maxQuads * 4),
          m_maxIndices(config.maxQuads * 6) {
        m_quadVertexBuffer.resize(m_maxVertices);
        m_textureSlots.fill(0);
    }

    void Begin(const glm::mat4& viewProjection) {
        m_viewProjection = viewProjection;
        m_quadIndexCount = 0;
        m_quadVertexCount = 0;
        m_textureSlotIndex = 1; // Slot 0 is reserved for 1x1 white texture
        m_textureSlots[0] = 0; // 0 = white texture handle
    }

    void End() {
        Flush();
    }

    void Flush() {
        if (m_quadIndexCount == 0) return;

        m_stats.drawCalls++;
        m_stats.quadCount += m_quadIndexCount / 6;
        m_stats.vertexCount += m_quadVertexCount;
        m_stats.indexCount += m_quadIndexCount;

        // Reset batch
        m_quadIndexCount = 0;
        m_quadVertexCount = 0;
        m_textureSlotIndex = 1;
    }

    void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
        DrawQuad(glm::vec3(position.x, position.y, 0.0f), size, color);
    }

    void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));
        DrawQuad(transform, color);
    }

    void DrawQuad(const glm::mat4& transform, const glm::vec4& color) {
        constexpr size_t quadVertexCount = 4;
        constexpr glm::vec2 texCoords[4] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f }
        };

        if (m_quadIndexCount >= m_maxIndices) {
            Flush();
        }

        constexpr glm::vec4 quadVertexPositions[4] = {
            { -0.5f, -0.5f, 0.0f, 1.0f },
            {  0.5f, -0.5f, 0.0f, 1.0f },
            {  0.5f,  0.5f, 0.0f, 1.0f },
            { -0.5f,  0.5f, 0.0f, 1.0f }
        };

        for (size_t i = 0; i < quadVertexCount; ++i) {
            m_quadVertexBuffer[m_quadVertexCount + i].position = glm::vec3(transform * quadVertexPositions[i]);
            m_quadVertexBuffer[m_quadVertexCount + i].color = color;
            m_quadVertexBuffer[m_quadVertexCount + i].texCoord = texCoords[i];
            m_quadVertexBuffer[m_quadVertexCount + i].texIndex = 0.0f;
            m_quadVertexBuffer[m_quadVertexCount + i].tilingFactor = 1.0f;
        }

        m_quadVertexCount += 4;
        m_quadIndexCount += 6;
    }

    void DrawQuad(const glm::vec3& position, const glm::vec2& size, uint32_t textureHandle, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f)) {
        float textureIndex = 0.0f;

        for (uint32_t i = 1; i < m_textureSlotIndex; ++i) {
            if (m_textureSlots[i] == textureHandle) {
                textureIndex = static_cast<float>(i);
                break;
            }
        }

        if (textureIndex == 0.0f) {
            if (m_textureSlotIndex >= m_config.maxTextureSlots) {
                Flush();
            }
            textureIndex = static_cast<float>(m_textureSlotIndex);
            m_textureSlots[m_textureSlotIndex] = textureHandle;
            m_textureSlotIndex++;
        }

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));

        if (m_quadIndexCount >= m_maxIndices) {
            Flush();
        }

        constexpr glm::vec4 quadVertexPositions[4] = {
            { -0.5f, -0.5f, 0.0f, 1.0f },
            {  0.5f, -0.5f, 0.0f, 1.0f },
            {  0.5f,  0.5f, 0.0f, 1.0f },
            { -0.5f,  0.5f, 0.0f, 1.0f }
        };

        constexpr glm::vec2 texCoords[4] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f }
        };

        for (size_t i = 0; i < 4; ++i) {
            m_quadVertexBuffer[m_quadVertexCount + i].position = glm::vec3(transform * quadVertexPositions[i]);
            m_quadVertexBuffer[m_quadVertexCount + i].color = tintColor;
            m_quadVertexBuffer[m_quadVertexCount + i].texCoord = texCoords[i];
            m_quadVertexBuffer[m_quadVertexCount + i].texIndex = textureIndex;
            m_quadVertexBuffer[m_quadVertexCount + i].tilingFactor = tilingFactor;
        }

        m_quadVertexCount += 4;
        m_quadIndexCount += 6;
    }

    const RenderStats& GetStats() const { return m_stats; }
    void ResetStats() { m_stats.Reset(); }

    uint32_t GetQuadCount() const { return m_quadIndexCount / 6; }
    uint32_t GetVertexCount() const { return m_quadVertexCount; }
    uint32_t GetIndexCount() const { return m_quadIndexCount; }
    uint32_t GetTextureSlotCount() const { return m_textureSlotIndex; }

private:
    Batch2DConfig m_config;
    uint32_t m_maxVertices;
    uint32_t m_maxIndices;

    glm::mat4 m_viewProjection{ 1.0f };
    std::vector<QuadVertex> m_quadVertexBuffer;
    uint32_t m_quadVertexCount = 0;
    uint32_t m_quadIndexCount = 0;

    std::array<uint32_t, 32> m_textureSlots{};
    uint32_t m_textureSlotIndex = 1;

    RenderStats m_stats;
};

} // namespace starlight::renderer
