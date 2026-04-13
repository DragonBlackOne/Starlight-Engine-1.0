#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Shader.hpp"

namespace titan {

    class SSAO_System {
    public:
        SSAO_System() = default;
        static SSAO_System& Get();

        void Initialize();
        void Render(unsigned int gPosition, unsigned int gNormal, const glm::mat4& projection);
        
        unsigned int GetSSAOTexture() const { return m_ssaoColorBuffer; }

    private:
        unsigned int m_ssaoFBO, m_ssaoBlurFBO;
        unsigned int m_ssaoColorBuffer, m_ssaoColorBufferBlur;
        unsigned int m_noiseTexture;

        std::vector<glm::vec3> m_ssaoKernel;
        std::shared_ptr<Shader> m_ssaoShader;
        std::shared_ptr<Shader> m_ssaoBlurShader;

        void SetupKernel();
        void SetupNoise();
    };

}
