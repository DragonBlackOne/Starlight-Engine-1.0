// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#include "Renderer2D.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include "Engine.hpp"
#include "Log.hpp"

namespace starlight {

    struct Renderer2DData {
        static const uint32_t MaxQuads = 10000;
        static const uint32_t MaxVertices = MaxQuads * 4;
        static const uint32_t MaxIndices = MaxQuads * 6;

        uint32_t quadVAO = 0;
        uint32_t quadVBO = 0;
        uint32_t quadIBO = 0;
        uint32_t whiteTexture = 0;
        uint32_t fontTexture = 0;

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

        // Generate slightly better 8x8 font (Procedural Bitmask)
        glGenTextures(1, &s_Data.fontTexture);
        glBindTexture(GL_TEXTURE_2D, s_Data.fontTexture);
        uint8_t fontData[128 * 128]; 
        memset(fontData, 0, sizeof(fontData));
        
        // Simple 5x7 font bitmasks for A-Z, 0-9
        auto drawChar = [&](char c, uint64_t mask) {
            int cx = (c % 16) * 8;
            int cy = (c / 16) * 8;
            for(int y=0; y<7; y++) {
                for(int x=0; x<5; x++) {
                    if((mask >> (y*5 + x)) & 1) fontData[(cy+y)*128 + (cx+x)] = 255;
                }
            }
        };

        // Minimalist Font Data (approximate 5x7)
        drawChar('A', 0x111F110EULL); drawChar('B', 0x1E111E111EULL); drawChar('C', 0x0E1110110EULL);
        drawChar('D', 0x1E1111111EULL); drawChar('E', 0x1F101E101FULL); drawChar('F', 0x10101E101FULL);
        drawChar('G', 0x0F1117100EULL); drawChar('H', 0x11111F1111ULL); drawChar('I', 0x0404040404ULL);
        drawChar('J', 0x0E11010101ULL); drawChar('K', 0x11121C1211ULL); drawChar('L', 0x1F10101010ULL);
        drawChar('M', 0x1111151B11ULL); drawChar('N', 0x1113151911ULL); drawChar('O', 0x0E1111110EULL);
        drawChar('P', 0x10101E111EULL); drawChar('Q', 0x0D1311110EULL); drawChar('R', 0x11121E111EULL);
        drawChar('S', 0x1E010E100FULL); drawChar('T', 0x040404041FULL); drawChar('U', 0x0E11111111ULL);
        drawChar('V', 0x040A111111ULL); drawChar('W', 0x111B151111ULL); drawChar('X', 0x110A040A11ULL);
        drawChar('Y', 0x0404040A11ULL); drawChar('Z', 0x1F0804021FULL);
        drawChar('0', 0x0E1111110EULL); drawChar('1', 0x040404040EULL); drawChar('2', 0x1F100E011EULL);
        drawChar('3', 0x1E010E011EULL); drawChar('4', 0x01011F1111ULL); drawChar('5', 0x1E011E101FULL);
        drawChar('6', 0x1E111E101EULL); drawChar('7', 0x020408101FULL); drawChar('8', 0x0E110E110EULL);
        drawChar('9', 0x1E011F110EULL);
        drawChar('!', 0x0400040404ULL); drawChar(':', 0x0400040000ULL); drawChar('-', 0x00001F0000ULL);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 128, 128, 0, GL_RED, GL_UNSIGNED_BYTE, fontData);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        s_Data.shader = Shader::LoadFromFile("assets/shaders/batch.vert", "assets/shaders/batch.frag");

