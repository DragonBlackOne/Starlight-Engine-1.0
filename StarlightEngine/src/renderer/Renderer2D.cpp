#include "Renderer2D.hpp"
#include <algorithm>
#include <array>
#include <cstring>
#include <glad/glad.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Engine.hpp"
#include "Log.hpp"

namespace starlight {

struct Renderer2D::Renderer2DData {
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

    std::vector<QuadCommand> quadBuffer;
    QuadCommand* quadWritePtr = nullptr;
    uint32_t quadCount = 0;

    std::shared_ptr<Shader> shader;
    Renderer2D::Statistics stats;

    static const uint32_t MaxTextureSlots = 16;
    std::array<uint32_t, MaxTextureSlots> textureSlots;
    uint32_t textureSlotIndex = 1;

    bool clipEnabled = false;
    glm::vec4 clipRect = {0.0f, 0.0f, 0.0f, 0.0f};
};

Renderer2D::Renderer2DData Renderer2D::s_Data;

void Renderer2D::Initialize() {
    s_Data.vertexBufferBase = new Vertex2D[s_Data.MaxVertices];
    s_Data.quadBuffer.resize(s_Data.MaxQuads);

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

    glGenTextures(1, &s_Data.fontTexture);
    glBindTexture(GL_TEXTURE_2D, s_Data.fontTexture);
    uint32_t fontData[128 * 128];
    memset(fontData, 0, sizeof(fontData));

    auto drawChar = [&](char c, uint64_t mask) {
        int cx = (c % 16) * 8;
        int cy = (c / 16) * 8;
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                if ((mask >> (y * 8 + x)) & 1) {
                    fontData[(cy + y) * 128 + (cx + x)] = 0xffffffff;
                }
            }
        }
    };

    drawChar('A', 0x004242427E422418ULL);
    drawChar('B', 0x003C22223C22223CULL);
    drawChar('C', 0x001C22404040221CULL);
    drawChar('D', 0x0038242222222438ULL);
    drawChar('E', 0x007E40407C40407EULL);
    drawChar('F', 0x004040407C40407EULL);
    drawChar('G', 0x003C42424E40423CULL);
    drawChar('H', 0x004242427E424242ULL);
    drawChar('I', 0x003E08080808083EULL);
    drawChar('J', 0x001C22020202021FULL);
    drawChar('K', 0x0044485060504844ULL);
    drawChar('L', 0x007E404040404040ULL);
    drawChar('M', 0x00424242425A6642ULL);
    drawChar('N', 0x004242464A526242ULL);
    drawChar('O', 0x003C42424242423CULL);
    drawChar('P', 0x004040407C42427CULL);
    drawChar('Q', 0x003A444A4242423CULL);
    drawChar('R', 0x004244487C42427CULL);
    drawChar('S', 0x003C42023C40423CULL);
    drawChar('T', 0x000808080808087EULL);
    drawChar('U', 0x003C424242424242ULL);
    drawChar('V', 0x0018242442424242ULL);
    drawChar('W', 0x0042665A42424242ULL);
    drawChar('X', 0x0042422418182442ULL);
    drawChar('Y', 0x0008080808182442ULL);
    drawChar('Z', 0x007E40201008047EULL);
    drawChar('0', 0x003C4A526242423CULL);
    drawChar('1', 0x003E080808281808ULL);
    drawChar('2', 0x007E40300C02423CULL);
    drawChar('3', 0x003C42021C02423CULL);
    drawChar('4', 0x0004047E24140C04ULL);
    drawChar('5', 0x003C4202027C407EULL);
    drawChar('6', 0x003C4242427C403CULL);
    drawChar('7', 0x001010100804027EULL);
    drawChar('8', 0x003C42423C42423CULL);
    drawChar('9', 0x003C02023E42423CULL);
    drawChar('!', 0x0008000808080808ULL);
    drawChar(':', 0x0000080000080000ULL);
    drawChar('-', 0x0000003E00000000ULL);
    drawChar('(', 0x0018204040402018ULL);
    drawChar(')', 0x0018040202020418ULL);
    drawChar('/', 0x0002020408102040ULL);
    drawChar('|', 0x0010101010101010ULL);
    drawChar('.', 0x0000000000000010ULL);
    drawChar(',', 0x0000000000001008ULL);
    drawChar('?', 0x0000380408100010ULL);
    drawChar('+', 0x001010107C101010ULL);
    drawChar('=', 0x0000007C007C0000ULL);
    drawChar('_', 0x000000000000007CULL);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, fontData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    s_Data.shader = Shader::LoadFromFile("assets/shaders/batch.vert", "assets/shaders/batch.frag");

    s_Data.textureSlots[0] = s_Data.whiteTexture;
    s_Data.textureSlots[1] = s_Data.fontTexture;
    s_Data.textureSlotIndex = 2;
    for (size_t i = 2; i < s_Data.MaxTextureSlots; i++)
        s_Data.textureSlots[i] = 0;
}

