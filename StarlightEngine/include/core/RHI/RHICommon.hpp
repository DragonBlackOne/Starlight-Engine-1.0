#pragma once
#include <cstdint>
#include <cstddef>
#include <string>

namespace starlight::rhi {

enum class BackendAPI {
    OpenGL,
    Vulkan,
    DirectX12,
    WebGPU
};

enum class BufferType {
    Vertex,
    Index,
    Uniform,
    Storage, // SSBO
    Indirect
};

enum class BufferUsage {
    Static,
    Dynamic,
    Stream,
    Staging
};

enum class TextureFormat {
    R8_UNORM,
    RG8_UNORM,
    RGB8_UNORM,
    RGBA8_UNORM,
    RGBA8_SRGB,
    RGBA16_FLOAT,
    RGBA32_FLOAT,
    R32_FLOAT,
    RG32_FLOAT,
    RGB32_FLOAT,
    D24_UNORM_S8_UINT,
    D32_FLOAT
};

enum class TextureType {
    Texture2D,
    Texture3D,
    TextureCube,
    Texture2DArray
};

enum class TextureFilter {
    Nearest,
    Linear,
    Anisotropic
};

enum class TextureWrap {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder
};

enum class PrimitiveTopology {
    Triangles,
    TriangleStrip,
    Lines,
    LineStrip,
    Points
};

enum class CullMode {
    None,
    Front,
    Back
};

enum class FrontFace {
    CounterClockwise,
    Clockwise
};

enum class PolygonMode {
    Fill,
    Line,
    Point
};

enum class CompareOp {
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always
};

enum class BlendFactor {
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha
};

enum class BlendOp {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max
};

struct Viewport {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    float minDepth = 0.0f;
    float maxDepth = 1.0f;
};

struct ScissorRect {
    int32_t x = 0;
    int32_t y = 0;
    uint32_t width = 0;
    uint32_t height = 0;
};

} // namespace starlight::rhi
