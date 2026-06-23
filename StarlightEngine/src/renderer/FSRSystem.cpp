#include "FSRSystem.hpp"
#include "Log.hpp"
#include "Shader.hpp"
#include <glad/glad.h>
#include <cmath>

namespace starlight {

namespace {

const char* fsrVertShader = R"(#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
out vec2 TexCoord;
void main() {
    TexCoord = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}
)";

const char* fsrEasuFragShader = R"(#version 450 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D uInputTex;
uniform vec2 uInputSize;
uniform vec2 uOutputSize;

// EASU edge-adaptive upscaling
void main() {
    vec2 texel = 1.0 / uInputSize;
    vec2 pos = TexCoord * uInputSize - 0.5;
    vec2 p = floor(pos);
    vec2 f = fract(pos);

    // Load a 4x4 neighborhood
    vec3 c05_05_neg = texture(uInputTex, (p + vec2(-0.5, -0.5)) * texel).rgb;
    vec3 c05_neg = texture(uInputTex, (p + vec2( 0.5, -0.5)) * texel).rgb;
    vec3 c15_neg = texture(uInputTex, (p + vec2( 1.5, -0.5)) * texel).rgb;
    vec3 c25_neg = texture(uInputTex, (p + vec2( 2.5, -0.5)) * texel).rgb;

    vec3 c05_neg_y = texture(uInputTex, (p + vec2(-0.5,  0.5)) * texel).rgb;
    vec3 c00 = texture(uInputTex, (p + vec2( 0.5,  0.5)) * texel).rgb;
    vec3 c10 = texture(uInputTex, (p + vec2( 1.5,  0.5)) * texel).rgb;
    vec3 c20 = texture(uInputTex, (p + vec2( 2.5,  0.5)) * texel).rgb;

    vec3 c05_pos_y = texture(uInputTex, (p + vec2(-0.5,  1.5)) * texel).rgb;
    vec3 c01 = texture(uInputTex, (p + vec2( 0.5,  1.5)) * texel).rgb;
    vec3 c11 = texture(uInputTex, (p + vec2( 1.5,  1.5)) * texel).rgb;
    vec3 c21 = texture(uInputTex, (p + vec2( 2.5,  1.5)) * texel).rgb;

    vec3 c05_25_pos = texture(uInputTex, (p + vec2(-0.5,  2.5)) * texel).rgb;
    vec3 c02 = texture(uInputTex, (p + vec2( 0.5,  2.5)) * texel).rgb;
    vec3 c12 = texture(uInputTex, (p + vec2( 1.5,  2.5)) * texel).rgb;
    vec3 c22 = texture(uInputTex, (p + vec2( 2.5,  2.5)) * texel).rgb;

    // Local edge analysis
    float g0 = length(c00 - c01);
    float g1 = length(c10 - c11);
    float g2 = length(c00 - c10);
    float g3 = length(c01 - c11);

    // Direction and weights
    float dirX = (g0 + g1) - (g2 + g3);
    float dirY = (g0 + g1) + (g2 + g3);

    // Filter weights based on edge gradients
    float wX = 1.0 / (1.0 + abs(dirX) * 4.0);
    float wY = 1.0 / (1.0 + abs(dirY) * 4.0);

    vec3 colX = mix(mix(c00, c10, f.x), mix(c01, c11, f.x), f.y);
    vec3 colY = mix(mix(c05_neg_y, c20, f.x), mix(c05_pos_y, c21, f.x), f.y);
    
    vec3 finalCol = mix(colX, colY, clamp(wY / (wX + wY + 0.0001), 0.0, 1.0));
    FragColor = vec4(finalCol, 1.0);
}
)";

const char* fsrRcasFragShader = R"(#version 450 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D uUpscaledTex;
uniform vec2 uSize;
uniform float uSharpness;

void main() {
    vec2 texel = 1.0 / uSize;
    vec4 c = texture(uUpscaledTex, TexCoord);
    vec4 n = texture(uUpscaledTex, TexCoord + vec2(0.0, -1.0) * texel);
    vec4 w = texture(uUpscaledTex, TexCoord + vec2(-1.0, 0.0) * texel);
    vec4 e = texture(uUpscaledTex, TexCoord + vec2(1.0, 0.0) * texel);
    vec4 s = texture(uUpscaledTex, TexCoord + vec2(0.0, 1.0) * texel);

    vec3 minCol = min(c.rgb, min(min(n.rgb, w.rgb), min(e.rgb, s.rgb)));
    vec3 maxCol = max(c.rgb, max(max(n.rgb, w.rgb), max(e.rgb, s.rgb)));

    vec3 minLuma = minCol;
    vec3 maxLuma = maxCol;
    vec3 diff = maxLuma - minLuma;
    vec3 strength = clamp(min(minLuma, 1.0 - maxLuma) / (diff + 0.0001), 0.0, 1.0);

    vec3 sharpen = strength * uSharpness * ((n.rgb + w.rgb + e.rgb + s.rgb) - 4.0 * c.rgb);
    FragColor = vec4(clamp(c.rgb + sharpen, 0.0, 1.0), c.a);
}
)";

} // anonymous namespace

