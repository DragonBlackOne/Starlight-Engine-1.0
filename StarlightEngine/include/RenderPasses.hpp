#pragma once
#include "RenderGraph.hpp"
#include <cstdio>
#include "Renderer.hpp"
#include "ShadowSystem.hpp"
#include "Renderer2D.hpp"
#include "DashboardSystem.hpp"
#include "ScriptSystem.hpp"
#include "CoreMinimal.hpp"
#include "Components.hpp"
#include "Engine.hpp"
#include "imgui.h"
#include "PostProcessing.hpp"
#include "VFXSystem.hpp"
#include "CVarSystem.hpp"

namespace starlight {

    class GBufferPass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
            auto rendererPtr = blackboard.Get<Renderer*>("Renderer");
            if (!rendererPtr) return;
            Renderer* r = *rendererPtr;

            if (!r->m_gbufferShader) return;

            glBindFramebuffer(GL_FRAMEBUFFER, r->m_gBuffer.Get());
            glViewport(0, 0, r->m_fboWidth, r->m_fboHeight);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            r->m_gbufferShader->Use();
            r->m_gbufferShader->SetMat4U("projection", r->m_projectionMatrix);
            r->m_gbufferShader->SetMat4U("view", r->m_view);

            for (const auto& cmd : r->m_commandBuffer) {
                if (!cmd.mesh) continue;
                r->m_gbufferShader->SetMat4U("model", cmd.transform);
                r->m_gbufferShader->SetVec3U("albedoColor", cmd.albedo);
                r->m_gbufferShader->SetFloatU("metallic", cmd.metallic);
                r->m_gbufferShader->SetFloatU("roughness", cmd.roughness);
                cmd.mesh->Draw();
            }

            auto scenePtr = blackboard.Get<Scene*>("ActiveScene");
            if (scenePtr) {
                r->RenderSkinnedToGBuffer((*scenePtr)->GetRegistry());
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        const char* GetName() const override { return "GBufferPass"; }
    };

    class ShadowPass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
            auto rendererPtr = blackboard.Get<Renderer*>("Renderer");
            if (!rendererPtr) return;
            Renderer* r = *rendererPtr;
            
            auto scenePtr = blackboard.Get<Scene*>("ActiveScene");
            if (!scenePtr) return;
            Scene* scene = *scenePtr;

