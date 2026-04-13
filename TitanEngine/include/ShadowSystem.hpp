#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Shader.hpp"

namespace titan {

    constexpr int MAX_CASCADES = 4;

    class ShadowSystem {
    public:
        ShadowSystem(int resolution = 2048);
        ~ShadowSystem();

        void Initialize();
        void CalculateCascades(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& lightDir, float nearP, float farP);
        
        void BeginPass(int cascadeIndex);
        void EndPass(int screenWidth, int screenHeight);

        void BindTextures(uint32_t slot = 1);
        
        uint32_t GetDepthTextureArray() const { return m_depthTextureArray; }
        const glm::mat4* GetLightSpaceMatrices() const { return m_lightSpaceMatrices; }
        const float* GetCascadeSplits() const { return m_cascadeSplits; }

    private:
        uint32_t m_fbo = 0;
        uint32_t m_depthTextureArray = 0;
        std::shared_ptr<Shader> m_shadowShader;
        
        int m_resolution;
        float m_cascadeSplits[MAX_CASCADES];
        glm::mat4 m_lightSpaceMatrices[MAX_CASCADES];
    };

}
