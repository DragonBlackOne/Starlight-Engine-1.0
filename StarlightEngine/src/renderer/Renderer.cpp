// Este projeto é feito por IA e só o prompt é feito por um humano.
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
#include "JobSystem.hpp"

namespace starlight {

    Renderer::Renderer() {}
    Renderer::~Renderer() {
        if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
        if (m_fboTexture) glDeleteTextures(1, &m_fboTexture);
        if (m_depthTexture) glDeleteTextures(1, &m_depthTexture);
        if (m_rbo) glDeleteRenderbuffers(1, &m_rbo);
        if (m_finalFBO) glDeleteFramebuffers(1, &m_finalFBO);
        if (m_finalColorTex) glDeleteTextures(1, &m_finalColorTex);
        
        glDeleteFramebuffers(2, m_pingpongFBO);
        glDeleteTextures(2, m_pingpongColorbuffers);
    }

    void Renderer::Initialize() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Compile Shaders from files
        m_basicShader = Shader::LoadFromFile("assets/shaders/basic.vert", "assets/shaders/basic.frag");
        m_mode7Shader = Shader::LoadFromFile("assets/shaders/mode7.vert", "assets/shaders/mode7.frag");
        m_crtShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/crt.frag");
        
        m_crtShader->Use();
        m_crtShader->SetInt("screenTexture", 0);

        m_pbrShader = Shader::LoadFromFile("assets/shaders/pbr.vert", "assets/shaders/pbr.frag");
        m_uiShader = Shader::LoadFromFile("assets/shaders/ui.vert", "assets/shaders/ui.frag");

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
        m_dashboardSystem->Initialize();
        m_gizmoSystem = std::make_unique<GizmoSystem>();
        m_gizmoSystem->Initialize();
        
        AABB worldBounds = { glm::vec3(-50, -10, -50), glm::vec3(50, 50, 50) };
        m_octreeSystem = std::make_unique<OctreeSystem>(worldBounds);

        // Phase 10: Shaders & FBOs
        m_bloomBrightShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/bloom_bright.frag");
        m_bloomBlurShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/bloom_blur.frag");
        m_postComposeShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/post_compose.frag");

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

