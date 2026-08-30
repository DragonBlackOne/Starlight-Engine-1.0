#pragma once
#include "RHICommon.hpp"
#include "RHIBuffer.hpp"
#include "RHITexture.hpp"
#include "RHIPipeline.hpp"
#include <memory>

namespace starlight::rhi {

class IRHICommandBuffer {
public:
    virtual ~IRHICommandBuffer() = default;

    virtual void Begin() = 0;
    virtual void End() = 0;

    virtual void SetViewport(const Viewport& viewport) = 0;
    virtual void SetScissor(const ScissorRect& scissor) = 0;

    virtual void BindPipeline(std::shared_ptr<IRHIPipeline> pipeline) = 0;
    virtual void BindVertexBuffer(std::shared_ptr<IRHIBuffer> buffer, uint32_t binding = 0, size_t offset = 0) = 0;
    virtual void BindIndexBuffer(std::shared_ptr<IRHIBuffer> buffer, size_t offset = 0) = 0;
    virtual void BindUniformBuffer(std::shared_ptr<IRHIBuffer> buffer, uint32_t slot) = 0;
    virtual void BindTexture(std::shared_ptr<IRHITexture> texture, uint32_t slot) = 0;

    virtual void Draw(uint32_t vertexCount, uint32_t startVertex = 0) = 0;
    virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndex = 0, uint32_t baseVertex = 0) = 0;
    virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex = 0, uint32_t startInstance = 0) = 0;
    virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex = 0, uint32_t baseVertex = 0, uint32_t startInstance = 0) = 0;

    virtual void DispatchCompute(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;
};

} // namespace starlight::rhi
