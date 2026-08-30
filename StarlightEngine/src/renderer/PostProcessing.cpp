#include "PostProcessing.hpp"
#include <glad/glad.h>
#include "Engine.hpp"
#include "Renderer.hpp"
#include "FSRSystem.hpp"
#include "CVarSystem.hpp"

namespace starlight {

    std::vector<PostProcessing::BloomMip> PostProcessing::s_bloomMips;
    uint32_t PostProcessing::s_bloomFBO = 0;
    static FSRSystem s_fsrSystem;
    static bool s_fsrInitialized = false;
    static uint32_t s_fsrInputFBO = 0;
    static uint32_t s_fsrInputTex = 0;
    static int s_fsrInputW = 0;
    static int s_fsrInputH = 0;
    std::shared_ptr<Shader> PostProcessing::s_downsampleShader;
    std::shared_ptr<Shader> PostProcessing::s_upsampleShader;
    std::shared_ptr<Shader> PostProcessing::s_compositionShader;
    std::shared_ptr<Mesh> PostProcessing::s_quadMesh;
    float PostProcessing::s_chromaticStrength = 0.0f;
    float PostProcessing::s_vignetteStrength = 0.0f;

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
        if (s_bloomFBO != 0) {
            glDeleteFramebuffers(1, &s_bloomFBO);
            s_bloomFBO = 0;
        }
        s_downsampleShader.reset();
        s_upsampleShader.reset();
        s_compositionShader.reset();
        s_quadMesh.reset();

