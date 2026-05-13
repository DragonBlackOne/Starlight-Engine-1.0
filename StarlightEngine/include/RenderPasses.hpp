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

            glBindFramebuffer(GL_FRAMEBUFFER, r->m_gBuffer);
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

            if (r->m_skyboxShader) {
                glDepthFunc(GL_LEQUAL);
                r->m_skyboxShader->Use();
                r->m_skyboxShader->SetMat4U("view", glm::mat4(glm::mat3(r->m_view)));
                r->m_skyboxShader->SetMat4U("projection", r->m_projectionMatrix);
                glBindTexture(GL_TEXTURE_CUBE_MAP, r->m_skyboxCubemap);
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

            if (!r->m_pbrShader) return;

            glBindFramebuffer(GL_FRAMEBUFFER, r->m_fbo);
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

    class SSAO_Pass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
            auto rendererPtr = blackboard.Get<Renderer*>("Renderer");
            if (!rendererPtr) return;
            Renderer* r = *rendererPtr;

            if (r->m_ssaoSystem) {
                r->m_ssaoSystem->Render(r->m_gPosition, r->m_gNormal, r->m_projectionMatrix, r->m_quadMesh);
            }
        }
        const char* GetName() const override { return "SSAO_Pass"; }
    };

    class VolumetricPass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
            auto renderer = blackboard.Get<Renderer*>("Renderer");
            if (!renderer || !*renderer) return;
            Renderer* r = *renderer;

            if (r->m_volumetricShader && !r->m_lastLightPositions.empty()) {
                // Simplified execution
            }
        }
        const char* GetName() const override { return "VolumetricPass"; }
    };

    class BloomPass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
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

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            r->m_screenShader->Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, r->m_fboTexture);
            r->m_screenShader->SetIntU("sceneTexture", 0);
            
            r->m_screenShader->SetFloatU("exposure", r->m_exposure);
            r->m_screenShader->SetFloatU("gamma", r->m_gamma);
            r->m_quadMesh->Draw();
        }
        const char* GetName() const override { return "CompositionPass"; }
    };

    class UIPass : public RenderGraphPass {
    public:
        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override {
            (void)resources;
            auto rendererPtr = blackboard.Get<Renderer*>("Renderer");
            if (!rendererPtr) return;
            Renderer* r = *rendererPtr;

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
