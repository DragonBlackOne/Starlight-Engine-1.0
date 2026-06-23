#include "IBLSystem.hpp"
#include "PathResolver.hpp"
#include "Log.hpp"
#include "stb_image.h"

namespace starlight {

    IBLSystem::IBLSystem() {
        SetupShaders();
    }

    IBLSystem::~IBLSystem() {
        m_equiToCubeShader.reset();
        m_irradianceShader.reset();
        m_prefilterShader.reset();
        m_brdfShader.reset();
        m_cubeMesh.reset();
        m_quadMesh.reset();
    }

    void IBLSystem::SetupShaders() {
        m_equiToCubeShader = Shader::LoadFromFile("assets/shaders/ibl/equi_to_cube.vert", "assets/shaders/ibl/equi_to_cube.frag");
        m_irradianceShader = Shader::LoadFromFile("assets/shaders/ibl/equi_to_cube.vert", "assets/shaders/ibl/irradiance.frag");
        m_prefilterShader  = Shader::LoadFromFile("assets/shaders/ibl/equi_to_cube.vert", "assets/shaders/ibl/prefilter.frag");
        m_brdfShader       = Shader::LoadFromFile("assets/shaders/ibl/brdf.vert", "assets/shaders/ibl/brdf.frag");
    }

    IBLData IBLSystem::ProcessHDR(const std::string& hdrPath) {
        IBLData data;
        
        // 1. Create Cube and Quad Meshes if they do not exist
        if (!m_cubeMesh) {
            std::vector<Vertex> cubeVertices = {
                // Back face
                {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {0,0,0,0}, {0,0,0,0}},
                {{ 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {0,0,0,0}, {0,0,0,0}},
                {{ 1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {0,0,0,0}, {0,0,0,0}},
                {{-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {0,0,0,0}, {0,0,0,0}},
                // Front face
                {{-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f,  1.0f}, {0.0f, 0.0f}, {0,0,0,0}, {0,0,0,0}},
                {{ 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f,  1.0f}, {1.0f, 0.0f}, {0,0,0,0}, {0,0,0,0}},
                {{ 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f,  1.0f}, {1.0f, 1.0f}, {0,0,0,0}, {0,0,0,0}},
                {{-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f,  1.0f}, {0.0f, 1.0f}, {0,0,0,0}, {0,0,0,0}},
                // Left face
                {{-1.0f,  1.0f,  1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0,0,0,0}, {0,0,0,0}},
                {{-1.0f,  1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0,0,0,0}, {0,0,0,0}},
                {{-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0,0,0,0}, {0,0,0,0}},
                {{-1.0f, -1.0f,  1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0,0,0,0}, {0,0,0,0}},
                // Right face
                {{ 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0,0,0,0}, {0,0,0,0}},
                {{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0,0,0,0}, {0,0,0,0}},
                {{ 1.0f,  1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0,0,0,0}, {0,0,0,0}},
                {{ 1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0,0,0,0}, {0,0,0,0}},
                // Bottom face
                {{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {0,0,0,0}, {0,0,0,0}},
                {{ 1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {0,0,0,0}, {0,0,0,0}},
                {{ 1.0f, -1.0f,  1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {0,0,0,0}, {0,0,0,0}},
                {{-1.0f, -1.0f,  1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0,0,0,0}, {0,0,0,0}},
                // Top face
                {{-1.0f,  1.0f, -1.0f}, {0.0f,  1.0f, 0.0f}, {0.0f, 1.0f}, {0,0,0,0}, {0,0,0,0}},
                {{-1.0f,  1.0f,  1.0f}, {0.0f,  1.0f, 0.0f}, {0.0f, 0.0f}, {0,0,0,0}, {0,0,0,0}},
                {{ 1.0f,  1.0f,  1.0f}, {0.0f,  1.0f, 0.0f}, {1.0f, 0.0f}, {0,0,0,0}, {0,0,0,0}},
                {{ 1.0f,  1.0f, -1.0f}, {0.0f,  1.0f, 0.0f}, {1.0f, 1.0f}, {0,0,0,0}, {0,0,0,0}}
            };
            std::vector<uint32_t> cubeIndices = {
                0, 2, 1, 0, 1, 3,
                4, 5, 6, 4, 6, 7,
                8, 9, 10, 8, 10, 11,
                12, 14, 13, 12, 13, 15,
                16, 17, 18, 16, 18, 19,
                20, 22, 21, 20, 21, 23
            };
            m_cubeMesh = std::make_shared<Mesh>(cubeVertices, cubeIndices);
        }
        if (!m_quadMesh) {
            std::vector<Vertex> quadVertices = {
                {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0,0,0,0}, {0,0,0,0}},
                {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0,0,0,0}, {0,0,0,0}},
                {{ 1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0,0,0,0}, {0,0,0,0}},
                {{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0,0,0,0}, {0,0,0,0}}
            };
            std::vector<uint32_t> quadIndices = { 0, 1, 2, 1, 3, 2 };
            m_quadMesh = std::make_shared<Mesh>(quadVertices, quadIndices);
        }

        std::string resolved = PathResolver::Resolve(hdrPath);

        // 2. Load HDR Image
        stbi_set_flip_vertically_on_load(true);
        int width, height, nrComponents;
        float* hdrData = stbi_loadf(resolved.c_str(), &width, &height, &nrComponents, 0);
        if (!hdrData) {
            Log::Error("IBLSystem: Failed to load HDR: " + resolved);
            return data;
        }

        uint32_t hdrTexture;
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, hdrData);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(hdrData);

        // 3. Setup Framebuffer for rendering to cubemap
        uint32_t captureFBO, captureRBO;
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

        // 4. Create Environment Cubemap
        glGenTextures(1, &data.envCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, data.envCubemap);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 5. Setup Projection and View Matrices
        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] = {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

        // Render HDR to Cubemap
        m_equiToCubeShader->Use();
        m_equiToCubeShader->SetMat4U("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        m_equiToCubeShader->SetIntU("equirectangularMap", 0);

        glViewport(0, 0, 512, 512);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for (unsigned int i = 0; i < 6; ++i) {
            m_equiToCubeShader->SetMat4U("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, data.envCubemap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_cubeMesh->Draw();
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, data.envCubemap);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 6. Create Irradiance Cubemap
        glGenTextures(1, &data.irradianceMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, data.irradianceMap);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

        m_irradianceShader->Use();
        m_irradianceShader->SetMat4U("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, data.envCubemap);
        m_irradianceShader->SetIntU("environmentMap", 0);

        glViewport(0, 0, 32, 32);
        for (unsigned int i = 0; i < 6; ++i) {
            m_irradianceShader->SetMat4U("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, data.irradianceMap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_cubeMesh->Draw();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 7. Create Prefilter Cubemap
        glGenTextures(1, &data.prefilterMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, data.prefilterMap);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        m_prefilterShader->Use();
        m_prefilterShader->SetMat4U("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, data.envCubemap);
        m_prefilterShader->SetIntU("environmentMap", 0);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        unsigned int maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
            unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5f, mip));
            unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5f, mip));
            glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);

            float roughness = (float)mip / (float)(maxMipLevels - 1);
            m_prefilterShader->SetFloatU("roughness", roughness);
            for (unsigned int i = 0; i < 6; ++i) {
                m_prefilterShader->SetMat4U("view", captureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, data.prefilterMap, mip);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                m_cubeMesh->Draw();
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 8. Create BRDF LUT Texture
        glGenTextures(1, &data.brdfLUT);
        glBindTexture(GL_TEXTURE_2D, data.brdfLUT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data.brdfLUT, 0);

        glViewport(0, 0, 512, 512);
        m_brdfShader->Use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_quadMesh->Draw();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 9. Clean up resources
        glDeleteFramebuffers(1, &captureFBO);
        glDeleteRenderbuffers(1, &captureRBO);
        glDeleteTextures(1, &hdrTexture);

        Log::Info("IBLSystem: HDR Processed successfully: " + resolved);
        return data;
    }

}
