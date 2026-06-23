#include "ParticleSystem.hpp"
#include "wicked/core/wiJobSystem.h"
#include <wiJobSystem.h> // Alternate for common setups
#include "Log.hpp"
#include <glad/glad.h>
#include <algorithm>
#include <immintrin.h>

namespace starlight {
    struct ParticleJob {
        ParticleSystem* sys;
        int start, end;
        float dt;
    };

    ParticleSystem::ParticleSystem(uint32_t maxParticles) : m_maxParticles(maxParticles) {
        m_particles.resize(maxParticles);
        
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(Particle), NULL, GL_DYNAMIC_DRAW);
        
        // Atributos p/ shader de partcula
        glEnableVertexAttribArray(0); // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
        glEnableVertexAttribArray(1); // Color
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
        glEnableVertexAttribArray(2); // Size
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));

        glBindVertexArray(0);

        // Shader de partcula (Simples Point Sprite)
        const char* vSrc = "#version 450 core\n layout(location=0) in vec3 pos; layout(location=1) in vec4 col; layout(location=2) in float sz; uniform mat4 vp; out vec4 vCol; void main() { vCol = col; gl_PointSize = sz; gl_Position = vp * vec4(pos, 1.0); }";
        const char* fSrc = "#version 450 core\n in vec4 vCol; out vec4 frag; void main() { frag = vCol; }";
        m_shader = std::make_shared<Shader>(vSrc, fSrc);
    }

    ParticleSystem::~ParticleSystem() {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
    }

    void ParticleSystem::Emit(const glm::vec3& pos, const glm::vec3& vel, const glm::vec4& color, float life, float size) {
        if (m_activeCount >= m_maxParticles) return;
        auto& p = m_particles[m_activeCount++];
        p.position = pos;
        p.velocity = vel;
        p.force = {0,0,0};
        p.color = color;
        p.life = life;
        p.maxLife = life;
        p.size = size;
        p.density = 0.1f;
        p.pressure = 0.0f;
    }

    void ParticleSystem::Update(float dt) {
        if (m_activeCount == 0) return;

        SimulateSPH(dt);

        // Cleanup
        for (uint32_t i = 0; i < m_activeCount; ++i) {
            if (m_particles[i].life <= 0.0f) {
                m_particles[i] = m_particles[m_activeCount - 1];
                m_activeCount--;
                i--;
            }
        }

        // Buffer update
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_activeCount * sizeof(Particle), m_particles.data());
    }

    void ParticleSystem::SimulateSPH(float dt) {
        // Parallel SPH Simulation using wi::jobsystem
        wi::jobsystem::context ctx;
        int numChunks = 4;
        int chunkSize = m_activeCount / numChunks;
        if (chunkSize < 1) { numChunks = 1; chunkSize = m_activeCount; }

        for (int i = 0; i < numChunks; ++i) {
            int start = i * chunkSize;
            int end = (i == numChunks - 1) ? m_activeCount : (i + 1) * chunkSize;
            
            wi::jobsystem::Execute(ctx, [this, start, end, dt](wi::jobsystem::JobArgs args) {
                (void)args;
                int j = start;
                for (; j < end - 1; j += 2) {
                    auto& p1 = m_particles[j];
                    auto& p2 = m_particles[j + 1];
                    p1.life -= dt;
                    p2.life -= dt;

                    // AVX2-FMA Vectorized position & velocity update for 2 particles at once
                    __m256 pos = _mm256_setr_ps(
                        p1.position.x, p1.position.y, p1.position.z, 0.0f,
                        p2.position.x, p2.position.y, p2.position.z, 0.0f
                    );
                    __m256 vel = _mm256_setr_ps(
                        p1.velocity.x, p1.velocity.y, p1.velocity.z, 0.0f,
                        p2.velocity.x, p2.velocity.y, p2.velocity.z, 0.0f
                    );
                    __m256 grav = _mm256_setr_ps(
                        0.0f, -9.81f, 0.0f, 0.0f,
                        0.0f, -9.81f, 0.0f, 0.0f
                    );
                    __m256 dt_vec = _mm256_set1_ps(dt);

                    // vel = vel + grav * dt
                    vel = _mm256_fmadd_ps(grav, dt_vec, vel);
                    // pos = pos + vel * dt
                    pos = _mm256_fmadd_ps(vel, dt_vec, pos);

                    alignas(32) float temp_pos[8];
                    alignas(32) float temp_vel[8];
                    _mm256_store_ps(temp_pos, pos);
                    _mm256_store_ps(temp_vel, vel);

                    p1.position.x = temp_pos[0];
                    p1.position.y = temp_pos[1];
                    p1.position.z = temp_pos[2];

                    p1.velocity.x = temp_vel[0];
                    p1.velocity.y = temp_vel[1];
                    p1.velocity.z = temp_vel[2];

                    p2.position.x = temp_pos[4];
                    p2.position.y = temp_pos[5];
                    p2.position.z = temp_pos[6];

                    p2.velocity.x = temp_vel[4];
                    p2.velocity.y = temp_vel[5];
                    p2.velocity.z = temp_vel[6];

                    // Simple wall bounce
                    if (p1.position.y < 0.0f) {
                        p1.position.y = 0.01f;
                        p1.velocity.y *= -0.4f;
                    }
                    p1.color.a = p1.life / p1.maxLife;

                    if (p2.position.y < 0.0f) {
                        p2.position.y = 0.01f;
                        p2.velocity.y *= -0.4f;
                    }
                    p2.color.a = p2.life / p2.maxLife;
                }

                // Remainder particle if odd count
                for (; j < end; ++j) {
                    auto& p = m_particles[j];
                    p.life -= dt;
                    p.velocity.y += -9.81f * dt;
                    p.position += p.velocity * dt;
                    if (p.position.y < 0.0f) {
                        p.position.y = 0.01f;
                        p.velocity.y *= -0.4f;
                    }
                    p.color.a = p.life / p.maxLife;
                }
            });
        }
        wi::jobsystem::Wait(ctx);
    }

    void ParticleSystem::Render(const glm::mat4& view, const glm::mat4& projection) {
        if (m_activeCount == 0) return;
        m_shader->Use();
        m_shader->SetMat4U("vp", projection * view);
        
        glEnable(GL_PROGRAM_POINT_SIZE);
        glBindVertexArray(m_vao);
        glDrawArrays(GL_POINTS, 0, m_activeCount);
        glBindVertexArray(0);
    }
}