            if (r->m_shadowSystem) {
                glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f));
                r->m_shadowSystem->CalculateCascades(r->m_view, r->m_projectionMatrix, lightDir, 0.1f, 1000.0f);
                
                glEnable(GL_DEPTH_TEST);
                glCullFace(GL_FRONT);
                glEnable(GL_CULL_FACE);
                
                for (int i = 0; i < MAX_CASCADES; ++i) {
                    r->m_shadowSystem->BeginPass(i);
                    auto& registry = scene->GetRegistry();
                    auto shadowView = registry.view<TransformComponent, MeshComponent>();
                    for (auto entity : shadowView) {
                        auto& t = shadowView.get<TransformComponent>(entity);
                        auto& mc = shadowView.get<MeshComponent>(entity);
                        if (!mc.mesh) continue;
                        r->m_shadowSystem->GetShadowShader()->SetMat4U("model", t.GetMatrix());
                        mc.mesh->Draw();
                    }
                }
                glCullFace(GL_BACK);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        }
        const char* GetName() const override { return "ShadowPass"; }
    };

    class SkyboxPass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
            auto renderer = blackboard.Get<Renderer*>("Renderer");
            if (!renderer || !*renderer) return;
            Renderer* r = *renderer;

            if (r->m_skyboxShader && r->m_skyboxCubemap) {
                glDepthFunc(GL_LEQUAL);
                r->m_skyboxShader->Use();
                r->m_skyboxShader->SetMat4U("view", glm::mat4(glm::mat3(r->m_view)));
                r->m_skyboxShader->SetMat4U("projection", r->m_projectionMatrix);
                glBindTexture(GL_TEXTURE_CUBE_MAP, r->m_skyboxCubemap.Get());
                r->m_cubeMesh->Draw();
                glDepthFunc(GL_LESS);
            }
        }
        const char* GetName() const override { return "SkyboxPass"; }
    };

    class GeometryPass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
            auto rendererPtr = blackboard.Get<Renderer*>("Renderer");
            if (!rendererPtr) return;
            Renderer* r = *rendererPtr;

            auto cvarSys = Engine::Get().GetSystem<CVarSystem>();
            bool isDeferred = cvarSys ? cvarSys->GetBool("r_deferred") : false;
            if (isDeferred) return;

            if (!r->m_pbrShader) return;

            glBindFramebuffer(GL_FRAMEBUFFER, r->m_fbo.Get());
            glViewport(0, 0, r->m_fboWidth, r->m_fboHeight);
            
            r->m_pbrShader->Use();
            r->m_pbrShader->SetMat4U("projection", r->m_projectionMatrix);
            r->m_pbrShader->SetMat4U("view", r->m_view);
            r->m_pbrShader->SetVec3U("camPos", r->m_cameraTransform.position);

            if (r->m_shadowSystem) {
                r->m_shadowSystem->BindTextures(5);
                r->m_pbrShader->SetIntU("shadowMap", 5);
                r->m_pbrShader->SetVec3U("lightDir", glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f)));
                
                auto matrices = r->m_shadowSystem->GetLightSpaceMatrices();
                auto splits = r->m_shadowSystem->GetCascadeSplits();
                for (int i = 0; i < MAX_CASCADES; i++) {
                    r->m_pbrShader->SetMat4U("lightSpaceMatrices[" + std::to_string(i) + "]", matrices[i]);
                    r->m_pbrShader->SetFloatU("cascadePlaneDistances[" + std::to_string(i) + "]", splits[i]);
                }
            }

            // Bind IBL maps (slots 6, 7, 8)
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_CUBE_MAP, r->m_iblData.irradianceMap);
            r->m_pbrShader->SetIntU("irradianceMap", 6);

            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_CUBE_MAP, r->m_iblData.prefilterMap);
            r->m_pbrShader->SetIntU("prefilterMap", 7);

            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_2D, r->m_iblData.brdfLUT);
            r->m_pbrShader->SetIntU("brdfLUT", 8);

            auto scenePtr = blackboard.Get<Scene*>("ActiveScene");
            if (scenePtr) {
                auto& registry = (*scenePtr)->GetRegistry();
                auto lights = registry.view<TransformComponent, PointLightComponent>();
                int i = 0;
                for (auto entity : lights) {
                    if (i >= 8) break;
                    auto& t = lights.get<TransformComponent>(entity);
                    auto& l = lights.get<PointLightComponent>(entity);
                    std::string base = "lights[" + std::to_string(i) + "].";
                    r->m_pbrShader->SetVec3U(base + "position", t.position);
                    r->m_pbrShader->SetVec3U(base + "color", l.color);
                    r->m_pbrShader->SetFloatU(base + "intensity", l.intensity);
                    i++;
                }
                r->m_pbrShader->SetIntU("lightCount", i);
            }

            for (const auto& cmd : r->m_commandBuffer) {
                if (!cmd.mesh) continue;
                r->m_pbrShader->SetMat4U("model", cmd.transform);
                r->m_pbrShader->SetVec3U("albedo", cmd.albedo);
                r->m_pbrShader->SetFloatU("metallic", cmd.metallic);
                r->m_pbrShader->SetFloatU("roughness", cmd.roughness);
                r->m_pbrShader->SetFloatU("ao", cmd.ao);
                r->m_pbrShader->SetIntU("useAlbedoMap", 0);
                cmd.mesh->Draw();
            }

            if (scenePtr) {
                r->RenderSkinnedMeshes((*scenePtr)->GetRegistry());
            }
        }
        const char* GetName() const override { return "GeometryPass"; }
    };

    class DeferredLightingPass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
            auto rendererPtr = blackboard.Get<Renderer*>("Renderer");
            if (!rendererPtr) return;
            Renderer* r = *rendererPtr;

            auto cvarSys = Engine::Get().GetSystem<CVarSystem>();
            bool isDeferred = cvarSys ? cvarSys->GetBool("r_deferred") : false;
            if (!isDeferred) return;

            if (!r->m_deferredLightShader) return;

            glBindFramebuffer(GL_READ_FRAMEBUFFER, r->m_gBuffer.Get());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, r->m_fbo.Get());
            glBlitFramebuffer(0, 0, r->m_fboWidth, r->m_fboHeight, 0, 0, r->m_fboWidth, r->m_fboHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

            glBindFramebuffer(GL_FRAMEBUFFER, r->m_fbo.Get());
            glViewport(0, 0, r->m_fboWidth, r->m_fboHeight);
            glClear(GL_COLOR_BUFFER_BIT);

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);

            r->m_deferredLightShader->Use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, r->m_gPosition.Get());
            r->m_deferredLightShader->SetIntU("gPosition", 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, r->m_gNormal.Get());
            r->m_deferredLightShader->SetIntU("gNormal", 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, r->m_gAlbedoSpec.Get());
            r->m_deferredLightShader->SetIntU("gAlbedoSpec", 2);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, r->m_gRoughnessAO.Get());
            r->m_deferredLightShader->SetIntU("gRoughnessAO", 3);

            r->m_deferredLightShader->SetVec3U("viewPos", r->m_cameraTransform.position);

            auto scenePtr = blackboard.Get<Scene*>("ActiveScene");
            int lightCount = 0;
            if (scenePtr) {
                auto& registry = (*scenePtr)->GetRegistry();
                auto lights = registry.view<TransformComponent, PointLightComponent>();
                for (auto entity : lights) {
                    if (lightCount >= 32) break;
                    auto& t = lights.get<TransformComponent>(entity);
                    auto& l = lights.get<PointLightComponent>(entity);
                    std::string base = "lights[" + std::to_string(lightCount) + "].";
                    r->m_deferredLightShader->SetVec3U(base + "Position", t.position);
                    r->m_deferredLightShader->SetVec3U(base + "Color", l.color * l.intensity);
                    r->m_deferredLightShader->SetFloatU(base + "Linear", 0.09f);
                    r->m_deferredLightShader->SetFloatU(base + "Quadratic", 0.032f);
                    lightCount++;
                }
            }
            for (int i = lightCount; i < 32; ++i) {
                std::string base = "lights[" + std::to_string(i) + "].";
                r->m_deferredLightShader->SetVec3U(base + "Position", glm::vec3(0.0f));
                r->m_deferredLightShader->SetVec3U(base + "Color", glm::vec3(0.0f));
                r->m_deferredLightShader->SetFloatU(base + "Linear", 0.0f);
                r->m_deferredLightShader->SetFloatU(base + "Quadratic", 0.0f);
            }

            r->m_quadMesh->Draw();

            glEnable(GL_DEPTH_TEST);
        }
        const char* GetName() const override { return "DeferredLightingPass"; }
    };

    class SSAO_Pass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
            auto rendererPtr = blackboard.Get<Renderer*>("Renderer");
            if (!rendererPtr) return;
            Renderer* r = *rendererPtr;

            if (r->IsSSAOEnabled() && r->m_ssaoSystem) {
                r->m_ssaoSystem->Render(r->m_gPosition.Get(), r->m_gNormal.Get(), r->m_projectionMatrix, r->m_quadMesh);
            }
        }
        const char* GetName() const override { return "SSAO_Pass"; }
    };

    class SSRPass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
            auto rendererPtr = blackboard.Get<Renderer*>("Renderer");
            if (!rendererPtr) return;
            Renderer* r = *rendererPtr;

            if (r->m_ssrShader) {
                // We'll render SSR to a temporary buffer or use additive blending
                // For now, let's just setup the shader and draw a quad
                glBindFramebuffer(GL_FRAMEBUFFER, r->m_fbo.Get()); // Draw back to scene FBO
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);

                r->m_ssrShader->Use();
                glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, r->m_gPosition.Get()); r->m_ssrShader->SetIntU("gPosition", 0);
                glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, r->m_gNormal.Get()); r->m_ssrShader->SetIntU("gNormal", 1);
                glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, r->m_gAlbedoSpec.Get()); r->m_ssrShader->SetIntU("gAlbedoSpec", 2);
                glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, r->m_fboTexture.Get()); r->m_ssrShader->SetIntU("sceneTexture", 3);

                r->m_ssrShader->SetMat4U("projection", r->m_projectionMatrix);
                r->m_ssrShader->SetMat4U("view", r->m_view);
                r->m_ssrShader->SetVec3U("camPos", r->m_cameraTransform.position);

                r->m_quadMesh->Draw();
                glDisable(GL_BLEND);
            }
        }
        const char* GetName() const override { return "SSRPass"; }
    };

    class VolumetricPass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
            auto rendererPtr = blackboard.Get<Renderer*>("Renderer");
            if (!rendererPtr) return;
            Renderer* r = *rendererPtr;
    
            if (r->m_volumetricShader && !r->m_lastLightPositions.empty()) {
                r->m_volumetricShader->Use();
                
                // For now, take the first light (usually the sun/main directional)
                glm::vec3 lightPos = r->m_lastLightPositions[0];
                glm::vec4 clipPos = r->m_projectionMatrix * r->m_view * glm::vec4(lightPos, 1.0f);
                
                // Only render if the light is in front of the camera
                if (clipPos.w > 0.0f) {
                    glm::vec2 screenPos = glm::vec2(clipPos.x / clipPos.w, clipPos.y / clipPos.w);
                    screenPos = screenPos * 0.5f + 0.5f;
    
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_ONE, GL_ONE);
    
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, r->m_fboTexture.Get());
                    r->m_volumetricShader->SetIntU("sceneTexture", 0);
                    r->m_volumetricShader->SetVec2U("lightScreenPos", screenPos);
    
                    r->m_quadMesh->Draw();
                    glDisable(GL_BLEND);
                }
            }
        }
        const char* GetName() const override { return "VolumetricPass"; }
    };

    class BloomPass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
            auto rendererPtr = blackboard.Get<Renderer*>("Renderer");
            if (!rendererPtr) return;
            Renderer* r = *rendererPtr;

            PostProcessing::RenderBloom(r->m_fboTexture.Get(), r->m_fboWidth, r->m_fboHeight);
        }
        const char* GetName() const override { return "BloomPass"; }
    };

    class CompositionPass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
            auto renderer = blackboard.Get<Renderer*>("Renderer");
            if (!renderer || !*renderer) return;
            Renderer* r = *renderer;

            if (!r->m_screenShader) return;

            GLuint targetFBO = r->m_viewportFBO ? r->m_viewportFBO : 0;
            int vpW = r->m_viewportFBO ? r->m_viewportWidth : Engine::Get().GetWindow().GetWidth();
            int vpH = r->m_viewportFBO ? r->m_viewportHeight : Engine::Get().GetWindow().GetHeight();

            glBindFramebuffer(GL_FRAMEBUFFER, targetFBO);
            glViewport(0, 0, vpW, vpH);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            PostProcessing::RenderFinalComposition(r->m_fboTexture.Get(), PostProcessing::GetBloomTexture(), r->m_exposure, r->m_gamma, targetFBO, vpW, vpH);
        }
        const char* GetName() const override { return "CompositionPass"; }
    };

    class VFXPass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)blackboard;
            (void)resources;
            auto vfx = Engine::Get().GetSystem<VFXSystem>();
            if (vfx) {
                vfx->RenderInternal();
            }
        }
        const char* GetName() const override { return "VFXPass"; }
    };

    class UIPass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
            auto rendererPtr = blackboard.Get<Renderer*>("Renderer");
            if (!rendererPtr) return;
            Renderer* r = *rendererPtr;

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // 1. Script UI (Lua)
            auto scripting = Engine::Get().GetSystem<ScriptSystem>();
            if (scripting) {
                scripting->OnUIRender(); // Executes Lua OnRenderUI
            }

            // 2. Dashboard UI (Native)
            auto dash = Engine::Get().GetSystem<DashboardSystem>();
            if (dash) {
                dash->End(*r);
            }

            // 3. Ensure everything is flushed
            Renderer2D::Flush();
        }
        const char* GetName() const override { return "UIPass"; }
    };

}