        s_fsrSystem.Shutdown();
        s_fsrInitialized = false;
        if (s_fsrInputFBO != 0) {
            glDeleteFramebuffers(1, &s_fsrInputFBO);
            s_fsrInputFBO = 0;
        }
        if (s_fsrInputTex != 0) {
            glDeleteTextures(1, &s_fsrInputTex);
            s_fsrInputTex = 0;
        }
        s_fsrInputW = 0;
        s_fsrInputH = 0;
    }

    void PostProcessing::RenderBloom(uint32_t inputTexture, uint32_t width, uint32_t height) {
        if (!s_downsampleShader || !s_upsampleShader || !s_quadMesh) return;

        // Dynamically recreate bloom mips if width/height changed
        if (s_bloomMips.empty() || s_bloomMips[0].size.x != (int)width / 2 || s_bloomMips[0].size.y != (int)height / 2) {
            for (auto& mip : s_bloomMips) {
                glDeleteTextures(1, &mip.texture);
            }
            s_bloomMips.clear();

            glm::ivec2 mipIntSize((int)width, (int)height);
            for (int i = 0; i < 6; i++) {
                mipIntSize /= 2;
                if (mipIntSize.x < 1) mipIntSize.x = 1;
                if (mipIntSize.y < 1) mipIntSize.y = 1;

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
        }

        glBindFramebuffer(GL_FRAMEBUFFER, s_bloomFBO);

        // 1. Downsample
        s_downsampleShader->Use();
        uint32_t currentSrc = inputTexture;
        glm::vec2 currentSize((float)width, (float)height);
        auto cvarSys = Engine::Get().GetSystem<CVarSystem>();
        float threshold = cvarSys ? cvarSys->GetFloat("r_bloom_threshold") : 1.0f;
        if (threshold <= 0.0f) threshold = 1.0f;

        s_downsampleShader->SetFloatU("threshold", threshold);

        int mipIndex = 0;
        for (auto& mip : s_bloomMips) {
            glViewport(0, 0, mip.size.x, mip.size.y);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, currentSrc);
            s_downsampleShader->SetVec2U("srcResolution", currentSize);
            s_downsampleShader->SetIntU("isFirstMip", mipIndex == 0 ? 1 : 0);
            
            s_quadMesh->Draw();

            currentSrc = mip.texture;
            currentSize = glm::vec2((float)mip.size.x, (float)mip.size.y);
            mipIndex++;
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

    void PostProcessing::RenderFinalComposition(uint32_t sceneTex, uint32_t bloomTex, float exposure, float gamma, uint32_t targetFBO, int vpW, int vpH) {
        if (!s_compositionShader || !s_quadMesh) {
            auto& renderer = Engine::Get().GetRenderer();
            glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer.GetFBO());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFBO);
            glBlitFramebuffer(0, 0, renderer.GetFBOWidth(), renderer.GetFBOHeight(), 0, 0, vpW, vpH, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, targetFBO);
            return;
        }

        auto cvarSys = Engine::Get().GetSystem<CVarSystem>();
        bool useFSR = cvarSys ? cvarSys->GetBool("r_fsr") : false;
        float sharpness = cvarSys ? cvarSys->GetFloat("r_fsr_sharpness") : 0.8f;

        auto& renderer = Engine::Get().GetRenderer();
        int internalW = renderer.GetFBOWidth();
        int internalH = renderer.GetFBOHeight();

        if (useFSR) {
            // Recreate temporary low-res target for FSR if size changed
            if (s_fsrInputTex == 0 || s_fsrInputW != internalW || s_fsrInputH != internalH) {
                if (s_fsrInputFBO == 0) {
                    glGenFramebuffers(1, &s_fsrInputFBO);
                }
                if (s_fsrInputTex != 0) {
                    glDeleteTextures(1, &s_fsrInputTex);
                }
                
                s_fsrInputW = internalW;
                s_fsrInputH = internalH;
                
                glBindFramebuffer(GL_FRAMEBUFFER, s_fsrInputFBO);
                glGenTextures(1, &s_fsrInputTex);
                glBindTexture(GL_TEXTURE_2D, s_fsrInputTex);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, internalW, internalH, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_fsrInputTex, 0);
            }

            // Render Composition to low-res FSR Input target
            glBindFramebuffer(GL_FRAMEBUFFER, s_fsrInputFBO);
            glViewport(0, 0, internalW, internalH);

            s_compositionShader->Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sceneTex);
            s_compositionShader->SetIntU("sceneTexture", 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, bloomTex);
            s_compositionShader->SetIntU("bloomTexture", 1);

            s_compositionShader->SetFloatU("exposure", exposure);
            s_compositionShader->SetFloatU("gamma", gamma);
            s_compositionShader->SetFloatU("chromaticStrength", s_chromaticStrength);
            s_compositionShader->SetFloatU("vignetteStrength", s_vignetteStrength);
            
            bool useCRT = cvarSys ? cvarSys->GetBool("r_crt") : false;
            s_compositionShader->SetIntU("useCRT", useCRT ? 1 : 0);
            s_compositionShader->SetFloatU("uTime", Engine::Get().GetTime().totalTime);

            s_quadMesh->Draw();

            // Initialize FSR context if not done
            if (!s_fsrInitialized) {
                FfxFsrContextDescription desc{};
                desc.flags = 0;
                desc.maxRenderSizeWidth = internalW;
                desc.maxRenderSizeHeight = internalH;
                desc.displaySizeWidth = vpW;
                desc.displaySizeHeight = vpH;
                s_fsrSystem.Initialize(desc);
                s_fsrInitialized = true;
            }

            // Dispatch FSR upscaling from low-res FSR Input target to final high-res target FBO
            s_fsrSystem.Dispatch(s_fsrInputTex, targetFBO, internalW, internalH, vpW, vpH, sharpness);

        } else {
            // Standard render straight to final target FBO
            glBindFramebuffer(GL_FRAMEBUFFER, targetFBO);
            glViewport(0, 0, vpW, vpH);

            s_compositionShader->Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sceneTex);
            s_compositionShader->SetIntU("sceneTexture", 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, bloomTex);
            s_compositionShader->SetIntU("bloomTexture", 1);

            s_compositionShader->SetFloatU("exposure", exposure);
            s_compositionShader->SetFloatU("gamma", gamma);
            s_compositionShader->SetFloatU("chromaticStrength", s_chromaticStrength);
            s_compositionShader->SetFloatU("vignetteStrength", s_vignetteStrength);

            bool useCRT = cvarSys ? cvarSys->GetBool("r_crt") : false;
            s_compositionShader->SetIntU("useCRT", useCRT ? 1 : 0);
            s_compositionShader->SetFloatU("uTime", Engine::Get().GetTime().totalTime);

            s_quadMesh->Draw();
        }
    }

    uint32_t PostProcessing::GetBloomTexture() {
        if (s_bloomMips.empty()) return 0;
        return s_bloomMips[0].texture;
    }

}
