#include "TAAPass.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "Log.hpp"
#include "CVarSystem.hpp"
#include "Engine.hpp"
#include <glad/glad.h>

namespace starlight {

    // Halton 2,3 sequence of 8 jitter offsets (scaled to pixels)
    static const glm::vec2 Halton23[8] = {
        {  0.0f/8.0f,   0.0f/9.0f },
        { -0.5f/8.0f,  -0.3333f/9.0f },
        {  0.5f/8.0f,   0.3333f/9.0f },
        { -0.75f/8.0f, -0.1111f/9.0f },
        {  0.25f/8.0f,  0.5555f/9.0f },
        { -0.25f/8.0f, -0.7777f/9.0f },
        {  0.75f/8.0f,  0.1111f/9.0f },
        { -0.875f/8.0f, 0.7777f/9.0f }
    };

    TAAPass::TAAPass() {
        m_taaShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/taa.frag");
        Log::Info("TAAPass created.");
    }

    TAAPass::~TAAPass() {
        DestroyBuffers();
    }

    void TAAPass::InitBuffers(uint32_t width, uint32_t height) {
        DestroyBuffers();

        m_width = width;
        m_height = height;

        glGenFramebuffers(2, m_historyFBOs);
        glGenTextures(2, m_historyTextures);

        for (int i = 0; i < 2; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_historyFBOs[i]);
            glBindTexture(GL_TEXTURE_2D, m_historyTextures[i]);
            
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_historyTextures[i], 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                Log::Error("TAAPass: History Framebuffer {} is incomplete!", i);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_firstFrame = true;
    }

    void TAAPass::DestroyBuffers() {
        if (m_historyFBOs[0]) {
            glDeleteFramebuffers(2, m_historyFBOs);
            m_historyFBOs[0] = m_historyFBOs[1] = 0;
        }
        if (m_historyTextures[0]) {
            glDeleteTextures(2, m_historyTextures);
            m_historyTextures[0] = m_historyTextures[1] = 0;
        }
    }

    void TAAPass::OnResize(uint32_t width, uint32_t height) {
        if (width != m_width || height != m_height) {
            InitBuffers(width, height);
        }
    }

    glm::vec2 TAAPass::GetJitterOffset() const {
        auto cvarSys = Engine::Get().GetSystem<CVarSystem>();
        bool taaEnabled = cvarSys ? cvarSys->GetBool("r_taa") : true;
        if (!taaEnabled) return glm::vec2(0.0f);

        return Halton23[m_frameCount % 8];
    }

    void TAAPass::ApplyJitter(glm::mat4& projection, uint32_t width, uint32_t height) {
        glm::vec2 jitter = GetJitterOffset();
        projection[2][0] += jitter.x * (2.0f / (float)width);
        projection[2][1] += jitter.y * (2.0f / (float)height);
    }

    void TAAPass::Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) {
        (void)resources;
        auto rendererPtr = blackboard.Get<Renderer*>("Renderer");
        if (!rendererPtr) return;
        Renderer* r = *rendererPtr;

        auto cvarSys = Engine::Get().GetSystem<CVarSystem>();
        bool taaEnabled = cvarSys ? cvarSys->GetBool("r_taa") : true;

        OnResize(r->m_fboWidth, r->m_fboHeight);

        glm::mat4 currentViewProj = r->m_projectionMatrix * r->m_view;

        if (!taaEnabled) {
            m_prevViewProj = currentViewProj;
            m_frameCount++;
            return;
        }

        // Target FBO for this frame is the opposite of the source history
        uint32_t targetIndex = 1 - m_historySourceIndex;

        glBindFramebuffer(GL_FRAMEBUFFER, m_historyFBOs[targetIndex]);
        glViewport(0, 0, r->m_fboWidth, r->m_fboHeight);

        m_taaShader->Use();

        // Bind Current Color (from r->m_fboTexture.Get())
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, r->m_fboTexture.Get());
        m_taaShader->SetIntU("currentTexture", 0);

        // Bind Previous Color History
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_historyTextures[m_historySourceIndex]);
        m_taaShader->SetIntU("historyTexture", 1);

        // Bind G-Buffer Position (for world space reprojection)
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, r->m_gPosition.Get());
        m_taaShader->SetIntU("gPosition", 2);

        // Bind G-Buffer Normal (to skip clamping on background/skybox pixels)
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, r->m_gNormal.Get());
        m_taaShader->SetIntU("gNormal", 3);

        m_taaShader->SetMat4U("prevViewProj", m_prevViewProj);
        m_taaShader->SetVec2U("texelSize", glm::vec2(1.0f / (float)r->m_fboWidth, 1.0f / (float)r->m_fboHeight));
        m_taaShader->SetIntU("firstFrame", m_firstFrame ? 1 : 0);

        r->m_quadMesh->Draw();

        // Copy history output back into the main scene color texture so it feeds the rest of the post-processing
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_historyFBOs[targetIndex]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, r->m_fbo.Get());
        glBlitFramebuffer(0, 0, r->m_fboWidth, r->m_fboHeight, 0, 0, r->m_fboWidth, r->m_fboHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Swap ping-pong history index
        m_historySourceIndex = targetIndex;
        m_prevViewProj = currentViewProj;
        m_firstFrame = false;
        m_frameCount++;
    }

}
