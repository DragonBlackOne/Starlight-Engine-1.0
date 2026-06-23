#include "GPUCullingSystem.hpp"
#include "Log.hpp"

namespace starlight {

    bool GPUCullingSystem::OnInitialize(const EngineContext& context) {
        (void)context;
        m_cullingShader = std::make_shared<ComputeShader>("assets/shaders/compute/culling.comp");
        
        // Initialize buffers...
        glGenBuffers(1, &m_instanceSSBO);
        glGenBuffers(1, &m_indirectBuffer);
        glGenBuffers(1, &m_visibleInstancesSSBO);

        Log::Info("GPUCullingSystem: GPU-Driven Pipeline Initialized (Phase 16).");
        return true;
    }

    void GPUCullingSystem::OnShutdown() {
        glDeleteBuffers(1, &m_instanceSSBO);
        glDeleteBuffers(1, &m_indirectBuffer);
        glDeleteBuffers(1, &m_visibleInstancesSSBO);
    }

    void GPUCullingSystem::OnUpdate(float dt) {
        (void)dt;
    }

    void GPUCullingSystem::Cull(uint32_t instanceCount, const glm::mat4& viewProj) {
        (void)viewProj;
        if (instanceCount == 0) return;

        m_cullingShader->Use();
        // Set uniforms (frustum planes, etc.)
        
        // Bind Buffers
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_instanceSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_visibleInstancesSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_indirectBuffer);

        uint32_t numGroups = (instanceCount + 63) / 64;
        m_cullingShader->Dispatch(numGroups, 1, 1);
        m_cullingShader->Wait();
    }

}
