#pragma once
#include "RHICommon.hpp"
#include "RHIBuffer.hpp"
#include "RHITexture.hpp"
#include "RHIShader.hpp"
#include "RHIPipeline.hpp"
#include "RHICommandBuffer.hpp"
#include <memory>
#include <vector>

namespace starlight::rhi {

struct RenderDeviceDesc {
    BackendAPI api = BackendAPI::OpenGL;
    bool enableValidationLayers = false;
    bool enableDebugMarkers = true;
    void* nativeWindowHandle = nullptr;
};

class IRHIDevice {
public:
    virtual ~IRHIDevice() = default;

    virtual BackendAPI GetAPI() const = 0;
    virtual const char* GetDeviceName() const = 0;

    virtual std::shared_ptr<IRHIBuffer> CreateBuffer(const BufferDesc& desc) = 0;
    virtual std::shared_ptr<IRHITexture> CreateTexture(const TextureDesc& desc) = 0;
    virtual std::shared_ptr<IRHIShader> CreateShader(const ShaderDesc& desc) = 0;
    virtual std::shared_ptr<IRHIPipeline> CreatePipeline(const PipelineDesc& desc) = 0;
    virtual std::shared_ptr<IRHICommandBuffer> CreateCommandBuffer() = 0;

    virtual void Submit(std::shared_ptr<IRHICommandBuffer> cmdBuffer) = 0;
    virtual void WaitIdle() = 0;

    static std::unique_ptr<IRHIDevice> Create(const RenderDeviceDesc& desc);
};

} // namespace starlight::rhi
