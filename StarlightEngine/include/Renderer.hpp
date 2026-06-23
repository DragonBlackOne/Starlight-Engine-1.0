#pragma once
#include <entt/entt.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <mutex>
#include <vector>
#include "Components.hpp"
#include "DashboardSystem.hpp"
#include "EngineSystem.hpp"
#include "GLResource.hpp"
#include "GizmoSystem.hpp"
#include "IBLSystem.hpp"
#include "Mesh.hpp"
#include "OctreeSystem.hpp"
#include "RenderGraph.hpp"
#include "SSAO_System.hpp"
#include "Shader.hpp"
#include "ShadowSystem.hpp"

namespace starlight {
class ParticleSystem;
class TAAPass;

struct RenderCommand {
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Shader> shader;
    glm::mat4 transform;
    glm::vec3 albedo = glm::vec3(1.0f);
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;
};

class Renderer : public ISystem {
    friend class DeferredLightingPass;
    friend class TAAPass;
    friend class GBufferPass;
    friend class ShadowPass;
    friend class SkyboxPass;
    friend class GeometryPass;
    friend class SSAO_Pass;
    friend class VolumetricPass;
    friend class BloomPass;
    friend class SSRPass;
    friend class CompositionPass;
    friend class UIPass;

public:
    Renderer();
    ~Renderer();

    // ISystem
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnRender() override;
    void OnResize(int w, int h) override;
    const char* GetName() const override {
        return "Renderer";
    }
    bool IsMainThreadOnly() const override { return true; }

    void UpdateProjection(float fov, float aspect, float nearP, float farP);
    void SetOrthoProjection(float width, float height);
    void SetProjectionMatrix(const glm::mat4& proj) {
        m_projectionMatrix = proj;
        m_unjitteredProjection = proj;
    }
    void SetCameraLookAt(glm::vec3 target);
    void SetViewMatrix(const glm::mat4& view) {
        m_view = view;
    }

    // Deferred GL Resource Deletion (Thread-Safe)
    static void SubmitDeferredTextureDeletion(uint32_t texID);
    static void SubmitDeferredMeshDeletion(uint32_t vao, uint32_t vbo, uint32_t ebo);

    const glm::mat4& GetViewMatrix() const {
        return m_view;
    }
    const glm::mat4& GetProjectionMatrix() const {
        return m_projectionMatrix;
    }
    TransformComponent& GetCameraTransform() {
        return m_cameraTransform;
    }

    void BeginFrame();
    void Submit(const RenderCommand& command);
    void SubmitForward(const RenderCommand& command);

    /// Hot-reload all shaders that were loaded from files
    void ReloadAllShaders();

    /// Render forward/transparent commands (call after deferred pass)
    void RenderForwardCommands();

    int GetFBOWidth() const {
        return m_fboWidth;
    }
    int GetFBOHeight() const {
        return m_fboHeight;
    }

    void RenderRegistry(entt::registry& registry);
    void RenderSkinnedMeshes(entt::registry& registry);
    void RenderSkinnedToGBuffer(entt::registry& registry);

    std::shared_ptr<Shader> GetPBRShader() const {
        return m_pbrShader;
    }
    std::shared_ptr<Shader> GetUIShader() const {
        return m_uiShader;
    }
    std::shared_ptr<Mesh> GetCubeMesh() const {
        return m_cubeMesh;
    }
    std::shared_ptr<Mesh> GetQuadMesh() const {
        return m_quadMesh;
    }

    DashboardSystem& GetDashboard() {
        return *m_dashboardSystem;
    }
    OctreeSystem& GetOctree() {
        return *m_octreeSystem;
    }

    uint32_t GetSceneTexture() const {
        return m_fboTexture.Get();
    }

    struct RendererStats {
        uint32_t totalMeshes = 0;
        uint32_t visibleMeshes = 0;
    };

    const RendererStats& GetStats() const {
        return m_stats;
    }
    void ResizeSceneFBO(uint32_t width, uint32_t height) {
        RecreateFBO(width, height);
    }

    // Viewport override for editor
    void SetViewportOverride(GLuint fbo, int width, int height) {
        m_viewportFBO = fbo;
        m_viewportWidth = width;
        m_viewportHeight = height;
    }
    void ClearViewportOverride() {
        m_viewportFBO = 0;
        m_viewportWidth = 0;
        m_viewportHeight = 0;
    }

    float GetBloomThreshold() const {
        return m_bloomThreshold;
    }
    void SetBloomThreshold(float t) {
        m_bloomThreshold = t;
    }

    int GetBloomBlurSteps() const {
        return m_bloomBlurSteps;
    }
    void SetBloomBlurSteps(int s) {
        m_bloomBlurSteps = s;
    }

