#pragma once
#include "RHICommon.hpp"
#include "RHIShader.hpp"
#include <memory>

namespace starlight::rhi {

struct RasterizerState {
    PolygonMode polygonMode = PolygonMode::Fill;
    CullMode cullMode = CullMode::Back;
    FrontFace frontFace = FrontFace::CounterClockwise;
    bool depthClampEnable = false;
    bool depthBiasEnable = false;
    float depthBiasConstant = 0.0f;
    float depthBiasSlope = 0.0f;
};

struct DepthStencilState {
    bool depthTestEnable = true;
    bool depthWriteEnable = true;
    CompareOp depthCompareOp = CompareOp::Less;
    bool stencilTestEnable = false;
};

struct BlendAttachmentState {
    bool blendEnable = false;
    BlendFactor srcColorBlendFactor = BlendFactor::SrcAlpha;
    BlendFactor dstColorBlendFactor = BlendFactor::OneMinusSrcAlpha;
    BlendOp colorBlendOp = BlendOp::Add;
    BlendFactor srcAlphaBlendFactor = BlendFactor::One;
    BlendFactor dstAlphaBlendFactor = BlendFactor::Zero;
    BlendOp alphaBlendOp = BlendOp::Add;
};

struct BlendState {
    bool alphaToCoverageEnable = false;
    std::vector<BlendAttachmentState> attachments{ BlendAttachmentState{} };
};

struct PipelineDesc {
    std::shared_ptr<IRHIShader> shader;
    PrimitiveTopology topology = PrimitiveTopology::Triangles;
    RasterizerState rasterizer{};
    DepthStencilState depthStencil{};
    BlendState blend{};
    std::string debugName = "";
};

class IRHIPipeline {
public:
    virtual ~IRHIPipeline() = default;

    virtual void Bind() = 0;
    virtual const PipelineDesc& GetDesc() const = 0;
    virtual uint64_t GetNativeHandle() const = 0;
};

} // namespace starlight::rhi