FSRSystem::FSRSystem() : m_initialized(false) {}

FSRSystem::~FSRSystem() {
    Shutdown();
}

bool FSRSystem::Initialize(const FfxFsrContextDescription& desc) {
    if (m_initialized) Shutdown();

    m_desc = desc;

    if (glGenVertexArrays == nullptr) {
        m_initialized = true;
        Log::Warn("OpenGL is not initialized. FSRSystem initialized in MOCK mode.");
        return true;
    }

    // Compile EASU and RCAS shaders
    m_easuShader = std::make_shared<Shader>(fsrVertShader, fsrEasuFragShader);
    m_rcasShader = std::make_shared<Shader>(fsrVertShader, fsrRcasFragShader);

    // Create full screen quad mesh
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
    };
    unsigned int quadIndices[] = {
        0, 1, 2,
        1, 3, 2
    };
    
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glGenBuffers(1, &m_quadEBO);
    
    glBindVertexArray(m_quadVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    
    // TexCoords attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    glBindVertexArray(0);

    // Generate intermediate FBO and texture
    glGenFramebuffers(1, &m_intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFBO);

    glGenTextures(1, &m_intermediateTex);
    glBindTexture(GL_TEXTURE_2D, m_intermediateTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, desc.displaySizeWidth, desc.displaySizeHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_intermediateTex, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_initialized = true;
    
    Log::Info("AMD FSR 1.0 (EASU & RCAS) context created successfully. Render resolution: {}x{}, Display resolution: {}x{}",
        desc.maxRenderSizeWidth, desc.maxRenderSizeHeight,
        desc.displaySizeWidth, desc.displaySizeHeight);
    
    return true;
}

void FSRSystem::Shutdown() {
    if (m_initialized) {
        m_easuShader.reset();
        m_rcasShader.reset();
        
        if (glGenVertexArrays != nullptr) {
            if (m_intermediateFBO != 0) {
                glDeleteFramebuffers(1, &m_intermediateFBO);
                m_intermediateFBO = 0;
            }
            if (m_intermediateTex != 0) {
                glDeleteTextures(1, &m_intermediateTex);
                m_intermediateTex = 0;
            }
            if (m_quadVAO != 0) {
                glDeleteVertexArrays(1, &m_quadVAO);
                m_quadVAO = 0;
            }
            if (m_quadVBO != 0) {
                glDeleteBuffers(1, &m_quadVBO);
                m_quadVBO = 0;
            }
            if (m_quadEBO != 0) {
                glDeleteBuffers(1, &m_quadEBO);
                m_quadEBO = 0;
            }
        }
        
        Log::Info("AMD FSR 1.0 context destroyed.");
        m_initialized = false;
    }
}

void FSRSystem::Dispatch(uint32_t inputTexture, [[maybe_unused]] uint32_t outputTexture, [[maybe_unused]] float dt, float sharpness) {
    if (!m_initialized) return;
    Dispatch(inputTexture, 0, m_desc.maxRenderSizeWidth, m_desc.maxRenderSizeHeight, m_desc.displaySizeWidth, m_desc.displaySizeHeight, sharpness);
}

void FSRSystem::Dispatch(uint32_t inputTexture, uint32_t outputFBO, int inputW, int inputH, int outputW, int outputH, float sharpness) {
    if (!m_initialized || glGenVertexArrays == nullptr) return;

    // Recreate intermediate texture if display size changed
    if (outputW != (int)m_desc.displaySizeWidth || outputH != (int)m_desc.displaySizeHeight) {
        m_desc.displaySizeWidth = outputW;
        m_desc.displaySizeHeight = outputH;
        
        glBindTexture(GL_TEXTURE_2D, m_intermediateTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, outputW, outputH, 0, GL_RGBA, GL_FLOAT, nullptr);
    }

    // Save current FBO and viewport states
    GLint currentFBO;
    GLint currentViewport[4];
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO);
    glGetIntegerv(GL_VIEWPORT, currentViewport);

    // Pass 1: EASU Upscaling (low-res input -> display-res intermediate)
    glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFBO);
    glViewport(0, 0, outputW, outputH);
    glClear(GL_COLOR_BUFFER_BIT);
    
    m_easuShader->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    m_easuShader->SetIntU("uInputTex", 0);
    m_easuShader->SetVec2U("uInputSize", glm::vec2((float)inputW, (float)inputH));
    m_easuShader->SetVec2U("uOutputSize", glm::vec2((float)outputW, (float)outputH));

    glBindVertexArray(m_quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Pass 2: RCAS Sharpening (display-res intermediate -> final output FBO)
    glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
    glViewport(0, 0, outputW, outputH);

    m_rcasShader->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_intermediateTex);
    m_rcasShader->SetIntU("uUpscaledTex", 0);
    m_rcasShader->SetVec2U("uSize", glm::vec2((float)outputW, (float)outputH));
    m_rcasShader->SetFloatU("uSharpness", sharpness);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Restore previous states
    glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
    glViewport(currentViewport[0], currentViewport[1], currentViewport[2], currentViewport[3]);
}

} // namespace starlight