    float GetExposure() const {
        return m_exposure;
    }
    void SetExposure(float e) {
        m_exposure = e;
    }

    float GetGamma() const {
        return m_gamma;
    }
    void SetGamma(float g) {
        m_gamma = g;
    }

    void SetClearColor(const glm::vec3& color) {
        m_clearColor = color;
    }
    glm::vec3 GetClearColor() const {
        return m_clearColor;
    }

    void SetGraphicsPreset(int preset);
    int GetGraphicsPreset() const { return m_qualityPreset; }
    void SetShadowResolution(int resolution);
    int GetShadowResolution() const { return m_shadowResolution; }
    bool IsSSAOEnabled() const { return m_useSSAO; }
    void SetSSAOEnabled(bool enabled) { m_useSSAO = enabled; }

private:
    float m_bloomThreshold = 1.0f;
    int m_bloomBlurSteps = 10;
    float m_exposure = 1.0f;
    float m_gamma = 2.2f;
    glm::vec3 m_clearColor = {0.02f, 0.02f, 0.04f};
    glm::mat4 m_view, m_projectionMatrix;
    std::vector<RenderCommand> m_commandBuffer;
    std::vector<RenderCommand> m_forwardCommandBuffer;

    // FBO and Textures
    GLFramebuffer m_fbo;
    GLTexture m_fboTexture;
    GLRenderbuffer m_rbo;
    uint32_t m_fboWidth = 1600, m_fboHeight = 900;
    GLTexture m_skyboxCubemap;

    // G-Buffer
    GLFramebuffer m_gBuffer;
    GLTexture m_gPosition, m_gNormal, m_gAlbedoSpec, m_gRoughnessAO;
    GLRenderbuffer m_gDepth;

    // Shaders
    std::shared_ptr<Shader> m_basicShader;
    std::shared_ptr<Shader> m_gbufferShader;
    std::shared_ptr<Shader> m_pbrShader;
    std::shared_ptr<Shader> m_deferredLightShader;
    std::shared_ptr<Shader> m_skyboxShader;
    std::shared_ptr<Shader> m_uiShader;
    std::shared_ptr<Shader> m_screenShader;
    std::shared_ptr<Shader> m_bloomBrightShader;
    std::shared_ptr<Shader> m_bloomBlurShader;
    std::shared_ptr<Shader> m_ssrShader;
    std::shared_ptr<Shader> m_postComposeShader;
    std::shared_ptr<Shader> m_volumetricShader;
    std::shared_ptr<Shader> m_cloudShader;
    std::shared_ptr<Shader> m_mode7Shader;
    std::shared_ptr<Shader> m_crtShader;

    // Default Meshes
    std::shared_ptr<Mesh> m_quadMesh;
    std::shared_ptr<Mesh> m_cubeMesh;

    // Core Systems
    std::unique_ptr<ShadowSystem> m_shadowSystem;
    std::unique_ptr<SSAO_System> m_ssaoSystem;
    std::unique_ptr<DashboardSystem> m_dashboardSystem;
    std::unique_ptr<GizmoSystem> m_gizmoSystem;
    std::unique_ptr<OctreeSystem> m_octreeSystem;
    std::unique_ptr<IBLSystem> m_iblSystem;
    IBLData m_iblData;
    std::unique_ptr<RenderGraph> m_renderGraph;

    // Cached Lights
    std::vector<glm::vec3> m_lastLightPositions;
    std::vector<glm::vec3> m_lastLightColors;

    TransformComponent m_cameraTransform;

    // Viewport override (set by EditorSystem)
    GLuint m_viewportFBO = 0;
    int m_viewportWidth = 0;
    int m_viewportHeight = 0;

    int m_shadowResolution = 2048;
    int m_qualityPreset = 1; // 0 = Performance, 1 = Balanced, 2 = Fidelity
    bool m_useSSAO = true;

    struct Plane {
        glm::vec3 normal;
        float distance;
        void Normalize() {
            float length = glm::length(normal);
            normal /= length;
            distance /= length;
        }
    };
    std::array<Plane, 6> m_frustumPlanes;
    RendererStats m_stats;

    void UpdateFrustumPlanes();
    bool IsSphereInFrustum(const glm::vec3& center, float radius) const;

    void RecreateFBO(int width, int height);

    TAAPass* m_taaPass = nullptr;
    glm::mat4 m_unjitteredProjection = glm::mat4(1.0f);

    struct MeshDeletion {
        uint32_t vao, vbo, ebo;
    };
    static std::vector<uint32_t> s_textureDeletionQueue;
    static std::vector<MeshDeletion> s_meshDeletionQueue;
    static std::mutex s_deletionMutex;
};
}  // namespace starlight
