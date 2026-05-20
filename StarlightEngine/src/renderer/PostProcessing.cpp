#include "PostProcessing.hpp"
#include <glad/glad.h>
#include "Engine.hpp"
#include "Renderer.hpp"

namespace starlight {

    std::vector<PostProcessing::BloomMip> PostProcessing::s_bloomMips;
    uint32_t PostProcessing::s_bloomFBO = 0;
    std::shared_ptr<Shader> PostProcessing::s_downsampleShader;
    std::shared_ptr<Shader> PostProcessing::s_upsampleShader;
    std::shared_ptr<Shader> PostProcessing::s_compositionShader;
    std::shared_ptr<Mesh> PostProcessing::s_quadMesh;

    void PostProcessing::Initialize() {
        if (s_bloomFBO != 0) return;

        glGenFramebuffers(1, &s_bloomFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, s_bloomFBO);

        uint32_t width = Engine::Get().GetWindow().GetWidth();
        uint32_t height = Engine::Get().GetWindow().GetHeight();
        glm::vec2 mipSize((float)width, (float)height);
        glm::ivec2 mipIntSize(width, height);

        for (int i = 0; i < 6; i++) {
            mipSize *= 0.5f;
            mipIntSize /= 2;

            BloomMip mip;
            mip.size = mipIntSize;
            glGenTextures(1, &mip.texture);
            glBindTexture(GL_TEXTURE_2D, mip.texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, mipIntSize.x, mipIntSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            s_bloomMips.push_back(mip);
        }

        s_downsampleShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/bloom_downsample.frag");
        s_upsampleShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/bloom_upsample.frag");
        s_compositionShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/composition.frag");
        
        // Reuse quad mesh from renderer if possible, or create new
        std::vector<Vertex> vertices = {
            {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0,0,0,0}, {1,0,0,0}},
            {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0,0,0,0}, {1,0,0,0}},
            {{ 1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0,0,0,0}, {1,0,0,0}},
            {{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0,0,0,0}, {1,0,0,0}},
        };
        std::vector<uint32_t> indices = { 0, 1, 2, 1, 3, 2 };
        s_quadMesh = std::make_shared<Mesh>(vertices, indices);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PostProcessing::Shutdown() {
        for (auto& mip : s_bloomMips) {
            glDeleteTextures(1, &mip.texture);
        }
        s_bloomMips.clear();
        glDeleteFramebuffers(1, &s_bloomFBO);
    }

    void PostProcessing::RenderBloom(uint32_t inputTexture, uint32_t width, uint32_t height) {
        if (!s_downsampleShader || !s_upsampleShader || !s_quadMesh) return;
        glBindFramebuffer(GL_FRAMEBUFFER, s_bloomFBO);

        // 1. Downsample
        s_downsampleShader->Use();
        uint32_t currentSrc = inputTexture;
        glm::vec2 currentSize((float)width, (float)height);

        for (auto& mip : s_bloomMips) {
            glViewport(0, 0, mip.size.x, mip.size.y);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, currentSrc);
            s_downsampleShader->SetVec2U("srcResolution", currentSize);
            
            s_quadMesh->Draw();

            currentSrc = mip.texture;
            currentSize = glm::vec2((float)mip.size.x, (float)mip.size.y);
        }

        // 2. Upsample (additive blending)
        s_upsampleShader->Use();
        s_upsampleShader->SetFloatU("filterRadius", 0.005f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        for (int i = (int)s_bloomMips.size() - 1; i > 0; i--) {
            auto& mip = s_bloomMips[i];
            auto& nextMip = s_bloomMips[i - 1];

            glViewport(0, 0, nextMip.size.x, nextMip.size.y);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.texture, 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mip.texture);
            
            s_quadMesh->Draw();
        }

        glDisable(GL_BLEND);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PostProcessing::RenderFinalComposition(uint32_t sceneTex, uint32_t bloomTex, float exposure, float gamma) {
        if (!s_compositionShader || !s_quadMesh) return;
        glViewport(0, 0, Engine::Get().GetWindow().GetWidth(), Engine::Get().GetWindow().GetHeight());
        s_compositionShader->Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneTex);
        s_compositionShader->SetIntU("sceneTexture", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, bloomTex);
        s_compositionShader->SetIntU("bloomTexture", 1);

        s_compositionShader->SetFloatU("exposure", exposure);
        s_compositionShader->SetFloatU("gamma", gamma);

        s_quadMesh->Draw();
    }

    uint32_t PostProcessing::GetBloomTexture() {
        if (s_bloomMips.empty()) return 0;
        return s_bloomMips[0].texture;
    }

}
