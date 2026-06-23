#pragma once
#include "RenderGraph.hpp"
#include "GLResource.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace starlight {

    class TAAPass : public RenderGraphPass {
    public:
        TAAPass();
        ~TAAPass();

        void Setup(RenderGraphBuilder& builder) override { (void)builder; }
        void Execute(RenderGraphBlackboard& blackboard, const RenderGraphResources& resources) override;
        const char* GetName() const override { return "TAAPass"; }

        void OnResize(uint32_t width, uint32_t height);
        glm::vec2 GetJitterOffset() const;
        void ApplyJitter(glm::mat4& projection, uint32_t width, uint32_t height);

        uint32_t GetHistoryTexture() const { return m_historyTextures[m_historySourceIndex]; }

    private:
        uint32_t m_width = 0;
        uint32_t m_height = 0;
        uint32_t m_frameCount = 0;

        // Double history buffers for ping-pong feedback
        uint32_t m_historyFBOs[2] = { 0, 0 };
        uint32_t m_historyTextures[2] = { 0, 0 };
        uint32_t m_historySourceIndex = 0; // 0 or 1

        std::shared_ptr<Shader> m_taaShader;
        glm::mat4 m_prevViewProj = glm::mat4(1.0f);
        bool m_firstFrame = true;

        void InitBuffers(uint32_t width, uint32_t height);
        void DestroyBuffers();
    };

}