void Renderer2D::Shutdown() {
    delete[] s_Data.vertexBufferBase;
    s_Data.vertexBufferBase = nullptr;
    s_Data.vertexBufferPtr = nullptr;
    s_Data.quadBuffer.clear();
    if (s_Data.quadVAO) {
        glDeleteVertexArrays(1, &s_Data.quadVAO);
        s_Data.quadVAO = 0;
    }
    if (s_Data.quadVBO) {
        glDeleteBuffers(1, &s_Data.quadVBO);
        s_Data.quadVBO = 0;
    }
    if (s_Data.quadIBO) {
        glDeleteBuffers(1, &s_Data.quadIBO);
        s_Data.quadIBO = 0;
    }
    if (s_Data.whiteTexture) {
        glDeleteTextures(1, &s_Data.whiteTexture);
        s_Data.whiteTexture = 0;
    }
    if (s_Data.fontTexture) {
        glDeleteTextures(1, &s_Data.fontTexture);
        s_Data.fontTexture = 0;
    }
}

void Renderer2D::BeginBatch() {
    StartBatch();
}

void Renderer2D::EndBatch() {
    Flush();
}

void Renderer2D::StartBatch() {
    s_Data.quadCount = 0;
    s_Data.quadWritePtr = s_Data.quadBuffer.data();
    s_Data.textureSlotIndex = 2;
    s_Data.clipEnabled = false;
}

void Renderer2D::Flush() {
    if (s_Data.quadCount == 0)
        return;

    // Sort quads by layer -> orderInLayer -> submissionID to preserve exact draw order (Z-order)
    std::sort(s_Data.quadBuffer.data(),
        s_Data.quadBuffer.data() + s_Data.quadCount,
        [](const QuadCommand& a, const QuadCommand& b) {
            if (a.layer != b.layer)
                return a.layer < b.layer;
            if (a.orderInLayer != b.orderInLayer)
                return a.orderInLayer < b.orderInLayer;
            return a.submissionID < b.submissionID;
        });

    // Assemble sorted vertices
    s_Data.indexCount = 0;
    s_Data.vertexBufferPtr = s_Data.vertexBufferBase;

    for (uint32_t i = 0; i < s_Data.quadCount; i++) {
        auto& q = s_Data.quadBuffer[i];

        for (int v = 0; v < 4; v++) {
            s_Data.vertexBufferPtr->position = q.positions[v];
            s_Data.vertexBufferPtr->texCoord = q.texCoords[v];
            s_Data.vertexBufferPtr->color = q.color;
            s_Data.vertexBufferPtr->textureIndex = q.textureIndex;
            s_Data.vertexBufferPtr->flags = q.flags;
            s_Data.vertexBufferPtr++;
        }

        s_Data.indexCount += 6;
    }

    FlushInternal();
}

