#include "VFXSystem.hpp"
#include "Renderer.hpp"
#include "Engine.hpp"
#include "Log.hpp"
#include <glad/glad.h>
#include <random>

namespace starlight {

    VFXSystem::VFXSystem()
        : m_randomEngine(std::random_device{}()), m_randomDistribution(-1.0f, 1.0f) {}

    VFXSystem::~VFXSystem() {
        OnShutdown();
    }

    bool VFXSystem::OnInitialize(const EngineContext& context) {
        (void)context;
        
        // 1. Setup SSBO
        glGenBuffers(1, &m_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_PARTICLES * sizeof(GPUParticle), NULL, GL_DYNAMIC_DRAW);
        
        // Initialize with dead particles
        GPUParticle* p = (GPUParticle*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
        for(int i=0; i<MAX_PARTICLES; ++i) {
            p[i].position = {0,0,0,0}; // life = 0
            p[i].velocity = {0,0,0,0.1f}; // size = 0.1
            p[i].color = {1,1,1,1};
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // 2. Setup VAO for rendering
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_ssbo);
        
        // Bind SSBO as vertex attributes for the render shader
        glEnableVertexAttribArray(0); // position.xyz
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)offsetof(GPUParticle, position));
        glEnableVertexAttribArray(1); // velocity.xyz, size (w)
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)offsetof(GPUParticle, velocity));
        glEnableVertexAttribArray(2); // color
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)offsetof(GPUParticle, color));
        
        glBindVertexArray(0);

        // 3. Load Shaders
        m_computeShader = std::make_shared<ComputeShader>("assets/shaders/particle_update.comp");
        m_renderShader = Shader::LoadFromFile("assets/shaders/particle.vert", "assets/shaders/particle.frag");

        m_initialized = true;
        Log::Info("VFXSystem: Industrial GPU Particle Engine initialized ({} particles).", MAX_PARTICLES);
        return true;
    }

    void VFXSystem::OnShutdown() {
        if (m_ssbo) glDeleteBuffers(1, &m_ssbo);
        if (m_vao) glDeleteVertexArrays(1, &m_vao);
        m_ssbo = 0;
        m_vao = 0;
    }

    void VFXSystem::Emit(const glm::vec3& pos, const glm::vec3& velRange, const glm::vec4& color, int count, float size) {
        m_pending.push_back({pos, velRange, color, count, size});
    }

    void VFXSystem::OnUpdate(float dt) {
        if (!m_initialized) return;

        // Process pending emissions (Inject into SSBO)
        if (!m_pending.empty()) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
            GPUParticle* particles = (GPUParticle*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
            
            static int lastIdx = 0;

            for (const auto& e : m_pending) {
                int emitted = 0;
                int searchStart = lastIdx;
                // Find dead slots
                for (int i = 0; i < m_maxParticles && emitted < e.count; ++i) {
                    int idx = (searchStart + i) % m_maxParticles;
                    if (particles[idx].position.w <= 0.0f) {
                        particles[idx].position = glm::vec4(e.pos, 2.0f); // 2.0s life
                        particles[idx].velocity = glm::vec4(
                            e.velRange.x * m_randomDistribution(m_randomEngine),
                            e.velRange.y * (0.5f + 0.5f * std::abs(m_randomDistribution(m_randomEngine))),
                            e.velRange.z * m_randomDistribution(m_randomEngine),
                            e.size
                        );
                        particles[idx].color = e.color;
                        emitted++;
                        lastIdx = (idx + 1) % m_maxParticles;
                    }
                }
            }
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            m_pending.clear();
        }

        // GPU Simulation
        m_computeShader->Use();
        m_computeShader->SetFloatU("dt", dt);
        m_computeShader->SetFloatU("max_life", 2.0f);
        // Note: The existing shader has an "auto-respawner" in the 'else' block.
        // We should probably modify it to NOT respawn if we want full manual control.
        // But for now, let's just dispatch.
        
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo);
        m_computeShader->Dispatch(m_maxParticles / 256, 1, 1);
        m_computeShader->Wait();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    }

    void VFXSystem::RenderInternal() {
        if (!m_initialized) return;

        auto& renderer = Engine::Get().GetRenderer();
        // This is a 3D effect, should be rendered in the main pass or a specific VFX pass
        // For simplicity, we'll render here with standard depth/blend
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive for "Industrial Glow"
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_PROGRAM_POINT_SIZE);

        m_renderShader->Use();
        m_renderShader->SetMat4U("m_view", renderer.GetViewMatrix());
        m_renderShader->SetMat4U("m_proj", renderer.GetProjectionMatrix());

        glBindVertexArray(m_vao);
        glDrawArrays(GL_POINTS, 0, m_maxParticles);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDepthMask(GL_TRUE);
        glDisable(GL_PROGRAM_POINT_SIZE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);
        glUseProgram(0);
    }
}
