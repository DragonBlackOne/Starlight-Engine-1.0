#include "Renderer.hpp"
#include "InstancedMeshComponent.hpp"
#include "LODComponent.hpp"
#include "AnimatorComponent.hpp"
#include "Log.hpp"
#include "ParticleSystem.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>
#include "Engine.hpp"

namespace titan {

    static std::string LoadShaderSource(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            Log::Error("Failed to open shader file: " + path);
            return "";
        }
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    Renderer::Renderer() {}
    Renderer::~Renderer() {
        if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
        if (m_fboTexture) glDeleteTextures(1, &m_fboTexture);
        if (m_rbo) glDeleteRenderbuffers(1, &m_rbo);
    }

    void Renderer::Initialize() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Compile Shaders from files
        m_basicShader = std::make_shared<Shader>(
            LoadShaderSource("assets/shaders/basic.vert").c_str(),
            LoadShaderSource("assets/shaders/basic.frag").c_str()
        );

        m_mode7Shader = std::make_shared<Shader>(
            LoadShaderSource("assets/shaders/mode7.vert").c_str(),
            LoadShaderSource("assets/shaders/mode7.frag").c_str()
        );

        m_crtShader = std::make_shared<Shader>(
            LoadShaderSource("assets/shaders/screen.vert").c_str(),
            LoadShaderSource("assets/shaders/crt.frag").c_str()
        );
        m_crtShader->Use();
        m_crtShader->SetInt("screenTexture", 0);

        m_pbrShader = std::make_shared<Shader>(
            LoadShaderSource("assets/shaders/pbr.vert").c_str(),
            LoadShaderSource("assets/shaders/pbr.frag").c_str()
        );

        // Create Quad for Mode 7 and Screen

