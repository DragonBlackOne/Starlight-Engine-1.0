// Starlight Engine: ULTRA-PREMIUM RENDERER
#include "Renderer.hpp"
#include "Log.hpp"
#include <glad/glad.h>
#include "Engine.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "RenderPasses.hpp"
#include "AnimationComponent.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace starlight {

    static std::shared_ptr<Mesh> InternalCreateCube() {
        std::vector<Vertex> vertices = {
            {{-1, -1,  1}, {0, 0, 1}, {0, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1, -1,  1}, {0, 0, 1}, {1, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1,  1,  1}, {0, 0, 1}, {1, 1}, {0,0,0,0}, {0,0,0,0}}, {{-1,  1,  1}, {0, 0, 1}, {0, 1}, {0,0,0,0}, {0,0,0,0}},
            {{-1, -1, -1}, {0, 0, -1}, {0, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1, -1, -1}, {0, 0, -1}, {1, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1,  1, -1}, {0, 0, -1}, {1, 1}, {0,0,0,0}, {0,0,0,0}}, {{-1,  1, -1}, {0, 0, -1}, {0, 1}, {0,0,0,0}, {0,0,0,0}},
            {{-1,  1,  1}, {0, 1, 0}, {0, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1,  1,  1}, {0, 1, 0}, {1, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1,  1, -1}, {0, 1, 0}, {1, 1}, {0,0,0,0}, {0,0,0,0}}, {{-1,  1, -1}, {0, 1, 0}, {0, 1}, {0,0,0,0}, {0,0,0,0}},
            {{-1, -1,  1}, {0, -1, 0}, {0, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1, -1,  1}, {0, -1, 0}, {1, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1, -1, -1}, {0, -1, 0}, {1, 1}, {0,0,0,0}, {0,0,0,0}}, {{-1, -1, -1}, {0, -1, 0}, {0, 1}, {0,0,0,0}, {0,0,0,0}},
            {{ 1, -1,  1}, {1, 0, 0}, {0, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1, -1, -1}, {1, 0, 0}, {1, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1,  1, -1}, {1, 0, 0}, {1, 1}, {0,0,0,0}, {0,0,0,0}}, {{ 1,  1,  1}, {1, 0, 0}, {0, 1}, {0,0,0,0}, {0,0,0,0}},
            {{-1, -1,  1}, {-1, 0, 0}, {0, 0}, {0,0,0,0}, {0,0,0,0}}, {{-1, -1, -1}, {-1, 0, 0}, {1, 0}, {0,0,0,0}, {0,0,0,0}}, {{-1,  1, -1}, {-1, 0, 0}, {1, 1}, {0,0,0,0}, {0,0,0,0}}, {{-1,  1,  1}, {-1, 0, 0}, {0, 1}, {0,0,0,0}, {0,0,0,0}}
        };
        std::vector<uint32_t> indices = { 0,1,2,2,3,0, 4,5,6,6,7,4, 8,9,10,10,11,8, 12,13,14,14,15,12, 16,17,18,18,19,16, 20,21,22,22,23,20 };
        return std::make_shared<Mesh>(vertices, indices);
    }

    static std::shared_ptr<Mesh> CreateQuad() {
        std::vector<Vertex> vertices = {
            {{-1,  1, 0}, {0,0,1}, {0,1}, {0,0,0,0}, {0,0,0,0}},
            {{-1, -1, 0}, {0,0,1}, {0,0}, {0,0,0,0}, {0,0,0,0}},
            {{ 1, -1, 0}, {0,0,1}, {1,0}, {0,0,0,0}, {0,0,0,0}},
            {{ 1,  1, 0}, {0,0,1}, {1,1}, {0,0,0,0}, {0,0,0,0}}
        };
        std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };
        return std::make_shared<Mesh>(vertices, indices);
    }

    std::vector<uint32_t> Renderer::s_textureDeletionQueue;
    std::vector<Renderer::MeshDeletion> Renderer::s_meshDeletionQueue;
    std::mutex Renderer::s_deletionMutex;

    void Renderer::SubmitDeferredTextureDeletion(uint32_t texID) {
        std::lock_guard<std::mutex> lock(s_deletionMutex);
        s_textureDeletionQueue.push_back(texID);
    }

    void Renderer::SubmitDeferredMeshDeletion(uint32_t vao, uint32_t vbo, uint32_t ebo) {
        std::lock_guard<std::mutex> lock(s_deletionMutex);
        s_meshDeletionQueue.push_back({vao, vbo, ebo});
    }

    Renderer::Renderer() {
        Log::Info("Renderer v5.0: Object Created.");
    }

    Renderer::~Renderer() {
        OnShutdown();
    }

    bool Renderer::OnInitialize(const EngineContext& context) {
        (void)context;
        
        // 1. Core Meshes
        m_cubeMesh = InternalCreateCube();
        m_quadMesh = CreateQuad();

        // 2. Load Industrial Shaders (v5.0 Nucleus)
        m_gbufferShader = Shader::LoadFromFile("assets/shaders/gbuffer.vert", "assets/shaders/gbuffer.frag");
        m_pbrShader = Shader::LoadFromFile("assets/shaders/pbr.vert", "assets/shaders/pbr.frag");
        m_uiShader = Shader::LoadFromFile("assets/shaders/ui.vert", "assets/shaders/ui.frag");
        m_screenShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/composition.frag");
        m_skyboxShader = Shader::LoadFromFile("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
        
        // Post-FX Shaders
        m_bloomBrightShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/bloom_bright.frag");
        m_bloomBlurShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/bloom_blur.frag");
        m_ssrShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/ssr.frag");
        m_postComposeShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/post_compose.frag");

        // 3. Systems Initialization
        m_shadowSystem = std::make_unique<ShadowSystem>(2048);
        m_shadowSystem->Initialize();
        
        m_ssaoSystem = std::make_unique<SSAO_System>();
        m_ssaoSystem->Initialize();

        PostProcessing::Initialize();

        m_dashboardSystem = std::make_unique<DashboardSystem>();

        m_renderGraph = std::make_unique<RenderGraph>();
        m_renderGraph->AddPass<GBufferPass>();
        m_renderGraph->AddPass<ShadowPass>();
        m_renderGraph->AddPass<SkyboxPass>();
        m_renderGraph->AddPass<SSAO_Pass>();
        m_renderGraph->AddPass<GeometryPass>();
        m_renderGraph->AddPass<VFXPass>();
        m_renderGraph->AddPass<BloomPass>();
        m_renderGraph->AddPass<CompositionPass>();
        m_renderGraph->AddPass<UIPass>();

        RecreateFBO(1600, 900);
        
        Log::Info("Renderer v5.0: Industrial Pipeline Initialized.");
        return true;
    }

    void Renderer::OnShutdown() {
        if (m_fbo) { glDeleteFramebuffers(1, &m_fbo); m_fbo = 0; }
        if (m_fboTexture) { glDeleteTextures(1, &m_fboTexture); m_fboTexture = 0; }
        if (m_fboDepth) { glDeleteTextures(1, &m_fboDepth); m_fboDepth = 0; }
        if (m_rbo) { glDeleteRenderbuffers(1, &m_rbo); m_rbo = 0; }
        
        m_renderGraph.reset();
        m_shadowSystem.reset();
        m_ssaoSystem.reset();
        m_dashboardSystem.reset();
        
        m_gbufferShader.reset();
        m_pbrShader.reset();
        m_uiShader.reset();
        m_screenShader.reset();
        m_skyboxShader.reset();

        PostProcessing::Shutdown();
    }

    void Renderer::OnRender() {
        // static int frameCount = 0;
        // if (frameCount++ % 100 == 0) Log::Info("Renderer: OnRender active");
        
        BeginFrame();
        
        auto activeScene = Engine::Get().GetSceneStack().Active();
        
        if (activeScene) {
            RenderRegistry(activeScene->GetRegistry());
        }

        // --- RenderGraph Execution (Phase 12 Unified) ---
        auto& bb = m_renderGraph->GetBlackboard();
        bb.Put("Renderer", this);
        if (activeScene) {
            bb.Put("ActiveScene", activeScene.get());
        }

        auto script = Engine::Get().GetSystem<ScriptSystem>();
        if (script) script->OnRender();
        
        m_renderGraph->Execute();
        
        // Restore state
        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::RecreateFBO(int width, int height) {
        m_fboWidth = width; m_fboHeight = height;
        if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
        if (m_fboTexture) glDeleteTextures(1, &m_fboTexture);
        if (m_rbo) glDeleteRenderbuffers(1, &m_rbo);

        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glGenTextures(1, &m_fboTexture);
        glBindTexture(GL_TEXTURE_2D, m_fboTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture, 0);
        glGenRenderbuffers(1, &m_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
        
        // --- G-Buffer Initialization ---
        if (m_gBuffer) glDeleteFramebuffers(1, &m_gBuffer);
        glGenFramebuffers(1, &m_gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

        auto createGBufferTex = [&](uint32_t& id, GLint internalFormat, GLenum format, GLenum type) {
            if (id) glDeleteTextures(1, &id);
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (&id == &m_gPosition ? 0 : (&id == &m_gNormal ? 1 : (&id == &m_gAlbedoSpec ? 2 : 3))), GL_TEXTURE_2D, id, 0);
        };

        createGBufferTex(m_gPosition, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        createGBufferTex(m_gNormal, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        createGBufferTex(m_gAlbedoSpec, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
        createGBufferTex(m_gRoughnessAO, GL_RG8, GL_RG, GL_UNSIGNED_BYTE);

        uint32_t attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(4, attachments);

        // Depth attachment for G-Buffer (shared with main FBO if needed, or separate)
        uint32_t gDepth;
        glGenRenderbuffers(1, &gDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Log::Error("Renderer: G-Buffer FBO Incomplete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (m_renderGraph) {
            m_renderGraph->Reset();
            m_renderGraph->ImportResource("SceneColor", m_fboTexture, m_fbo);
            m_renderGraph->Compile();
        }
    }

    void Renderer::BeginFrame() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glClearColor(0.02f, 0.02f, 0.04f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        m_commandBuffer.clear();
        m_cameraTransform.UpdateLocalMatrix();
        m_cameraTransform.worldMatrix = m_cameraTransform.localMatrix; 
        m_view = glm::inverse(m_cameraTransform.GetMatrix());
    }

    void Renderer::EndFrame() {
        // Redundant draw calls removed. Logic moved to RenderGraph Passes.
    }

    void Renderer::RenderRegistry(entt::registry& registry) {
        // Data population only. Rendering is handled by the RenderGraph.
        m_commandBuffer.clear();
        m_lastLightPositions.clear();

        // 1. Submit Meshes using cache-friendly group
        auto group = registry.group<MeshComponent>(entt::get<TransformComponent>);
        group.each([this](const auto& mc, const auto& t) {
            if (!mc.mesh) return;

            RenderCommand cmd;
            cmd.mesh = mc.mesh;
            cmd.transform = t.GetMatrix();
            
            // Access material from MeshComponent
            cmd.albedo = mc.material.albedo;
            cmd.metallic = mc.material.metallic;
            cmd.roughness = mc.material.roughness;
            cmd.ao = mc.material.ao;

            m_commandBuffer.push_back(cmd);
        });

        // 2. Track light positions for Volumetric effects
        auto lights = registry.view<TransformComponent, PointLightComponent>();
        for (auto entity : lights) {
            auto& t = lights.get<TransformComponent>(entity);
            m_lastLightPositions.push_back(t.position);
            if (m_lastLightPositions.size() >= 8) break;
        }
    }

    void Renderer::Submit(const RenderCommand& command) { m_commandBuffer.push_back(command); }
    void Renderer::UpdateProjection(float fov, float aspect, float nearPlane, float farPlane) {
        m_projectionMatrix = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }

    void Renderer::SetCameraLookAt(glm::vec3 target) {
        m_view = glm::lookAt(m_cameraTransform.position, target, glm::vec3(0, 1, 0));
        // Also update camera rotation to match (optional but good for consistency)
        // m_cameraTransform.rotation = glm::quatLookAt(glm::normalize(target - m_cameraTransform.position), glm::vec3(0, 1, 0));
    }

    void Renderer::RenderSkinnedMeshes(entt::registry& registry) {
        auto view = registry.view<TransformComponent, MeshComponent, SkeletalAnimationComponent>();
        view.each([this](const auto& transform, const auto& mc, const auto& anim) {
            if (!mc.mesh || !anim.playing) return;

            m_pbrShader->Use();
            m_pbrShader->SetMat4U("model", transform.GetMatrix());
            m_pbrShader->SetVec3U("albedo", mc.material.albedo);
            m_pbrShader->SetIntU("has_anim", 1);

            for (size_t i = 0; i < anim.boneMatrices.size() && i < 100; i++) {
                char nameBuf[64];
                sprintf(nameBuf, "bone_matrices[%d]", (int)i);
                m_pbrShader->SetMat4U(nameBuf, anim.boneMatrices[i]);
            }

            mc.mesh->Draw();
        });
        m_pbrShader->SetIntU("has_anim", 0);
    }

    void Renderer::RenderSkinnedToGBuffer(entt::registry& registry) {
        auto view = registry.view<TransformComponent, MeshComponent, SkeletalAnimationComponent>();
        view.each([this](const auto& transform, const auto& mc, const auto& anim) {
            if (!mc.mesh || !anim.playing) return;

            m_gbufferShader->Use();
            m_gbufferShader->SetMat4U("model", transform.GetMatrix());
            m_gbufferShader->SetVec3U("albedoColor", mc.material.albedo);
            m_gbufferShader->SetIntU("has_anim", 1);

            for (size_t i = 0; i < anim.boneMatrices.size() && i < 100; i++) {
                char nameBuf[64];
                sprintf(nameBuf, "bone_matrices[%d]", (int)i);
                m_gbufferShader->SetMat4U(nameBuf, anim.boneMatrices[i]);
            }

            mc.mesh->Draw();
        });
        m_gbufferShader->SetIntU("has_anim", 0);
    }
}
