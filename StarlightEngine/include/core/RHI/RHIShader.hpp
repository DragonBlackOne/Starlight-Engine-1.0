#pragma once
#include "RHICommon.hpp"
#include <vector>

namespace starlight::rhi {

enum class ShaderStage {
    Vertex = 1 << 0,
    Fragment = 1 << 1,
    Geometry = 1 << 2,
    Compute = 1 << 3
};

struct ShaderStageDesc {
    ShaderStage stage;
    std::string source; // GLSL/HLSL/WGSL text or binary SPIR-V bytecode
    std::string entryPoint = "main";
};

struct ShaderDesc {
    std::vector<ShaderStageDesc> stages;
    std::string debugName = "";
};

class IRHIShader {
public:
    virtual ~IRHIShader() = default;

    virtual bool IsValid() const = 0;
    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual uint64_t GetNativeHandle() const = 0;

    virtual void SetUniformInt(const std::string& name, int value) = 0;
    virtual void SetUniformFloat(const std::string& name, float value) = 0;
    virtual void SetUniformVec2(const std::string& name, float x, float y) = 0;
    virtual void SetUniformVec3(const std::string& name, float x, float y, float z) = 0;
    virtual void SetUniformVec4(const std::string& name, float x, float y, float z, float w) = 0;
    virtual void SetUniformMat4(const std::string& name, const float* matrixData) = 0;
};

} // namespace starlight::rhi