        m_renderGraph = std::make_unique<RenderGraph>();
        // Futura conversÃ£o completa: Adicionar passes aqui
        // auto mainPass = std::make_shared<RenderPass>("MainPass");
        // m_renderGraph->AddPass(mainPass);
        m_renderGraph->Compile();
    }

    void Renderer::RecreateFBO(int width, int height) {
        m_fboWidth = width;
        m_fboHeight = height;
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
            
        // Final FBO for ImGui Viewport
        if (m_finalFBO) {
            glDeleteFramebuffers(1, &m_finalFBO);
            glDeleteTextures(1, &m_finalColorTex);
        }
        glGenFramebuffers(1, &m_finalFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_finalFBO);
        glGenTextures(1, &m_finalColorTex);
        glBindTexture(GL_TEXTURE_2D, m_finalColorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_finalColorTex, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::UpdateProjection(float fov, float aspect, float nearP, float farP) {
        m_projectionMatrix = glm::perspective(glm::radians(fov), aspect, nearP, farP);
    }

    void Renderer::BeginFrame() {
        glViewport(0, 0, m_fboWidth, m_fboHeight);
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
        unsigned int amount = m_bloomBlurSteps;
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
        glBindFramebuffer(GL_FRAMEBUFFER, m_finalFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_postComposeShader->Use();
        
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_fboTexture);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, m_pingpongColorbuffers[!horizontal]);
        glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, m_depthTexture);
        glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, m_ssaoSystem->GetSSAOTexture());

        m_postComposeShader->SetInt("screenTexture", 0);
        m_postComposeShader->SetInt("bloomBlur", 1);
        m_postComposeShader->SetInt("depthTexture", 2);
        m_postComposeShader->SetInt("ssaoTexture", 3);
        
        m_postComposeShader->SetFloat("u_exposure", m_exposure);
        m_postComposeShader->SetFloat("u_gamma", m_gamma);
        
        // Matrices for SSR
        m_postComposeShader->SetMat4("u_view", m_view);
        m_postComposeShader->SetMat4("u_proj", m_projectionMatrix);
        m_postComposeShader->SetMat4("u_inv_view", glm::inverse(m_view));
        m_postComposeShader->SetMat4("u_inv_proj", glm::inverse(m_projectionMatrix));
        
        m_quadMesh->Draw();

        // 5. Dashboard / Debug UI Pass
        // (UI implementation was handled by the caller/module in Update)
        m_dashboardSystem->End(*this);

        glEnable(GL_DEPTH_TEST);
        
        // Clear backbuffer so ImGui can draw over it cleanly
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Renderer::RenderRegistry(entt::registry& registry) {
        glm::vec3 activeCamPos = glm::vec3(0.0f, 2.0f, 5.0f);

        // Fetch cached View and Projection Matrices from CameraSystem
        auto mainCamView = registry.view<TransformComponent, CameraComponent>();
        for (auto camEntity : mainCamView) {
            auto& cam = mainCamView.get<CameraComponent>(camEntity);
            if (cam.primary) {
                m_view = cam.view;
                m_projectionMatrix = cam.projection;
                activeCamPos = mainCamView.get<TransformComponent>(camEntity).position;
                break;
            }
        }

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

        // 2. Multithreaded Frustum Culling & Submission Prep
        glm::mat4 vp = m_projectionMatrix * m_view;
        glm::mat4 tvp = glm::transpose(vp);
        glm::vec4 planes[6];
        planes[0] = tvp[3] + tvp[0]; planes[1] = tvp[3] - tvp[0]; // L, R
        planes[2] = tvp[3] + tvp[1]; planes[3] = tvp[3] - tvp[1]; // B, T
        planes[4] = tvp[3] + tvp[2]; planes[5] = tvp[3] - tvp[2]; // N, F
        for (int i = 0; i < 6; i++) planes[i] = planes[i] / glm::length(glm::vec3(planes[i]));

        auto meshView = registry.view<TransformComponent, MeshComponent>();
        std::vector<entt::entity> cullingList(meshView.begin(), meshView.end());

        if (!cullingList.empty()) {
            JobContext cullCtx;
            JobSystem::Dispatch(cullCtx, (uint32_t)cullingList.size(), 256, [&](uint32_t i) {
                auto entity = cullingList[i];
                auto& transform = registry.get<TransformComponent>(entity);
                auto& meshComp = registry.get<MeshComponent>(entity);

                float maxScale = std::max({transform.scale.x, transform.scale.y, transform.scale.z});
                float radius = meshComp.boundingRadius * maxScale;
                
                meshComp.isVisible = true;
                for (int p = 0; p < 6; ++p) {
                    float dist = glm::dot(glm::vec3(planes[p]), transform.position) + planes[p].w;
                    if (dist < -radius) {
                        meshComp.isVisible = false;
                        break;
                    }
                }
            });
            JobSystem::Wait(cullCtx);
        }

        // 3. Submit 3D Meshes
        for (auto entity : meshView) {
            auto& transform = meshView.get<TransformComponent>(entity);
            auto& meshComp = meshView.get<MeshComponent>(entity);
            
            if (!meshComp.isVisible) continue;
            
            if(meshComp.material.isPBR && meshComp.material.shader) {
                meshComp.material.shader->Use();
                for(int i = 0; i < 4; ++i) {
                    meshComp.material.shader->SetVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
                    meshComp.material.shader->SetVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
                }
                
                // Camera position for specular
                meshComp.material.shader->SetVec3("camPos", activeCamPos);

                // PBR Material uniforms (CRITICAL - shader needs these!)
                glm::vec3 albedo = meshComp.material.albedo;
                if (albedo == glm::vec3(0.0f)) albedo = meshComp.material.color; // fallback to color
                meshComp.material.shader->SetVec3("albedo", albedo);
                meshComp.material.shader->SetFloat("metallic", meshComp.material.metallic);
                meshComp.material.shader->SetFloat("roughness", meshComp.material.roughness);
                meshComp.material.shader->SetFloat("ao", meshComp.material.ao > 0.0f ? meshComp.material.ao : 1.0f);
                
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
        
        for (auto entity : lodView) {
            auto& t = lodView.get<TransformComponent>(entity);
            auto& lod = lodView.get<LODComponent>(entity);
            
            if (lod.levels.empty()) continue;

            std::shared_ptr<Mesh> selectedMesh = lod.levels[lod.currentLevel].mesh;

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