        s_Data.textureSlots[0] = s_Data.whiteTexture;
        s_Data.textureSlots[1] = s_Data.fontTexture;
        s_Data.textureSlotIndex = 2;
        for (size_t i = 2; i < s_Data.MaxTextureSlots; i++) s_Data.textureSlots[i] = 0;
    }

    void Renderer2D::Shutdown() {
        delete[] s_Data.vertexBufferBase;
        s_Data.vertexBufferBase = nullptr;
        s_Data.vertexBufferPtr = nullptr;
        if (s_Data.quadVAO) { glDeleteVertexArrays(1, &s_Data.quadVAO); s_Data.quadVAO = 0; }
        if (s_Data.quadVBO) { glDeleteBuffers(1, &s_Data.quadVBO); s_Data.quadVBO = 0; }
        if (s_Data.quadIBO) { glDeleteBuffers(1, &s_Data.quadIBO); s_Data.quadIBO = 0; }
        if (s_Data.whiteTexture) { glDeleteTextures(1, &s_Data.whiteTexture); s_Data.whiteTexture = 0; }
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
        s_Data.textureSlotIndex = 2; // Slots 0 and 1 are reserved for White and Font
    }

    void Renderer2D::Flush() {
        static int flushCount = 0;
        if (flushCount++ < 10) {
            Log::Info("Renderer2D::Flush: indexCount = {}, quadVAO = {}, shaderValid = {}", s_Data.indexCount, s_Data.quadVAO, s_Data.shader ? 1 : 0);
        }
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
        // glDisable(GL_CULL_FACE); // Keep disabled for industrial stability
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
            // Check if it's already in the slots beyond the reserved ones
            bool found = false;
            for (uint32_t i = 2; i < s_Data.textureSlotIndex; i++) {
                if (s_Data.textureSlots[i] == textureID) {
                    textureIndex = (float)i;
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                textureIndex = (float)s_Data.textureSlotIndex;
                s_Data.textureSlots[s_Data.textureSlotIndex] = textureID;
                s_Data.textureSlotIndex++;
            }
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

    void Renderer2D::DrawString(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color) {
        float x = position.x;
        float y = position.y;
        float charWidth = 8.0f * scale;
        float charHeight = 8.0f * scale;

        for (char c : text) {
            if (c == '\n') {
                x = position.x;
                y += charHeight;
                continue;
            }
            if (c < 32 || c > 127) continue;

            float u = (float)(c % 16) / 16.0f;
            float v = (float)(c / 16) / 16.0f;
            float uvSize = 1.0f / 16.0f;

            // Manual DrawQuadEx behavior for custom UVs
            if (s_Data.indexCount >= s_Data.MaxIndices || s_Data.textureSlotIndex >= s_Data.MaxTextureSlots) {
                EndBatch();
                StartBatch();
            }

            float textureIndex = 1.0f; // Font is always slot 1
            float flags = 0.0f;

            s_Data.vertexBufferPtr->position = {x, y};
            s_Data.vertexBufferPtr->texCoord = {u, v};
            s_Data.vertexBufferPtr->color = color;
            s_Data.vertexBufferPtr->textureIndex = textureIndex;
            s_Data.vertexBufferPtr->flags = flags;
            s_Data.vertexBufferPtr++;

            s_Data.vertexBufferPtr->position = {x + charWidth, y};
            s_Data.vertexBufferPtr->texCoord = {u + uvSize, v};
            s_Data.vertexBufferPtr->color = color;
            s_Data.vertexBufferPtr->textureIndex = textureIndex;
            s_Data.vertexBufferPtr->flags = flags;
            s_Data.vertexBufferPtr++;

            s_Data.vertexBufferPtr->position = {x + charWidth, y + charHeight};
            s_Data.vertexBufferPtr->texCoord = {u + uvSize, v + uvSize};
            s_Data.vertexBufferPtr->color = color;
            s_Data.vertexBufferPtr->textureIndex = textureIndex;
            s_Data.vertexBufferPtr->flags = flags;
            s_Data.vertexBufferPtr++;

            s_Data.vertexBufferPtr->position = {x, y + charHeight};
            s_Data.vertexBufferPtr->texCoord = {u, v + uvSize};
            s_Data.vertexBufferPtr->color = color;
            s_Data.vertexBufferPtr->textureIndex = textureIndex;
            s_Data.vertexBufferPtr->flags = flags;
            s_Data.vertexBufferPtr++;

            s_Data.indexCount += 6;
            s_Data.stats.quadCount++;

            x += charWidth;
        }
    }

}