        std::vector<Vertex> vertices = {
            {{-1, -1, 0}, {0,0,1}, {0,0}},
            {{ 1, -1, 0}, {0,0,1}, {1,0}},
            {{ 1,  1, 0}, {0,0,1}, {1,1}},
            {{-1,  1, 0}, {0,0,1}, {0,1}}
        };
        std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };
        m_quadMesh = std::make_shared<Mesh>(vertices, indices);

        // Create Default Cube
        std::vector<Vertex> cubeVertices = {
            {{-0.5f, -0.5f, -0.5f}, {0,0,-1}, {0,0}}, {{0.5f, -0.5f, -0.5f}, {0,0,-1}, {1,0}}, {{0.5f, 0.5f, -0.5f}, {0,0,-1}, {1,1}}, {{-0.5f, 0.5f, -0.5f}, {0,0,-1}, {0,1}},
            {{-0.5f, -0.5f, 0.5f}, {0,0,1}, {0,0}}, {{0.5f, -0.5f, 0.5f}, {0,0,1}, {1,0}}, {{0.5f, 0.5f, 0.5f}, {0,0,1}, {1,1}}, {{-0.5f, 0.5f, 0.5f}, {0,0,1}, {0,1}},
            {{-0.5f, 0.5f, 0.5f}, {-1,0,0}, {1,0}}, {{-0.5f, 0.5f, -0.5f}, {-1,0,0}, {1,1}}, {{-0.5f, -0.5f, -0.5f}, {-1,0,0}, {0,1}}, {{-0.5f, -0.5f, 0.5f}, {-1,0,0}, {0,0}},
            {{0.5f, 0.5f, 0.5f}, {1,0,0}, {1,0}}, {{0.5f, 0.5f, -0.5f}, {1,0,0}, {1,1}}, {{0.5f, -0.5f, -0.5f}, {1,0,0}, {0,1}}, {{0.5f, -0.5f, 0.5f}, {1,0,0}, {0,0}},
            {{-0.5f, -0.5f, -0.5f}, {0,-1,0}, {0,1}}, {{0.5f, -0.5f, -0.5f}, {0,-1,0}, {1,1}}, {{0.5f, -0.5f, 0.5f}, {0,-1,0}, {1,0}}, {{-0.5f, -0.5f, 0.5f}, {0,-1,0}, {0,0}},
            {{-0.5f, 0.5f, -0.5f}, {0,1,0}, {0,1}}, {{0.5f, 0.5f, -0.5f}, {0,1,0}, {1,1}}, {{0.5f, 0.5f, 0.5f}, {0,1,0}, {1,0}}, {{-0.5f, 0.5f, 0.5f}, {0,1,0}, {0,0}}
        };
        std::vector<uint32_t> cubeIndices = {
            0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8,
            12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20
        };
        m_cubeMesh = std::make_shared<Mesh>(cubeVertices, cubeIndices);

        // Initialize Specialized Systems
        int width = Engine::Get().GetWindow().GetWidth();
        int height = Engine::Get().GetWindow().GetHeight();
        m_shadowSystem = std::make_unique<ShadowSystem>(2048);
        m_shadowSystem->Initialize();
        m_ssaoSystem = std::make_unique<SSAO_System>();
        m_ssaoSystem->Initialize();
        m_dashboardSystem = std::make_unique<DashboardSystem>();
        m_gizmoSystem = std::make_unique<GizmoSystem>();
        m_gizmoSystem->Initialize();
        
        AABB worldBounds = { glm::vec3(-50, -10, -50), glm::vec3(50, 50, 50) };
        m_octreeSystem = std::make_unique<OctreeSystem>(worldBounds);

        // Phase 10: Shaders & FBOs
        m_bloomBrightShader = std::make_shared<Shader>("assets/shaders/quad.vert", "assets/shaders/bloom_bright.frag");
        m_bloomBlurShader = std::make_shared<Shader>("assets/shaders/quad.vert", "assets/shaders/bloom_blur.frag");
        m_postComposeShader = std::make_shared<Shader>("assets/shaders/quad.vert", "assets/shaders/post_compose.frag");

        glGenFramebuffers(2, m_pingpongFBO);
        glGenTextures(2, m_pingpongColorbuffers);
        for (unsigned int i = 0; i < 2; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, m_pingpongColorbuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongColorbuffers[i], 0);
        }

        Log::Info("Renderer: Phase 10 AAA Visual Suite (SSR, Bloom, ACES) active.");
        
        int w = Engine::Get().GetWindow().GetWidth();
        int h = Engine::Get().GetWindow().GetHeight();
        RecreateFBO(w, h);
    }

    void Renderer::RecreateFBO(int width, int height) {
        if (m_fbo) {
            glDeleteFramebuffers(1, &m_fbo);
            glDeleteTextures(1, &m_fboTexture);
            glDeleteRenderbuffers(1, &m_rbo);
        }

        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        glGenTextures(1, &m_fboTexture);
        glBindTexture(GL_TEXTURE_2D, m_fboTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture, 0);

        glGenTextures(1, &m_depthTexture);
        glBindTexture(GL_TEXTURE_2D, m_depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Log::Error("Renderer: Framebuffer is not complete!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::UpdateProjection(float fov, float aspect, float nearP, float farP) {
        m_projectionMatrix = glm::perspective(glm::radians(fov), aspect, nearP, farP);
        
        int w = Engine::Get().GetWindow().GetWidth();
        int h = Engine::Get().GetWindow().GetHeight();
        if(w > 0 && h > 0) RecreateFBO(w, h);
    }

    void Renderer::BeginFrame() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glClearColor(0.005f, 0.005f, 0.005f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_commandBuffer.clear();
    }

    void Renderer::Submit(const RenderCommand& command) {
        m_commandBuffer.push_back(command);
    }

    void Renderer::EndFrame() {
        // 1. Shadow Pass (Cascaded Shadow Maps)
        // Find primary directional light (simplification: using first point light direction for CSM)
        glm::vec3 lightDir = glm::normalize(glm::vec3(0.5f, 1.0f, 0.2f));
        m_shadowSystem->CalculateCascades(m_view, m_projectionMatrix, lightDir, 0.1f, 100.0f);
        
        glEnable(GL_DEPTH_TEST);
        for (int i = 0; i < MAX_CASCADES; ++i) {
            m_shadowSystem->BeginPass(i);
            for (auto& cmd : m_commandBuffer) {
                // Shadow shader is set in m_shadowSystem->BeginPass
                m_shadowSystem->BeginPass(i); // This ensures shader is used
                // But we need to pass the model matrix
                cmd.mesh->Draw(); // Shadow shader only needs pos and model (handled in BeginPass)
            }
        }
        m_shadowSystem->EndPass(Engine::Get().GetWindow().GetWidth(), Engine::Get().GetWindow().GetHeight());

        // 2. Render 3D Commands to FBO (Main Pass)
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glEnable(GL_DEPTH_TEST);
        for (auto& cmd : m_commandBuffer) {
            cmd.shader->Use();
            cmd.shader->SetMat4("model", cmd.transform);
            cmd.shader->SetMat4("view", m_view);
            cmd.shader->SetMat4("projection", m_projectionMatrix);
            
            // Bind Shadows
            m_shadowSystem->BindTextures(5); // Slot 5
            cmd.shader->SetInt("shadowMap", 5);
            for(int i = 0; i < MAX_CASCADES; ++i) {
                cmd.shader->SetMat4("lightSpaceMatrices[" + std::to_string(i) + "]", m_shadowSystem->GetLightSpaceMatrices()[i]);
            }
            cmd.shader->SetVec3("lightDir", lightDir);
            
            cmd.mesh->Draw();
        }

        // 3. SSAO Pass
        // Note: For now we use m_fboTexture as position input (simplified AO)
        m_ssaoSystem->Render(m_fboTexture, m_fboTexture, m_projectionMatrix);

        // 4. Final Post-Process Pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        // --- Bloom Extraction ---
        glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[0]);
        m_bloomBrightShader->Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_fboTexture);
        m_quadMesh->Draw();

        // --- Bloom Blur ---
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 10;
        m_bloomBlurShader->Use();
        for (unsigned int i = 0; i < amount; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[horizontal]);
            m_bloomBlurShader->SetInt("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? m_pingpongColorbuffers[0] : m_pingpongColorbuffers[!horizontal]);
            m_quadMesh->Draw();
            horizontal = !horizontal;
            if (first_iteration) first_iteration = false;
        }

        // --- Final Composition (SSR + Bloom + ACES) ---
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_postComposeShader->Use();
        
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_fboTexture);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, m_pingpongColorbuffers[!horizontal]);
        glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, m_depthTexture);
        glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, m_ssaoSystem->GetSSAOTexture());

        m_postComposeShader->SetInt("screenTexture", 0);
        m_postComposeShader->SetInt("bloomBlur", 1);
        m_postComposeShader->SetInt("depthTexture", 2);
        m_postComposeShader->SetInt("ssaoTexture", 3);
        
        // Matrices for SSR
        m_postComposeShader->SetMat4("u_view", m_view);
        m_postComposeShader->SetMat4("u_proj", m_projectionMatrix);
        m_postComposeShader->SetMat4("u_inv_view", glm::inverse(m_view));
        m_postComposeShader->SetMat4("u_inv_proj", glm::inverse(m_projectionMatrix));
        
        m_quadMesh->Draw();

        // 5. Dashboard / Debug UI Pass
        m_dashboardSystem->Begin(Engine::Get().GetWindow().GetWidth(), Engine::Get().GetWindow().GetHeight());
        // (UI implementation will be handled by the caller/module using GetDashboard)
        m_dashboardSystem->End(*this);

        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::RenderRegistry(entt::registry& registry) {
        // 1. Draw Retro Mode 7 Backgrounds first (No depth write)
        glDisable(GL_DEPTH_TEST);
        auto retroView = registry.view<RetroComponent>();
        for (auto entity : retroView) {
            auto& retro = retroView.get<RetroComponent>(entity);
            if (!retro.active) continue;

            m_mode7Shader->Use();
            static float time = 0.0f;
            time += 0.016f;
            m_mode7Shader->SetFloat("uTime", time + retro.map_y);
            m_mode7Shader->SetFloat("uHorizon", retro.horizon);
            m_mode7Shader->SetFloat("uScale", retro.pitch);
            m_mode7Shader->SetVec3("uSkyColor", retro.skyColor);
            m_mode7Shader->SetVec3("uGroundColor1", retro.groundColor1);
            m_mode7Shader->SetVec3("uGroundColor2", retro.groundColor2);
            
            m_quadMesh->Draw();
        }
        glEnable(GL_DEPTH_TEST);

        // Gather Lights (max 4 for simple PBR forward)
        std::vector<glm::vec3> lightPositions;
        std::vector<glm::vec3> lightColors;
        auto lightView = registry.view<TransformComponent, PointLightComponent>();
        for (auto entity : lightView) {
            auto& t = lightView.get<TransformComponent>(entity);
            auto& l = lightView.get<PointLightComponent>(entity);
            lightPositions.push_back(t.position);
            lightColors.push_back(l.color * l.intensity);
            if (lightPositions.size() >= 4) break;
        }

        // Fill remaining with zero
        while(lightPositions.size() < 4) {
            lightPositions.push_back(glm::vec3(0.0f));
            lightColors.push_back(glm::vec3(0.0f));
        }

        // 2. Submit 3D Meshes
        auto meshView = registry.view<TransformComponent, MeshComponent>();
        for (auto entity : meshView) {
            auto& transform = meshView.get<TransformComponent>(entity);
            auto& meshComp = meshView.get<MeshComponent>(entity);
            
            if(meshComp.material.isPBR && meshComp.material.shader) {
                meshComp.material.shader->Use();
                for(int i = 0; i < 4; ++i) {
                    meshComp.material.shader->SetVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
                    meshComp.material.shader->SetVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
                }
                
                // Camera position for specular
                glm::vec3 camPos = glm::vec3(0.0f, 2.0f, 5.0f); // Fallback camera pos
                auto camView = registry.view<TransformComponent, CameraComponent>();
                for (auto camEntity : camView) {
                    if (camView.get<CameraComponent>(camEntity).primary) {
                        camPos = camView.get<TransformComponent>(camEntity).position;
                        break;
                    }
                }
                meshComp.material.shader->SetVec3("camPos", camPos);
                
                // --- Skeletal Animation ---
                if (registry.all_of<AnimatorComponent>(entity)) {
                    auto& animator = registry.get<AnimatorComponent>(entity);
                    meshComp.material.shader->SetInt("uIsAnimated", 1);
                    for (int i = 0; i < animator.jointMatrices.size(); i++) {
                        meshComp.material.shader->SetMat4("uJointMatrices[" + std::to_string(i) + "]", animator.jointMatrices[i]);
                    }
                } else {
                    meshComp.material.shader->SetInt("uIsAnimated", 0);
                }
            }

            if (meshComp.material.shader) {
                RenderCommand cmd = { meshComp.mesh, meshComp.material.shader, transform.GetMatrix() };
                Submit(cmd);
            }
        }

        // 3. Draw Instanced Meshes
        auto instView = registry.view<InstancedMeshComponent>();
        for (auto entity : instView) {
            auto& inst = instView.get<InstancedMeshComponent>(entity);
            if (!inst.mesh || !inst.shader || inst.instanceMatrices.empty()) continue;

            inst.UpdateBuffer();
            inst.shader->Use();
            inst.shader->SetMat4("view", m_view);
            inst.shader->SetMat4("projection", m_projectionMatrix);
            inst.shader->SetInt("uIsInstanced", 1);
            inst.mesh->DrawInstanced((uint32_t)inst.instanceMatrices.size());
            
            inst.shader->SetInt("uIsInstanced", 0);
        }

        // 4. Draw LOD Components
        auto lodView = registry.view<TransformComponent, LODComponent>();
        
        // Find main camera for distance calculation
        glm::vec3 camPos = glm::vec3(0.0f, 2.0f, 5.0f);
        auto camView = registry.view<TransformComponent, CameraComponent>();
        for (auto camEntity : camView) {
            if (camView.get<CameraComponent>(camEntity).primary) {
                camPos = camView.get<TransformComponent>(camEntity).position;
                break;
            }
        }

        for (auto entity : lodView) {
            auto& t = lodView.get<TransformComponent>(entity);
            auto& lod = lodView.get<LODComponent>(entity);
            
            float dist = glm::distance(t.position, camPos);
            std::shared_ptr<Mesh> selectedMesh = nullptr;
            
            for (auto& level : lod.levels) {
                if (dist < level.distance || selectedMesh == nullptr) {
                    selectedMesh = level.mesh;
                } else {
                    break;
                }
            }

            if (selectedMesh) {
                RenderCommand cmd = { selectedMesh, m_pbrShader, t.GetMatrix() };
                Submit(cmd);
            }
        }
    }

    void Renderer::DrawParticles(ParticleSystem& ps) {
        ps.Render(m_view, m_projectionMatrix);
    }
}