void Renderer2D::FlushInternal() {
    if (s_Data.indexCount == 0)
        return;

    uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.vertexBufferPtr - (uint8_t*)s_Data.vertexBufferBase);

    GLint currentFBO = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)currentFBO);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float width = (float)viewport[2];
    float height = (float)viewport[3];
    glm::mat4 ortho = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);

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

    int samplers[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    s_Data.shader->SetIntArray("uTextures", samplers, 16);

    glBindVertexArray(s_Data.quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.quadVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, s_Data.vertexBufferBase);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.quadIBO);

    // Group-based rendering by scissor state
    uint32_t groupStartQuad = 0;
    bool currentClipEnabled = s_Data.quadBuffer[0].clipEnabled;
    glm::vec4 currentClipRect = s_Data.quadBuffer[0].clipRect;

    for (uint32_t i = 1; i <= s_Data.quadCount; i++) {
        bool nextClipEnabled = (i < s_Data.quadCount) ? s_Data.quadBuffer[i].clipEnabled : !currentClipEnabled;
        glm::vec4 nextClipRect = (i < s_Data.quadCount) ? s_Data.quadBuffer[i].clipRect : glm::vec4(0.0f);

        bool clipStateChanged =
            (nextClipEnabled != currentClipEnabled) || (currentClipEnabled && (nextClipRect != currentClipRect));

        if (i == s_Data.quadCount || clipStateChanged) {
            uint32_t groupQuadCount = i - groupStartQuad;
            uint32_t groupIndexCount = groupQuadCount * 6;
            uint32_t groupStartIndex = groupStartQuad * 6;

            if (currentClipEnabled) {
                glEnable(GL_SCISSOR_TEST);
                GLint scissorX = (GLint)currentClipRect.x;
                GLint scissorY = (GLint)(height - (currentClipRect.y + currentClipRect.w));
                GLint scissorW = (GLint)currentClipRect.z;
                GLint scissorH = (GLint)currentClipRect.w;
                glScissor(scissorX, scissorY, scissorW, scissorH);
            } else {
                glDisable(GL_SCISSOR_TEST);
            }

            glDrawElements(
                GL_TRIANGLES, groupIndexCount, GL_UNSIGNED_INT, (const void*)(groupStartIndex * sizeof(uint32_t)));
            s_Data.stats.drawCalls++;

            if (i < s_Data.quadCount) {
                groupStartQuad = i;
                currentClipEnabled = nextClipEnabled;
                currentClipRect = nextClipRect;
            }
        }
    }

    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);

    s_Data.indexCount = 0;
}

void Renderer2D::AddQuad(const glm::vec2 positions[4],
    const glm::vec2 texCoords[4],
    uint32_t textureID,
    const glm::vec4& color,
    float flags,
    int layer,
    int orderInLayer) {
    if (s_Data.quadCount >= s_Data.MaxQuads) {
        EndBatch();
        StartBatch();
    }

    float textureIndex = GetOrCacheTextureSlot(textureID);

    auto& q = s_Data.quadBuffer[s_Data.quadCount];
    for (int i = 0; i < 4; i++) {
        q.positions[i] = positions[i];
        q.texCoords[i] = texCoords[i];
    }
    q.color = color;
    q.textureIndex = textureIndex;
    q.flags = flags;
    q.layer = layer;
    q.orderInLayer = orderInLayer;
    q.clipEnabled = s_Data.clipEnabled;
    q.clipRect = s_Data.clipRect;
    q.submissionID = s_Data.quadCount;

    s_Data.quadCount++;
    s_Data.stats.quadCount++;
}

float Renderer2D::GetOrCacheTextureSlot(uint32_t textureID) {
    if (textureID == s_Data.whiteTexture)
        return 0.0f;
    if (textureID == s_Data.fontTexture)
        return 1.0f;

    for (uint32_t i = 2; i < s_Data.textureSlotIndex; i++) {
        if (s_Data.textureSlots[i] == textureID) {
            return (float)i;
        }
    }

    if (s_Data.textureSlotIndex >= s_Data.MaxTextureSlots) {
        EndBatch();
        StartBatch();
    }

    float slot = (float)s_Data.textureSlotIndex;
    s_Data.textureSlots[s_Data.textureSlotIndex] = textureID;
    s_Data.textureSlotIndex++;
    return slot;
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, int layer) {
    DrawQuad(position, size, s_Data.whiteTexture, color, layer);
}

