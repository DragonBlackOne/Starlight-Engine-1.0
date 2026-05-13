// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#include "Renderer2D.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include "Engine.hpp"

namespace starlight {

    struct Renderer2DData {
        static const uint32_t MaxQuads = 10000;
        static const uint32_t MaxVertices = MaxQuads * 4;
        static const uint32_t MaxIndices = MaxQuads * 6;

        uint32_t quadVAO = 0;
        uint32_t quadVBO = 0;
        uint32_t quadIBO = 0;
        uint32_t whiteTexture = 0;

        uint32_t indexCount = 0;
        Vertex2D* vertexBufferBase = nullptr;
        Vertex2D* vertexBufferPtr = nullptr;

        std::shared_ptr<Shader> shader;
        Renderer2D::Statistics stats;

        static const uint32_t MaxTextureSlots = 16;
        std::array<uint32_t, MaxTextureSlots> textureSlots;
        uint32_t textureSlotIndex = 1; // 0 = white texture
    };

    static Renderer2DData s_Data;

    void Renderer2D::Initialize() {
        s_Data.vertexBufferBase = new Vertex2D[s_Data.MaxVertices];

        glGenVertexArrays(1, &s_Data.quadVAO);
        glBindVertexArray(s_Data.quadVAO);

        glGenBuffers(1, &s_Data.quadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, s_Data.quadVBO);
        glBufferData(GL_ARRAY_BUFFER, s_Data.MaxVertices * sizeof(Vertex2D), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)offsetof(Vertex2D, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)offsetof(Vertex2D, texCoord));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)offsetof(Vertex2D, color));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)offsetof(Vertex2D, textureIndex));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)offsetof(Vertex2D, flags));

        uint32_t* indices = new uint32_t[s_Data.MaxIndices];
        uint32_t offset = 0;
        for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6) {
            indices[i + 0] = offset + 0;
            indices[i + 1] = offset + 1;
            indices[i + 2] = offset + 2;
            indices[i + 3] = offset + 2;
            indices[i + 4] = offset + 3;
            indices[i + 5] = offset + 0;
            offset += 4;
        }

        glGenBuffers(1, &s_Data.quadIBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.quadIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, s_Data.MaxIndices * sizeof(uint32_t), indices, GL_STATIC_DRAW);
        delete[] indices;

        glGenTextures(1, &s_Data.whiteTexture);
        glBindTexture(GL_TEXTURE_2D, s_Data.whiteTexture);
        uint32_t white = 0xffffffff;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        s_Data.shader = Shader::LoadFromFile("assets/shaders/batch.vert", "assets/shaders/batch.frag");

        s_Data.textureSlots[0] = s_Data.whiteTexture;
        for (size_t i = 1; i < s_Data.MaxTextureSlots; i++) s_Data.textureSlots[i] = 0;
    }

    void Renderer2D::Shutdown() {
        delete[] s_Data.vertexBufferBase;
        glDeleteVertexArrays(1, &s_Data.quadVAO);
        glDeleteBuffers(1, &s_Data.quadVBO);
        glDeleteBuffers(1, &s_Data.quadIBO);
    }

    void Renderer2D::BeginBatch() {
        StartBatch();
    }

    void Renderer2D::EndBatch() {
        Flush();
    }

    void Renderer2D::StartBatch() {
        s_Data.indexCount = 0;
        s_Data.vertexBufferPtr = s_Data.vertexBufferBase;
        s_Data.textureSlotIndex = 1;
    }

    void Renderer2D::Flush() {
        if (s_Data.indexCount == 0) return;

        uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.vertexBufferPtr - (uint8_t*)s_Data.vertexBufferBase);

        // Ensure we draw to screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        float width = (float)Engine::Get().GetWindow().GetWidth();
        float height = (float)Engine::Get().GetWindow().GetHeight();
        glViewport(0, 0, (GLsizei)width, (GLsizei)height);
        glm::mat4 ortho = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);

        // Set GL state for 2D overlay
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        s_Data.shader->Use();
        s_Data.shader->SetMat4U("projection", ortho);

        for (uint32_t i = 0; i < s_Data.textureSlotIndex; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, s_Data.textureSlots[i]);
        }
        
        // We'll need to set the texture sampler array in the shader
        int samplers[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
        s_Data.shader->SetIntArray("uTextures", samplers, 16);

        // Upload vertex data and draw
        glBindVertexArray(s_Data.quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, s_Data.quadVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, s_Data.vertexBufferBase);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.quadIBO);
        glDrawElements(GL_TRIANGLES, s_Data.indexCount, GL_UNSIGNED_INT, nullptr);

        // Restore state
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        s_Data.stats.drawCalls++;
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
        DrawQuad(position, size, s_Data.whiteTexture, color);
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec4& color) {
        DrawQuadEx(position, size, textureID, color, 0.0f);
    }

    void Renderer2D::DrawIsometricTile(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec4& color) {
        DrawQuadEx(position, size, textureID, color, 1.0f);
    }

    void Renderer2D::DrawSpriteClean(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec4& color) {
        DrawQuadEx(position, size, textureID, color, 2.0f);
    }

    void Renderer2D::DrawQuadEx(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec4& color, float flags) {
        if (s_Data.indexCount >= s_Data.MaxIndices || s_Data.textureSlotIndex >= s_Data.MaxTextureSlots) {
            EndBatch();
            StartBatch();
        }

        float textureIndex = 0.0f;
        for (uint32_t i = 0; i < s_Data.textureSlotIndex; i++) {
            if (s_Data.textureSlots[i] == textureID) {
                textureIndex = (float)i;
                break;
            }
        }

        if (textureIndex == 0.0f && textureID != s_Data.whiteTexture) {
            textureIndex = (float)s_Data.textureSlotIndex;
            s_Data.textureSlots[s_Data.textureSlotIndex] = textureID;
            s_Data.textureSlotIndex++;
        }

        s_Data.vertexBufferPtr->position = position;
        s_Data.vertexBufferPtr->texCoord = {0, 0};
        s_Data.vertexBufferPtr->color = color;
        s_Data.vertexBufferPtr->textureIndex = textureIndex;
        s_Data.vertexBufferPtr->flags = flags;
        s_Data.vertexBufferPtr++;

        s_Data.vertexBufferPtr->position = {position.x + size.x, position.y};
        s_Data.vertexBufferPtr->texCoord = {1, 0};
        s_Data.vertexBufferPtr->color = color;
        s_Data.vertexBufferPtr->textureIndex = textureIndex;
        s_Data.vertexBufferPtr->flags = flags;
        s_Data.vertexBufferPtr++;

        s_Data.vertexBufferPtr->position = {position.x + size.x, position.y + size.y};
        s_Data.vertexBufferPtr->texCoord = {1, 1};
        s_Data.vertexBufferPtr->color = color;
        s_Data.vertexBufferPtr->textureIndex = textureIndex;
        s_Data.vertexBufferPtr->flags = flags;
        s_Data.vertexBufferPtr++;

        s_Data.vertexBufferPtr->position = {position.x, position.y + size.y};
        s_Data.vertexBufferPtr->texCoord = {0, 1};
        s_Data.vertexBufferPtr->color = color;
        s_Data.vertexBufferPtr->textureIndex = textureIndex;
        s_Data.vertexBufferPtr->flags = flags;
        s_Data.vertexBufferPtr++;

        s_Data.indexCount += 6;
        s_Data.stats.quadCount++;
    }

    Renderer2D::Statistics Renderer2D::GetStats() { return s_Data.stats; }

}
