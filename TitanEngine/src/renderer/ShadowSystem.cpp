#include "ShadowSystem.hpp"
#include "Log.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace titan {

    ShadowSystem::ShadowSystem(int resolution) 
        : m_resolution(resolution) {}

    ShadowSystem::~ShadowSystem() {
        if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
        if (m_depthTextureArray) glDeleteTextures(1, &m_depthTextureArray);
    }

    void ShadowSystem::Initialize() {
        glGenFramebuffers(1, &m_fbo);
        
        glGenTextures(1, &m_depthTextureArray);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_depthTextureArray);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, m_resolution, m_resolution, MAX_CASCADES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        
        float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTextureArray, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Log::Error("ShadowSystem: Shadow Array FBO incomplete!");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Load Shadow Shader (usually very simple vertex shader)
        m_shadowShader = std::make_shared<Shader>(
            "#version 450 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "uniform mat4 lightSpaceMatrix;\n"
            "uniform mat4 model;\n"
            "void main() {\n"
            "    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);\n"
            "}\n",
            "#version 450 core\n"
            "void main() {}"
        );
        
        Log::Info("ShadowSystem: Initialized with {}x{} resolution ({} cascades).", m_resolution, m_resolution, MAX_CASCADES);
    }

    void ShadowSystem::CalculateCascades(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& lightDir, float nearP, float farP) {
        // Cascade split definitions (exponential for more detail near camera)
        m_cascadeSplits[0] = farP / 50.0f;
        m_cascadeSplits[1] = farP / 25.0f;
        m_cascadeSplits[2] = farP / 10.0f;
        m_cascadeSplits[3] = farP;

        glm::mat4 invVP = glm::inverse(projection * view);

        for (int i = 0; i < MAX_CASCADES; ++i) {
            // Calculate 8 frustum corners in world space for this slice
            // (Simplified version: using full frustum for now, can be optimized per split)
            float prevSplit = (i == 0) ? nearP : m_cascadeSplits[i - 1];
            float nextSplit = m_cascadeSplits[i];

            std::vector<glm::vec4> corners;
            for (int x = 0; x < 2; ++x) {
                for (int y = 0; y < 2; ++y) {
                    for (int z = 0; z < 2; ++z) {
                        glm::vec4 pt = invVP * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                        corners.push_back(pt / pt.w);
                    }
                }
            }

            glm::vec3 center(0.0f);
            for (const auto& v : corners) center += glm::vec3(v);
            center /= (float)corners.size();

            glm::mat4 lightView = glm::lookAt(center + lightDir, center, glm::vec3(0, 1, 0));

            float minX = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::lowest();
            float minY = std::numeric_limits<float>::max();
            float maxY = std::numeric_limits<float>::lowest();
            float minZ = std::numeric_limits<float>::max();
            float maxZ = std::numeric_limits<float>::lowest();

            for (const auto& v : corners) {
                glm::vec4 trf = lightView * v;
                minX = std::min(minX, trf.x); maxX = std::max(maxX, trf.x);
                minY = std::min(minY, trf.y); maxY = std::max(maxY, trf.y);
                minZ = std::min(minZ, trf.z); maxZ = std::max(maxZ, trf.z);
            }

            // Tune Z bounds to catch objects outside frustum casting shadows
            float zMult = 5.0f;
            if (minZ < 0) minZ *= zMult; else minZ /= zMult;
            if (maxZ < 0) maxZ /= zMult; else maxZ *= zMult;

            glm::mat4 lightProj = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
            m_lightSpaceMatrices[i] = lightProj * lightView;
        }
    }

    void ShadowSystem::BeginPass(int cascadeIndex) {
        glViewport(0, 0, m_resolution, m_resolution);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTextureArray, 0, cascadeIndex);
        glClear(GL_DEPTH_BUFFER_BIT);
        m_shadowShader->Use();
        m_shadowShader->SetMat4("lightSpaceMatrix", m_lightSpaceMatrices[cascadeIndex]);
    }

    void ShadowSystem::EndPass(int screenWidth, int screenHeight) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screenWidth, screenHeight);
    }

    void ShadowSystem::BindTextures(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_depthTextureArray);
    }

}
