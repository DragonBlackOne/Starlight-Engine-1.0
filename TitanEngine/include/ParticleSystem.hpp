// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "Shader.hpp"

namespace titan {
    struct Particle {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 force;
        glm::vec4 color;
        float life;
        float maxLife;
        float size;
        float density;
        float pressure;
    };

    class ParticleSystem {
    public:
        ParticleSystem(uint32_t maxParticles = 5000);
        ~ParticleSystem();

        void Emit(const glm::vec3& pos, const glm::vec3& vel, const glm::vec4& color, float life, float size);
        void Update(float dt);
        void Render(const glm::mat4& view, const glm::mat4& projection);

    private:
        std::vector<Particle> m_particles;
        uint32_t m_activeCount = 0;
        uint32_t m_maxParticles;
        uint32_t m_vao, m_vbo;
        std::shared_ptr<Shader> m_shader;

        void SimulateSPH(float dt);
    };
}