void Renderer2D::DrawQuad(
    const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec4& color, int layer) {
    glm::vec2 positions[4] = {{position.x, position.y},
        {position.x + size.x, position.y},
        {position.x + size.x, position.y + size.y},
        {position.x, position.y + size.y}};
    glm::vec2 texCoords[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    AddQuad(positions, texCoords, textureID, color, 0.0f, layer, 0);
}

void Renderer2D::DrawQuad(
    const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color, int layer) {
    DrawQuad(position, size, rotation, s_Data.whiteTexture, color, layer);
}

void Renderer2D::DrawQuad(const glm::vec2& position,
    const glm::vec2& size,
    float rotation,
    uint32_t textureID,
    const glm::vec4& color,
    int layer) {
    glm::vec2 pivot = {0.5f, 0.5f};
    glm::vec2 center = {position.x + size.x * pivot.x, position.y + size.y * pivot.y};

    float cosA = cosf(rotation);
    float sinA = sinf(rotation);

    glm::vec2 local[4] = {{-size.x * pivot.x, -size.y * pivot.y},
        {size.x * (1.0f - pivot.x), -size.y * pivot.y},
        {size.x * (1.0f - pivot.x), size.y * (1.0f - pivot.y)},
        {-size.x * pivot.x, size.y * (1.0f - pivot.y)}};

    glm::vec2 positions[4];
    for (int i = 0; i < 4; i++) {
        positions[i] = {
            center.x + local[i].x * cosA - local[i].y * sinA, center.y + local[i].x * sinA + local[i].y * cosA};
    }

    glm::vec2 texCoords[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    AddQuad(positions, texCoords, textureID, color, 0.0f, layer, 0);
}

void Renderer2D::DrawQuad(const glm::vec2& position,
    const glm::vec2& size,
    float rotation,
    const glm::vec2& pivot,
    uint32_t textureID,
    const glm::vec4& color,
    int layer,
    bool flipX,
    bool flipY) {
    glm::vec2 center = {position.x + size.x * pivot.x, position.y + size.y * pivot.y};

    float cosA = cosf(rotation);
    float sinA = sinf(rotation);

    glm::vec2 local[4] = {{-size.x * pivot.x, -size.y * pivot.y},
        {size.x * (1.0f - pivot.x), -size.y * pivot.y},
        {size.x * (1.0f - pivot.x), size.y * (1.0f - pivot.y)},
        {-size.x * pivot.x, size.y * (1.0f - pivot.y)}};

    glm::vec2 positions[4];
    for (int i = 0; i < 4; i++) {
        positions[i] = {
            center.x + local[i].x * cosA - local[i].y * sinA, center.y + local[i].x * sinA + local[i].y * cosA};
    }

    float uMin = flipX ? 1.0f : 0.0f;
    float uMax = flipX ? 0.0f : 1.0f;
    float vMin = flipY ? 1.0f : 0.0f;
    float vMax = flipY ? 0.0f : 1.0f;

    glm::vec2 texCoords[4] = {
        {uMin, vMin},
        {uMax, vMin},
        {uMax, vMax},
        {uMin, vMax}
    };

    AddQuad(positions, texCoords, textureID, color, 0.0f, layer, 0);
}

void Renderer2D::DrawQuad(const glm::vec2& position,
    const glm::vec2& size,
    uint32_t textureID,
    const glm::vec2& uv0,
    const glm::vec2& uv1,
    const glm::vec2& uv2,
    const glm::vec2& uv3,
    const glm::vec4& color,
    int layer) {
    glm::vec2 positions[4] = {{position.x, position.y},
        {position.x + size.x, position.y},
        {position.x + size.x, position.y + size.y},
        {position.x, position.y + size.y}};
    glm::vec2 texCoords[4] = {uv0, uv1, uv2, uv3};
    AddQuad(positions, texCoords, textureID, color, 0.0f, layer, 0);
}

uint32_t Renderer2D::GetWhiteTexture() {
    return s_Data.whiteTexture;
}
uint32_t Renderer2D::GetFontTexture() {
    return s_Data.fontTexture;
}

Renderer2D::Statistics Renderer2D::GetStats() {
    return s_Data.stats;
}

void Renderer2D::ResetStats() {
    s_Data.stats.drawCalls = 0;
    s_Data.stats.quadCount = 0;
    s_Data.stats.total2DObjects = 0;
    s_Data.stats.visible2DObjects = 0;
}

void Renderer2D::Increment2DStats(uint32_t total, uint32_t visible) {
    s_Data.stats.total2DObjects += total;
    s_Data.stats.visible2DObjects += visible;
}

void Renderer2D::DrawString(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color, bool isSDF) {
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
        if (c == ' ') {
            x += charWidth;
            continue;
        }
        
        // Convert lowercase characters to uppercase automatically to fit retro layout
        char renderChar = c;
        if (renderChar >= 'a' && renderChar <= 'z') {
            renderChar = renderChar - 'a' + 'A';
        }

        if (renderChar < 32 || renderChar > 127)
            continue;

        float u = (float)(renderChar % 16) / 16.0f;
        float v = (float)(renderChar / 16) / 16.0f;
        float uvSize = 1.0f / 16.0f;

        glm::vec2 positions[4] = {{x, y}, {x + charWidth, y}, {x + charWidth, y + charHeight}, {x, y + charHeight}};
        glm::vec2 texCoords[4] = {
            {u + uvSize, v},
            {u, v},
            {u, v + uvSize},
            {u + uvSize, v + uvSize}
        };

        float flags = isSDF ? 1.0f : 0.0f;
        AddQuad(positions, texCoords, s_Data.fontTexture, color, flags, 0, 0);

        x += charWidth;
    }
}

void Renderer2D::DrawTriangle(
    const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color, int layer) {
    glm::vec2 positions[4] = {p0, p1, p2, p2};
    glm::vec2 texCoords[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    AddQuad(positions, texCoords, s_Data.whiteTexture, color, 0.0f, layer, 0);
}

void Renderer2D::DrawRoundedRect(
    const glm::vec2& position, const glm::vec2& size, float radius, const glm::vec4& color, int layer) {
    float r = std::min({radius, size.x * 0.5f, size.y * 0.5f});
    if (r <= 0.0f) {
        DrawQuad(position, size, color, layer);
        return;
    }

    DrawQuad({position.x + r, position.y}, {size.x - 2.0f * r, size.y}, color, layer);
    DrawQuad({position.x, position.y + r}, {r, size.y - 2.0f * r}, color, layer);
    DrawQuad({position.x + size.x - r, position.y + r}, {r, size.y - 2.0f * r}, color, layer);

    auto drawCorner = [&](const glm::vec2& center, float startAngle, float endAngle) {
        const int segments = 8;
        float step = (endAngle - startAngle) / segments;
        for (int i = 0; i < segments; i++) {
            float a1 = startAngle + i * step;
            float a2 = startAngle + (i + 1) * step;
            glm::vec2 p1 = center + glm::vec2(cosf(a1), sinf(a1)) * r;
            glm::vec2 p2 = center + glm::vec2(cosf(a2), sinf(a2)) * r;
            DrawTriangle(center, p1, p2, color, layer);
        }
    };

    float pi = 3.14159265f;
    drawCorner({position.x + r, position.y + r}, pi, pi * 1.5f);
    drawCorner({position.x + size.x - r, position.y + r}, pi * 1.5f, pi * 2.0f);
    drawCorner({position.x + size.x - r, position.y + size.y - r}, 0.0f, pi * 0.5f);
    drawCorner({position.x + r, position.y + size.y - r}, pi * 0.5f, pi);
}

void Renderer2D::DrawLine(
    const glm::vec2& p0, const glm::vec2& p1, float thickness, const glm::vec4& color, int layer) {
    glm::vec2 dir = p1 - p0;
    float len = glm::length(dir);
    if (len < 0.0001f) return;

    glm::vec2 dirNorm = dir / len;
    glm::vec2 normal = glm::vec2(-dirNorm.y, dirNorm.x);
    glm::vec2 offset = normal * (thickness * 0.5f);

    glm::vec2 positions[4] = {
        p0 - offset,
        p0 + offset,
        p1 + offset,
        p1 - offset
    };
    glm::vec2 texCoords[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    AddQuad(positions, texCoords, s_Data.whiteTexture, color, 0.0f, layer, 0);
}

void Renderer2D::DrawCircle(
    const glm::vec2& center, float radius, const glm::vec4& color, int layer) {
    if (radius <= 0.0f) return;

    const int segments = 32;
    float step = (2.0f * 3.14159265f) / segments;
    for (int i = 0; i < segments; i++) {
        float a1 = i * step;
        float a2 = (i + 1) * step;
        glm::vec2 p1 = center + glm::vec2(cosf(a1), sinf(a1)) * radius;
        glm::vec2 p2 = center + glm::vec2(cosf(a2), sinf(a2)) * radius;
        DrawTriangle(center, p1, p2, color, layer);
    }
}

void Renderer2D::SetClipRect(const glm::vec4& clipRect) {
    if (s_Data.clipEnabled && s_Data.clipRect == clipRect)
        return;

    EndBatch();
    StartBatch();

    s_Data.clipEnabled = true;
    s_Data.clipRect = clipRect;
}

void Renderer2D::ClearClipRect() {
    if (!s_Data.clipEnabled)
        return;

    EndBatch();
    StartBatch();

    s_Data.clipEnabled = false;
    s_Data.clipRect = glm::vec4(0.0f);
}

}  // namespace starlight
