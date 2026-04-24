// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "SSAO_System.hpp"
#include <random>
#include <glad/glad.h>

namespace starlight {

    SSAO_System& SSAO_System::Get() {
        static SSAO_System instance;
        return instance;
    }

    void SSAO_System::Initialize() {
        SetupKernel();
        SetupNoise();

        // Shaders
        m_ssaoShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/ssao.frag");
        m_ssaoBlurShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/ssao_blur.frag");

        // FBOs and Textures (1280x720)
        glGenFramebuffers(1, &m_ssaoFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO);
        glGenTextures(1, &m_ssaoColorBuffer);
        glBindTexture(GL_TEXTURE_2D, m_ssaoColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1280, 720, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoColorBuffer, 0);

        glGenFramebuffers(1, &m_ssaoBlurFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoBlurFBO);
        glGenTextures(1, &m_ssaoColorBufferBlur);
        glBindTexture(GL_TEXTURE_2D, m_ssaoColorBufferBlur);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1280, 720, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoColorBufferBlur, 0);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SSAO_System::SetupKernel() {
        std::uniform_real_distribution<float> randomValues(0.0, 1.0);
        std::default_random_engine generator;
        for (unsigned int i = 0; i < 64; ++i) {
            glm::vec3 sample(randomValues(generator) * 2.0 - 1.0, randomValues(generator) * 2.0 - 1.0, randomValues(generator));
            sample = glm::normalize(sample);
            sample *= randomValues(generator);
            float scale = (float)i / 64.0f;
            scale = 0.1f + scale * scale * (1.0f - 0.1f);
            m_ssaoKernel.push_back(sample * scale);
        }
    }

    void SSAO_System::SetupNoise() {
        std::uniform_real_distribution<float> randomValues(0.0, 1.0);
        std::default_random_engine generator;
        std::vector<glm::vec3> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++) {
            glm::vec3 noise(randomValues(generator) * 2.0 - 1.0, randomValues(generator) * 2.0 - 1.0, 0.0f);
            ssaoNoise.push_back(noise);
        }
        glGenTextures(1, &m_noiseTexture);
        glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    void SSAO_System::Render(unsigned int gPosition, unsigned int gNormal, const glm::mat4& projection) {
        // Pass 1: AO
        glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        m_ssaoShader->Use();
        for (unsigned int i = 0; i < 64; ++i)
            m_ssaoShader->SetVec3("samples[" + std::to_string(i) + "]", m_ssaoKernel[i]);
        m_ssaoShader->SetMat4("projection", projection);
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
        m_ssaoShader->SetInt("gPosition", 0);
        m_ssaoShader->SetInt("gNormal", 1);
        m_ssaoShader->SetInt("texNoise", 2);

        // Pass 2: Blur
        glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        m_ssaoBlurShader->Use();
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_ssaoColorBuffer);
        m_ssaoBlurShader->SetInt("ssaoInput", 0);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}
