#pragma once
#include "RHICommon.hpp"

namespace starlight::rhi {

struct SamplerDesc {
    TextureFilter minFilter = TextureFilter::Linear;
    TextureFilter magFilter = TextureFilter::Linear;
    TextureFilter mipmapMode = TextureFilter::Linear;
    TextureWrap addressModeU = TextureWrap::Repeat;
    TextureWrap addressModeV = TextureWrap::Repeat;
    TextureWrap addressModeW = TextureWrap::Repeat;
    float maxAnisotropy = 16.0f;
    bool enableAnisotropy = true;
    CompareOp compareOp = CompareOp::Never;
};

struct TextureDesc {
    TextureType type = TextureType::Texture2D;
    TextureFormat format = TextureFormat::RGBA8_UNORM;
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;
    uint32_t mipLevels = 1;
    uint32_t arrayLayers = 1;
    SamplerDesc sampler{};
    const void* initialData = nullptr;
    std::string debugName = "";
};

class IRHITexture {
public:
    virtual ~IRHITexture() = default;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual uint32_t GetDepth() const = 0;
    virtual uint32_t GetMipLevels() const = 0;
    virtual TextureFormat GetFormat() const = 0;
    virtual TextureType GetType() const = 0;
    virtual uint64_t GetNativeHandle() const = 0;

    virtual void UpdateData(const void* data, uint32_t width, uint32_t height, uint32_t mipLevel = 0) = 0;
};

} // namespace starlight::rhi
