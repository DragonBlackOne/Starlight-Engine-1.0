// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <vector>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "ComputeShader.hpp"
#include "EngineSystem.hpp"

namespace starlight {

    struct GPUInstanceData {
        glm::mat4 model;
        glm::vec4 boundingSphere; // xyz: center, w: radius
    };

    struct DrawElementsIndirectCommand {
        uint32_t count;
        uint32_t instanceCount;
        uint32_t firstIndex;
        uint32_t baseVertex;
        uint32_t baseInstance;
    };

    /**
     * @brief GPUCullingSystem (Phase 13)
     * Performs frustum culling on the GPU for thousands of instances.
     */
    class GPUCullingSystem : public ISystem {
    public:
        // ISystem
        bool OnInitialize(const EngineContext& context) override;
        void OnShutdown() override;
        void OnUpdate(float dt) override;
        const char* GetName() const override { return "GPUCullingSystem"; }

        void Cull(uint32_t instanceCount, const glm::mat4& viewProj);

    private:
        std::shared_ptr<ComputeShader> m_cullingShader;
        uint32_t m_instanceSSBO = 0;
        uint32_t m_indirectBuffer = 0;
        uint32_t m_visibleInstancesSSBO = 0;
    };

}
