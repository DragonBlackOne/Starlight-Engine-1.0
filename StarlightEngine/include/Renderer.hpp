// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <vector>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Components.hpp"
#include "ShadowSystem.hpp"
#include "SSAO_System.hpp"
#include "DashboardSystem.hpp"
#include "GizmoSystem.hpp"
#include "OctreeSystem.hpp"
#include "RenderGraph.hpp"

namespace starlight {
    class ParticleSystem;

    struct RenderCommand {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Shader> shader;
        glm::mat4 transform;
        glm::vec3 albedo = glm::vec3(1.0f);
        float metallic = 0.0f;
        float roughness = 0.5f;
        float ao = 1.0f;
    };

    class Renderer {
    public:
        Renderer();
        ~Renderer();

        void Initialize();
        void UpdateProjection(float fov, float aspect, float nearP, float farP);
        void SetViewMatrix(const glm::mat4& view) { m_view = view; }
        TransformComponent& GetCameraTransform() { return m_cameraTransform; }
        
        void BeginFrame();
        void Submit(const RenderCommand& command);
        void SubmitForward(const RenderCommand& command);
        void EndFrame();

        void RenderRegistry(entt::registry& registry);
        void DrawParticles(ParticleSystem& ps);

        std::shared_ptr<Shader> GetBasicShader() { return m_basicShader; }
        std::shared_ptr<Shader> GetPBRShader() { return m_pbrShader; }
        std::shared_ptr<Shader> GetPostComposeShader() { return m_postComposeShader; }
        std::shared_ptr<Shader> GetUIShader() { return m_uiShader; }
        std::shared_ptr<Mesh> GetCubeMesh() { return m_cubeMesh; }
        DashboardSystem& GetDashboard() { return *m_dashboardSystem; }
        OctreeSystem& GetOctree() { return *m_octreeSystem; }

        uint32_t GetSceneTexture() const { return m_finalColorTex; }
        void ResizeSceneFBO(uint32_t width, uint32_t height) { RecreateFBO(width, height); }

        float m_bloomThreshold = 1.0f;
        int m_bloomBlurSteps = 10;
        float m_exposure = 1.0f;
        float m_gamma = 2.2f;

    private:
        glm::mat4 m_view, m_projectionMatrix;
        std::vector<RenderCommand> m_commandBuffer;
        std::vector<RenderCommand> m_forwardCommandBuffer;
        
        // FBO and Post Processing
        uint32_t m_fbo = 0;
        uint32_t m_fboTexture = 0;
        uint32_t m_depthTexture = 0;
        uint32_t m_rbo = 0;
        
        uint32_t m_fboWidth = 1280;
        uint32_t m_fboHeight = 720;
        
        uint32_t m_finalFBO = 0;
        uint32_t m_finalColorTex = 0;
        
        void RecreateFBO(int width, int height);

        // Default Shaders
        std::shared_ptr<Shader> m_basicShader;
        std::shared_ptr<Shader> m_mode7Shader;
        std::shared_ptr<Shader> m_crtShader;
        std::shared_ptr<Shader> m_pbrShader;
        std::shared_ptr<Shader> m_uiShader;
        
        // Default Meshes
        std::shared_ptr<Mesh> m_quadMesh;
        std::shared_ptr<Mesh> m_cubeMesh;

        // G-Buffer for Deferred Rendering
        uint32_t m_gBuffer = 0;
        uint32_t m_gPosition = 0, m_gNormal = 0, m_gAlbedoSpec = 0, m_gRoughnessAO = 0;
        std::shared_ptr<Shader> m_deferredLightShader;
        std::shared_ptr<Shader> m_gbufferShader;
        std::shared_ptr<Shader> m_skyboxShader;
        uint32_t m_skyboxCubemap = 0;

    public:
        // Volumetric Clouds
        float m_cloudCoverage = 0.5f;
        float m_cloudDensity = 0.05f;

        std::shared_ptr<Shader> m_cloudShader;
        std::shared_ptr<Shader> m_screenShader;
        
        // Specialized Systems
        std::unique_ptr<ShadowSystem> m_shadowSystem;
        std::unique_ptr<SSAO_System> m_ssaoSystem;
        std::unique_ptr<DashboardSystem> m_dashboardSystem;
        std::unique_ptr<GizmoSystem> m_gizmoSystem;
        std::unique_ptr<OctreeSystem> m_octreeSystem;

        // Phase 10 Post-FX
        std::shared_ptr<Shader> m_bloomBrightShader;
        std::shared_ptr<Shader> m_bloomBlurShader;
        std::shared_ptr<Shader> m_postComposeShader;
        uint32_t m_pingpongFBO[2], m_pingpongColorbuffers[2];

        // Escala 10 TrilhÃµes
        std::unique_ptr<RenderGraph> m_renderGraph;

        // Cached Lights for Deferred Pass
        std::vector<glm::vec3> m_lastLightPositions;
        std::vector<glm::vec3> m_lastLightColors;

        TransformComponent m_cameraTransform;
    };
}
