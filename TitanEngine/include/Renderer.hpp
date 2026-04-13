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

namespace titan {
    class ParticleSystem;

    struct RenderCommand {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Shader> shader;
        glm::mat4 transform;
    };

    class Renderer {
    public:
        Renderer();
        ~Renderer();

        void Initialize();
        void UpdateProjection(float fov, float aspect, float nearP, float farP);
        void SetViewMatrix(const glm::mat4& view) { m_view = view; }
        
        void BeginFrame();
        void Submit(const RenderCommand& command);
        void EndFrame();

        void RenderRegistry(entt::registry& registry);
        void DrawParticles(ParticleSystem& ps);

        std::shared_ptr<Shader> GetBasicShader() { return m_basicShader; }
        std::shared_ptr<Shader> GetPBRShader() { return m_pbrShader; }
        std::shared_ptr<Shader> GetPostComposeShader() { return m_postComposeShader; }
        std::shared_ptr<Mesh> GetCubeMesh() { return m_cubeMesh; }
        DashboardSystem& GetDashboard() { return *m_dashboardSystem; }
        OctreeSystem& GetOctree() { return *m_octreeSystem; }

    private:
        glm::mat4 m_view, m_projectionMatrix;
        std::vector<RenderCommand> m_commandBuffer;
        
        // FBO and Post Processing
        uint32_t m_fbo = 0;
        uint32_t m_fboTexture = 0;
        uint32_t m_depthTexture = 0;
        uint32_t m_rbo = 0;
        void RecreateFBO(int width, int height);

        // Default Shaders
        std::shared_ptr<Shader> m_basicShader;
        std::shared_ptr<Shader> m_mode7Shader;
        std::shared_ptr<Shader> m_crtShader;
        std::shared_ptr<Shader> m_pbrShader;
        
        // Default Meshes
        std::shared_ptr<Mesh> m_quadMesh;
        std::shared_ptr<Mesh> m_cubeMesh;

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
    };
}
