#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include <random>
#include "EngineSystem.hpp"
#include "Shader.hpp"
#include "ComputeShader.hpp"

namespace starlight {

    struct GPUParticle {
        glm::vec4 position; // xyz = pos, w = life
        glm::vec4 velocity; // xyz = vel, w = size
        glm::vec4 color;    // rgba
    };

    class VFXSystem : public ISystem {
    public:
        VFXSystem();
        ~VFXSystem();

        // ISystem
        bool OnInitialize(const EngineContext& context) override;
        void OnShutdown() override;
        void OnUpdate(float dt) override;
        void OnRender() override {}
        void RenderInternal();
        const char* GetName() const override { return "VFXSystem"; }
        bool IsMainThreadOnly() const override { return true; }

        // Industrial API
        void Emit(const glm::vec3& pos, const glm::vec3& velRange, const glm::vec4& color, int count = 1, float size = 0.1f);
        void SetGravity(const glm::vec3& gravity) { m_gravity = gravity; }
        void SetMaxParticles(int maxParticles) {
            if (maxParticles > MAX_PARTICLES) maxParticles = MAX_PARTICLES;
            if (maxParticles < 256) maxParticles = 256;
            m_maxParticles = maxParticles;
        }
        int GetMaxParticles() const { return m_maxParticles; }

    private:
        static const int MAX_PARTICLES = 256000;
        int m_maxParticles = 256000;
        
        uint32_t m_ssbo = 0;
        uint32_t m_vao = 0;
        
        std::shared_ptr<ComputeShader> m_computeShader;
        std::shared_ptr<Shader> m_renderShader;
        
        glm::vec3 m_gravity = {0.0f, -9.81f, 0.0f};
        bool m_initialized = false;

        // Emitter helper
        struct PendingEmission {
            glm::vec3 pos;
            glm::vec3 velRange;
            glm::vec4 color;
            int count;
            float size;
        };
        std::vector<PendingEmission> m_pending;

        std::mt19937 m_randomEngine;
        std::uniform_real_distribution<float> m_randomDistribution;
    };
}
