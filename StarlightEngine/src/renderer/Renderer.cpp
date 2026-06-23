// Starlight Engine: ULTRA-PREMIUM RENDERER
#include "Renderer.hpp"
#include <algorithm>
#include <filesystem>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "AnimationComponent.hpp"
#include "Engine.hpp"
#include "Log.hpp"
#include "RenderPasses.hpp"
#include "VFXSystem.hpp"
#include "TAAPass.hpp"
#include "PostProcessing.hpp"
#include "CVarSystem.hpp"

namespace starlight {

static std::shared_ptr<Mesh> InternalCreateCube() {
    std::vector<Vertex> vertices = {{{-1, -1, 1}, {0, 0, 1}, {0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, -1, 1}, {0, 0, 1}, {1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 1}, {0, 0, 1}, {1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{-1, 1, 1}, {0, 0, 1}, {0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{-1, -1, -1}, {0, 0, -1}, {0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, -1, -1}, {0, 0, -1}, {1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, -1}, {0, 0, -1}, {1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{-1, 1, -1}, {0, 0, -1}, {0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{-1, 1, 1}, {0, 1, 0}, {0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 1}, {0, 1, 0}, {1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, -1}, {0, 1, 0}, {1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{-1, 1, -1}, {0, 1, 0}, {0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{-1, -1, 1}, {0, -1, 0}, {0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, -1, 1}, {0, -1, 0}, {1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, -1, -1}, {0, -1, 0}, {1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{-1, -1, -1}, {0, -1, 0}, {0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, -1, 1}, {1, 0, 0}, {0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, -1, -1}, {1, 0, 0}, {1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, -1}, {1, 0, 0}, {1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 1}, {1, 0, 0}, {0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{-1, -1, 1}, {-1, 0, 0}, {0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{-1, -1, -1}, {-1, 0, 0}, {1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{-1, 1, -1}, {-1, 0, 0}, {1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{-1, 1, 1}, {-1, 0, 0}, {0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}};
    std::vector<uint32_t> indices = {0,
        1,
        2,
        2,
        3,
        0,
        4,
        5,
        6,
        6,
        7,
        4,
        8,
        9,
        10,
        10,
        11,
        8,
        12,
        13,
        14,
        14,
        15,
        12,
        16,
        17,
        18,
        18,
        19,
        16,
        20,
        21,
        22,
        22,
        23,
        20};
    return std::make_shared<Mesh>(vertices, indices);
}

static std::shared_ptr<Mesh> CreateQuad() {
    std::vector<Vertex> vertices = {{{-1, 1, 0}, {0, 0, 1}, {0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{-1, -1, 0}, {0, 0, 1}, {0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, -1, 0}, {0, 0, 1}, {1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0}, {0, 0, 1}, {1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}};
    std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
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
    Log::Info("Renderer v7.0: Object Created.");
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

    // Volumetric shader (loaded but only used if light positions exist)
    m_volumetricShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/volumetric.frag");
    m_deferredLightShader = Shader::LoadFromFile("assets/shaders/screen.vert", "assets/shaders/deferred_light.frag");

    // 3. Systems Initialization
    m_shadowSystem = std::make_unique<ShadowSystem>(m_shadowResolution);
    m_shadowSystem->Initialize();

    m_ssaoSystem = std::make_unique<SSAO_System>();
    m_ssaoSystem->Initialize();

    m_iblSystem = std::make_unique<IBLSystem>();
    if (std::filesystem::exists("assets/textures/default_env.hdr")) {
        m_iblData = m_iblSystem->ProcessHDR("assets/textures/default_env.hdr");
    }

    PostProcessing::Initialize();

    m_dashboardSystem = std::make_unique<DashboardSystem>();

    m_renderGraph = std::make_unique<RenderGraph>();
    m_renderGraph->AddPass<GBufferPass>();
    m_renderGraph->AddPass<ShadowPass>();
    m_renderGraph->AddPass<SkyboxPass>();
    m_renderGraph->AddPass<SSAO_Pass>();
    m_renderGraph->AddPass<GeometryPass>();
    m_renderGraph->AddPass<DeferredLightingPass>();
    m_renderGraph->AddPass<VFXPass>();
    m_taaPass = &m_renderGraph->AddPass<TAAPass>();
    m_renderGraph->AddPass<BloomPass>();
    m_renderGraph->AddPass<CompositionPass>();
    m_renderGraph->AddPass<VolumetricPass>();
    m_renderGraph->AddPass<UIPass>();

    RecreateFBO(1600, 900);
    m_unjitteredProjection = m_projectionMatrix;

    Log::Info("Renderer v7.0: Industrial Pipeline Initialized.");
    return true;
}

void Renderer::OnShutdown() {
    m_renderGraph.reset();
    m_shadowSystem.reset();
    m_ssaoSystem.reset();
    m_iblSystem.reset();
    m_dashboardSystem.reset();

    // Clean up IBL texture resources
    if (m_iblData.envCubemap) {
        glDeleteTextures(1, &m_iblData.envCubemap);
        m_iblData.envCubemap = 0;
    }
    if (m_iblData.irradianceMap) {
        glDeleteTextures(1, &m_iblData.irradianceMap);
        m_iblData.irradianceMap = 0;
    }
    if (m_iblData.prefilterMap) {
        glDeleteTextures(1, &m_iblData.prefilterMap);
        m_iblData.prefilterMap = 0;
    }
    if (m_iblData.brdfLUT) {
        glDeleteTextures(1, &m_iblData.brdfLUT);
        m_iblData.brdfLUT = 0;
    }

    PostProcessing::Shutdown();
}

void Renderer::OnRender() {
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
    if (script)
        script->OnRender();

    m_renderGraph->Execute();

    // --- Forward / Transparent Pass (after deferred) ---
    RenderForwardCommands();

    // Restore state
    glEnable(GL_DEPTH_TEST);
}

void Renderer::OnResize(int w, int h) {
    if (w <= 0 || h <= 0)
        return;
    if (w == (int)m_fboWidth && h == (int)m_fboHeight)
        return;

    Log::Info("Renderer: FBO auto-resize {}x{} -> {}x{}", m_fboWidth, m_fboHeight, w, h);
    RecreateFBO(w, h);

    // Also recreate SSAO buffers at the new resolution
    if (m_ssaoSystem) {
        m_ssaoSystem->Shutdown();
        m_ssaoSystem->Initialize();
    }
}

void Renderer::RecreateFBO(int width, int height) {
    m_fboWidth = width;
    m_fboHeight = height;

    // Destroy old resources before generating new ones (RAII: Ptr() overwrites m_id)
    m_fbo.Destroy();
    m_fboTexture.Destroy();
    m_rbo.Destroy();
    m_gBuffer.Destroy();
    m_gPosition.Destroy();
    m_gNormal.Destroy();
    m_gAlbedoSpec.Destroy();
    m_gRoughnessAO.Destroy();
    m_gDepth.Destroy();

    glGenFramebuffers(1, m_fbo.Ptr());
    m_fbo.Bind();
    glGenTextures(1, m_fboTexture.Ptr());
    m_fboTexture.Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture.Get(), 0);
    glGenRenderbuffers(1, m_rbo.Ptr());
    m_rbo.Bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo.Get());

    // --- G-Buffer Initialization ---
    glGenFramebuffers(1, m_gBuffer.Ptr());
    m_gBuffer.Bind();

    auto createGBufferTex = [&](GLTexture& tex, int attachmentIndex, GLint internalFormat, GLenum format, GLenum type) {
        glGenTextures(1, tex.Ptr());
        tex.Bind();
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_TEXTURE_2D, tex.Get(), 0);
    };

    createGBufferTex(m_gPosition, 0, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    createGBufferTex(m_gNormal, 1, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    createGBufferTex(m_gAlbedoSpec, 2, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    createGBufferTex(m_gRoughnessAO, 3, GL_RG8, GL_RG, GL_UNSIGNED_BYTE);

    uint32_t attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments);

    // Depth attachment for G-Buffer
    glGenRenderbuffers(1, m_gDepth.Ptr());
    m_gDepth.Bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gDepth.Get());

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Log::Error("Renderer: G-Buffer FBO Incomplete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (m_renderGraph) {
        m_renderGraph->Reset();
        m_renderGraph->ImportResource("SceneColor", m_fboTexture.Get(), m_fbo.Get());
        m_renderGraph->Compile();
    }
}

void Renderer::BeginFrame() {
    // Process deferred GPU resource deletions
    {
        std::lock_guard<std::mutex> lock(s_deletionMutex);
        if (!s_textureDeletionQueue.empty()) {
            glDeleteTextures(static_cast<GLsizei>(s_textureDeletionQueue.size()), s_textureDeletionQueue.data());
            s_textureDeletionQueue.clear();
        }
        for (const auto& mesh : s_meshDeletionQueue) {
            if (mesh.vao)
                glDeleteVertexArrays(1, &mesh.vao);
            if (mesh.vbo)
                glDeleteBuffers(1, &mesh.vbo);
            if (mesh.ebo)
                glDeleteBuffers(1, &mesh.ebo);
        }
        s_meshDeletionQueue.clear();
    }

    m_projectionMatrix = m_unjitteredProjection;
    if (m_taaPass) {
        m_taaPass->ApplyJitter(m_projectionMatrix, m_fboWidth, m_fboHeight);
    }

    m_fbo.Bind();
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    m_commandBuffer.clear();
    m_cameraTransform.UpdateLocalMatrix();
    m_cameraTransform.worldMatrix = m_cameraTransform.localMatrix;
    m_view = glm::inverse(m_cameraTransform.GetMatrix());
}

void Renderer::RenderRegistry(entt::registry& registry) {
    // Data population only. Rendering is handled by the RenderGraph.
    m_commandBuffer.clear();
    m_lastLightPositions.clear();
    m_lastLightColors.clear();

    m_stats.totalMeshes = 0;
    m_stats.visibleMeshes = 0;

    auto cvarSys = Engine::Get().GetSystem<CVarSystem>();
    bool enableCulling = cvarSys ? cvarSys->GetBool("r_cull") : true;

    if (enableCulling) {
        UpdateFrustumPlanes();
    }

    // 1. Submit Meshes using cache-friendly group
    auto group = registry.group<MeshComponent>(entt::get<TransformComponent>);
    group.each([this, enableCulling](const auto& mc, const auto& t) {
        if (!mc.mesh || !mc.isVisible)
            return;

        m_stats.totalMeshes++;

        if (enableCulling) {
            glm::vec3 worldPos = glm::vec3(t.worldMatrix[3]);
            if (!IsSphereInFrustum(worldPos, mc.boundingRadius)) {
                return; // Descarta objeto invisível (Culled!)
            }
        }

        m_stats.visibleMeshes++;

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

    // 2. Track light positions AND colors for Volumetric effects
    auto lights = registry.view<TransformComponent, PointLightComponent>();
    for (auto entity : lights) {
        auto& t = lights.get<TransformComponent>(entity);
        auto& l = lights.get<PointLightComponent>(entity);
        m_lastLightPositions.push_back(t.position);
        m_lastLightColors.push_back(l.color * l.intensity);
        if (m_lastLightPositions.size() >= 8)
            break;
    }
}

void Renderer::Submit(const RenderCommand& command) {
    m_commandBuffer.push_back(command);
}
void Renderer::SubmitForward(const RenderCommand& command) {
    m_forwardCommandBuffer.push_back(command);
}

void Renderer::RenderForwardCommands() {
    if (m_forwardCommandBuffer.empty())
        return;

    // Sort transparent objects back-to-front (farthest first)
    glm::vec3 camPos = m_cameraTransform.position;
    std::sort(m_forwardCommandBuffer.begin(),
        m_forwardCommandBuffer.end(),
        [&camPos](const RenderCommand& a, const RenderCommand& b) {
            float distA = glm::length(glm::vec3(a.transform[3]) - camPos);
            float distB = glm::length(glm::vec3(b.transform[3]) - camPos);
            return distA > distB;  // far to near
        });

    // Render with alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);  // Don't write depth for transparent objects

    m_pbrShader->Use();
    m_pbrShader->SetMat4U("view", m_view);
    m_pbrShader->SetMat4U("projection", m_projectionMatrix);
    m_pbrShader->SetVec3U("viewPos", m_cameraTransform.position);

    for (const auto& cmd : m_forwardCommandBuffer) {
        if (!cmd.mesh)
            continue;
        m_pbrShader->SetMat4U("model", cmd.transform);
        m_pbrShader->SetVec3U("albedo", cmd.albedo);
        m_pbrShader->SetFloatU("metallic", cmd.metallic);
        m_pbrShader->SetFloatU("roughness", cmd.roughness);
        m_pbrShader->SetFloatU("ao", cmd.ao);
        cmd.mesh->Draw();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    m_forwardCommandBuffer.clear();
}

void Renderer::ReloadAllShaders() {
    Log::Info("Renderer: Hot-reloading all shaders...");
    int reloaded = 0, failed = 0;

    auto tryReload = [&](std::shared_ptr<Shader>& s, const char* name) {
        if (s && s->HasSourcePaths()) {
            if (s->Reload()) {
                reloaded++;
            } else {
                failed++;
                Log::Warn("  Failed to reload: {}", name);
            }
        }
    };

    tryReload(m_gbufferShader, "gbuffer");
    tryReload(m_pbrShader, "pbr");
    tryReload(m_uiShader, "ui");
    tryReload(m_screenShader, "screen/composition");
    tryReload(m_skyboxShader, "skybox");
    tryReload(m_bloomBrightShader, "bloom_bright");
    tryReload(m_bloomBlurShader, "bloom_blur");
    tryReload(m_ssrShader, "ssr");
    tryReload(m_postComposeShader, "post_compose");
    tryReload(m_deferredLightShader, "deferred_light");

    Log::Info("Renderer: Shader reload complete ({} reloaded, {} failed).", reloaded, failed);
}
void Renderer::UpdateProjection(float fov, float aspect, float nearPlane, float farPlane) {
    m_projectionMatrix = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    m_unjitteredProjection = m_projectionMatrix;
}

void Renderer::SetOrthoProjection(float width, float height) {
    m_projectionMatrix = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
    m_unjitteredProjection = m_projectionMatrix;
}

void Renderer::SetCameraLookAt(glm::vec3 target) {
    m_view = glm::lookAt(m_cameraTransform.position, target, glm::vec3(0, 1, 0));
    // Also update camera rotation to match (optional but good for consistency)
    // m_cameraTransform.rotation = glm::quatLookAt(glm::normalize(target - m_cameraTransform.position), glm::vec3(0, 1,
    // 0));
}

void Renderer::RenderSkinnedMeshes(entt::registry& registry) {
    auto view = registry.view<TransformComponent, MeshComponent, SkeletalAnimationComponent>();
    view.each([this](const auto& transform, const auto& mc, const auto& anim) {
        if (!mc.mesh || !anim.playing)
            return;

        m_pbrShader->Use();
        m_pbrShader->SetMat4U("model", transform.GetMatrix());
        m_pbrShader->SetVec3U("albedo", mc.material.albedo);
        m_pbrShader->SetIntU("has_anim", 1);

        for (size_t i = 0; i < anim.boneMatrices.size() && i < 100; i++) {
            char nameBuf[64];
            snprintf(nameBuf, sizeof(nameBuf), "bone_matrices[%d]", (int)i);
            m_pbrShader->SetMat4U(nameBuf, anim.boneMatrices[i]);
        }

        mc.mesh->Draw();
    });
    m_pbrShader->SetIntU("has_anim", 0);
}

void Renderer::RenderSkinnedToGBuffer(entt::registry& registry) {
    auto view = registry.view<TransformComponent, MeshComponent, SkeletalAnimationComponent>();
    view.each([this](const auto& transform, const auto& mc, const auto& anim) {
        if (!mc.mesh || !anim.playing)
            return;

        m_gbufferShader->Use();
        m_gbufferShader->SetMat4U("model", transform.GetMatrix());
        m_gbufferShader->SetVec3U("albedoColor", mc.material.albedo);
        m_gbufferShader->SetIntU("has_anim", 1);

        for (size_t i = 0; i < anim.boneMatrices.size() && i < 100; i++) {
            char nameBuf[64];
            snprintf(nameBuf, sizeof(nameBuf), "bone_matrices[%d]", (int)i);
            m_gbufferShader->SetMat4U(nameBuf, anim.boneMatrices[i]);
        }

        mc.mesh->Draw();
    });
    m_gbufferShader->SetIntU("has_anim", 0);
}

void Renderer::SetShadowResolution(int resolution) {
    if (m_shadowResolution == resolution && m_shadowSystem) return;
    m_shadowResolution = resolution;
    if (m_shadowSystem) {
        m_shadowSystem.reset();
    }
    m_shadowSystem = std::make_unique<ShadowSystem>(resolution);
    m_shadowSystem->Initialize();
    Log::Info("Renderer: Shadow Resolution set to {}x{}", resolution, resolution);
}

void Renderer::SetGraphicsPreset(int preset) {
    m_qualityPreset = preset;
    auto vfx = Engine::Get().GetSystem<VFXSystem>();
    if (preset == 0) { // Performance
        SetShadowResolution(1024);
        SetSSAOEnabled(false);
        SetBloomBlurSteps(2);
        if (vfx) vfx->SetMaxParticles(64000);
        PostProcessing::SetChromaticStrength(0.0f);
        PostProcessing::SetVignetteStrength(0.0f);
        Log::Info("Renderer: Graphics Preset set to PERFORMANCE (Low).");
    } else if (preset == 1) { // Balanced
        SetShadowResolution(2048);
        SetSSAOEnabled(true);
        SetBloomBlurSteps(5);
        if (vfx) vfx->SetMaxParticles(128000);
        PostProcessing::SetChromaticStrength(0.003f);
        PostProcessing::SetVignetteStrength(0.25f);
        Log::Info("Renderer: Graphics Preset set to BALANCED (Medium).");
    } else { // Fidelity / Ultra
        SetShadowResolution(4096);
        SetSSAOEnabled(true);
        SetBloomBlurSteps(10);
        if (vfx) vfx->SetMaxParticles(256000);
        PostProcessing::SetChromaticStrength(0.006f);
        PostProcessing::SetVignetteStrength(0.35f);
        Log::Info("Renderer: Graphics Preset set to FIDELITY (Ultra).");
    }
}

void Renderer::UpdateFrustumPlanes() {
    glm::mat4 vp = m_projectionMatrix * m_view;

    // Left Plane
    m_frustumPlanes[0].normal.x = vp[0][3] + vp[0][0];
    m_frustumPlanes[0].normal.y = vp[1][3] + vp[1][0];
    m_frustumPlanes[0].normal.z = vp[2][3] + vp[2][0];
    m_frustumPlanes[0].distance = vp[3][3] + vp[3][0];

    // Right Plane
    m_frustumPlanes[1].normal.x = vp[0][3] - vp[0][0];
    m_frustumPlanes[1].normal.y = vp[1][3] - vp[1][0];
    m_frustumPlanes[1].normal.z = vp[2][3] - vp[2][0];
    m_frustumPlanes[1].distance = vp[3][3] - vp[3][0];

    // Bottom Plane
    m_frustumPlanes[2].normal.x = vp[0][3] + vp[0][1];
    m_frustumPlanes[2].normal.y = vp[1][3] + vp[1][1];
    m_frustumPlanes[2].normal.z = vp[2][3] + vp[2][1];
    m_frustumPlanes[2].distance = vp[3][3] + vp[3][1];

    // Top Plane
    m_frustumPlanes[3].normal.x = vp[0][3] - vp[0][1];
    m_frustumPlanes[3].normal.y = vp[1][3] - vp[1][1];
    m_frustumPlanes[3].normal.z = vp[2][3] - vp[2][1];
    m_frustumPlanes[3].distance = vp[3][3] - vp[3][1];

    // Near Plane
    m_frustumPlanes[4].normal.x = vp[0][3] + vp[0][2];
    m_frustumPlanes[4].normal.y = vp[1][3] + vp[1][2];
    m_frustumPlanes[4].normal.z = vp[2][3] + vp[2][2];
    m_frustumPlanes[4].distance = vp[3][3] + vp[3][2];

    // Far Plane
    m_frustumPlanes[5].normal.x = vp[0][3] - vp[0][2];
    m_frustumPlanes[5].normal.y = vp[1][3] - vp[1][2];
    m_frustumPlanes[5].normal.z = vp[2][3] - vp[2][2];
    m_frustumPlanes[5].distance = vp[3][3] - vp[3][2];

    for (int i = 0; i < 6; i++) {
        m_frustumPlanes[i].Normalize();
    }
}

bool Renderer::IsSphereInFrustum(const glm::vec3& center, float radius) const {
    for (int i = 0; i < 6; i++) {
        float distance = glm::dot(m_frustumPlanes[i].normal, center) + m_frustumPlanes[i].distance;
        if (distance < -radius) {
            return false;
        }
    }
    return true;
}

}  // namespace starlight
