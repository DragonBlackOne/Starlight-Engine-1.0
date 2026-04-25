// Starlight Engine: PBR FORWARD RENDERER
#include "Renderer.hpp"
#include "Log.hpp"
#include <glad/glad.h>
#include "Engine.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace starlight {

    // Helper to create a cube mesh locally
    static std::shared_ptr<Mesh> InternalCreateCube() {
        std::vector<Vertex> vertices = {
            // Front
            {{-1, -1,  1}, {0, 0, 1}, {0, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1, -1,  1}, {0, 0, 1}, {1, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1,  1,  1}, {0, 0, 1}, {1, 1}, {0,0,0,0}, {0,0,0,0}}, {{-1,  1,  1}, {0, 0, 1}, {0, 1}, {0,0,0,0}, {0,0,0,0}},
            // Back
            {{-1, -1, -1}, {0, 0, -1}, {0, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1, -1, -1}, {0, 0, -1}, {1, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1,  1, -1}, {0, 0, -1}, {1, 1}, {0,0,0,0}, {0,0,0,0}}, {{-1,  1, -1}, {0, 0, -1}, {0, 1}, {0,0,0,0}, {0,0,0,0}},
            // Top
            {{-1,  1,  1}, {0, 1, 0}, {0, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1,  1,  1}, {0, 1, 0}, {1, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1,  1, -1}, {0, 1, 0}, {1, 1}, {0,0,0,0}, {0,0,0,0}}, {{-1,  1, -1}, {0, 1, 0}, {0, 1}, {0,0,0,0}, {0,0,0,0}},
            // Bottom
            {{-1, -1,  1}, {0, -1, 0}, {0, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1, -1,  1}, {0, -1, 0}, {1, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1, -1, -1}, {0, -1, 0}, {1, 1}, {0,0,0,0}, {0,0,0,0}}, {{-1, -1, -1}, {0, -1, 0}, {0, 1}, {0,0,0,0}, {0,0,0,0}},
            // Right
            {{ 1, -1,  1}, {1, 0, 0}, {0, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1, -1, -1}, {1, 0, 0}, {1, 0}, {0,0,0,0}, {0,0,0,0}}, {{ 1,  1, -1}, {1, 0, 0}, {1, 1}, {0,0,0,0}, {0,0,0,0}}, {{ 1,  1,  1}, {1, 0, 0}, {0, 1}, {0,0,0,0}, {0,0,0,0}},
            // Left
            {{-1, -1,  1}, {-1, 0, 0}, {0, 0}, {0,0,0,0}, {0,0,0,0}}, {{-1, -1, -1}, {-1, 0, 0}, {1, 0}, {0,0,0,0}, {0,0,0,0}}, {{-1,  1, -1}, {-1, 0, 0}, {1, 1}, {0,0,0,0}, {0,0,0,0}}, {{-1,  1,  1}, {-1, 0, 0}, {0, 1}, {0,0,0,0}, {0,0,0,0}}
        };
        std::vector<uint32_t> indices = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            8, 9, 10, 10, 11, 8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20
        };
        return std::make_shared<Mesh>(vertices, indices);
    }

    Renderer::Renderer() : m_fboWidth(1280), m_fboHeight(720) {}
    Renderer::~Renderer() {}

    void Renderer::Initialize() {
        Log::Info("Renderer: Initializing Forward PBR Pipeline...");
        
        m_fboWidth = Engine::Get().GetWindow().GetWidth();
        m_fboHeight = Engine::Get().GetWindow().GetHeight();

        // Load Basic Shaders
        m_pbrShader = Shader::LoadFromFile("assets/shaders/pbr.vert", "assets/shaders/pbr.frag");
        if (!m_pbrShader) {
            Log::Error("Renderer: Failed to load PBR shader!");
        }

        // Initialize Default Meshes
        m_cubeMesh = InternalCreateCube();
        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Renderer::BeginFrame() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_fboWidth, m_fboHeight);
        
        // Dark Neutral Grey for Showcase
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        m_commandBuffer.clear();

        // Update View Matrix from Camera Transform
        m_cameraTransform.UpdateLocalMatrix();
        m_cameraTransform.worldMatrix = m_cameraTransform.localMatrix; 
        m_view = glm::inverse(m_cameraTransform.GetMatrix());
    }

    void Renderer::EndFrame() {
        if (!m_pbrShader) return;

        m_pbrShader->Use();
        m_pbrShader->SetMat4("projection", m_projectionMatrix);
        m_pbrShader->SetMat4("view", m_view);
        
        // Camera position for PBR highlights
        glm::vec3 camPos = m_cameraTransform.position;
        m_pbrShader->SetVec3("camPos", camPos);

        // Render all submitted meshes
        for (const auto& cmd : m_commandBuffer) {
            m_pbrShader->SetMat4("model", cmd.transform);
            m_pbrShader->SetVec3("albedo", cmd.albedo);
            m_pbrShader->SetFloat("metallic", cmd.metallic);
            m_pbrShader->SetFloat("roughness", cmd.roughness);
            
            if (cmd.mesh) cmd.mesh->Draw();
        }
    }

    void Renderer::RenderRegistry(entt::registry& registry) {
        // Collect all meshes and lights
        auto view = registry.view<TransformComponent, MeshComponent>();
        for (auto entity : view) {
            auto& t = view.get<TransformComponent>(entity);
            auto& m = view.get<MeshComponent>(entity);
            
            // Sync transform before rendering
            t.UpdateLocalMatrix();
            t.worldMatrix = t.localMatrix; // Simple forward sync
            
            RenderCommand cmd;
            cmd.mesh = m.mesh;
            cmd.transform = t.GetMatrix();
            cmd.albedo = m.material.color;
            cmd.metallic = m.material.metallic;
            cmd.roughness = m.material.roughness;
            
            Submit(cmd);
        }
        
        // Pass light data to shader
        auto lights = registry.view<TransformComponent, PointLightComponent>();
        int i = 0;
        for (auto entity : lights) {
            if (i >= 8) break; // Limit to 8 lights for forward pass
            auto& t = lights.get<TransformComponent>(entity);
            auto& l = lights.get<PointLightComponent>(entity);
            
            std::string base = "lights[" + std::to_string(i) + "].";
            m_pbrShader->Use();
            m_pbrShader->SetVec3(base + "position", t.position);
            m_pbrShader->SetVec3(base + "color", l.color);
            m_pbrShader->SetFloat(base + "intensity", l.intensity);
            i++;
        }
        m_pbrShader->SetInt("lightCount", i);
    }

    void Renderer::Submit(const RenderCommand& command) {
        m_commandBuffer.push_back(command);
    }

    void Renderer::UpdateProjection(float fov, float aspect, float nearP, float farP) {
        m_projectionMatrix = glm::perspective(glm::radians(fov), aspect, nearP, farP);
    }

    void Renderer::SubmitForward(const RenderCommand& c) {}
    void Renderer::RecreateFBO(int w, int h) { m_fboWidth = w; m_fboHeight = h; }
    void Renderer::DrawParticles(ParticleSystem& p) {}
}
