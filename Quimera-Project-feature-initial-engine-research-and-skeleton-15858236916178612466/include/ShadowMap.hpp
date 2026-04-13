#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.hpp"
#include <memory>

namespace Vamos {
    // Shadow Map system inspired by Wicked Engine's cascaded shadow approach
    // Simplified to a single directional light shadow map for now
    class ShadowMap {
    public:
        static constexpr unsigned int SHADOW_WIDTH = 2048;
        static constexpr unsigned int SHADOW_HEIGHT = 2048;

        ShadowMap() = default;

        void Initialize() {
            // Create depth FBO
            glGenFramebuffers(1, &depthMapFBO);
            
            // Create depth texture
            glGenTextures(1, &depthMap);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
            
            // Attach to FBO
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Load shadow depth shader
            depthShader = std::make_unique<Shader>("assets/shaders/shadow_depth.vert", "assets/shaders/shadow_depth.frag");
            
            initialized = true;
        }

        // Begin shadow pass — binds depth FBO and sets up light space matrix
        void BeginShadowPass(const glm::vec3& lightDir, const glm::vec3& sceneCenter, float orthoSize = 40.0f) {
            if (!initialized) return;

            // Compute light-space matrices (directional light as orthographic)
            float near_plane = 0.1f, far_plane = 100.0f;
            glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, near_plane, far_plane);
            
            glm::vec3 lightPos = sceneCenter - glm::normalize(lightDir) * 30.0f;
            glm::mat4 lightView = glm::lookAt(lightPos, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
            
            lightSpaceMatrix = lightProjection * lightView;

            // Bind depth FBO
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);

            depthShader->Use();
            depthShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
        }

        // Render a single mesh into the shadow map
        void RenderMesh(const glm::mat4& modelMatrix) {
            if (!initialized || !depthShader) return;
            depthShader->SetMat4("model", modelMatrix);
        }

        // End shadow pass — restores default framebuffer
        void EndShadowPass(int screenWidth, int screenHeight) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, screenWidth, screenHeight);
        }

        // Bind shadow map texture for sampling in the main render pass
        void BindShadowMapTexture(unsigned int textureUnit = 5) {
            if (!initialized) return;
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, depthMap);
        }

        glm::mat4 GetLightSpaceMatrix() const { return lightSpaceMatrix; }
        unsigned int GetDepthMap() const { return depthMap; }
        bool IsInitialized() const { return initialized; }

        ~ShadowMap() {
            if (depthMapFBO) glDeleteFramebuffers(1, &depthMapFBO);
            if (depthMap) glDeleteTextures(1, &depthMap);
        }

    private:
        unsigned int depthMapFBO = 0;
        unsigned int depthMap = 0;
        glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
        std::unique_ptr<Shader> depthShader;
        bool initialized = false;
    };
}
