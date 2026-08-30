#pragma once
#include "RHICommon.hpp"

namespace starlight::rhi {

struct BufferDesc {
    BufferType type = BufferType::Vertex;
    BufferUsage usage = BufferUsage::Static;
    size_t size = 0;
    size_t stride = 0;
    const void* initialData = nullptr;
    std::string debugName = "";
};

class IRHIBuffer {
public:
    virtual ~IRHIBuffer() = default;

    virtual void Update(const void* data, size_t size, size_t offset = 0) = 0;
    virtual void* Map() = 0;
    virtual void Unmap() = 0;

    virtual BufferType GetType() const = 0;
    virtual BufferUsage GetUsage() const = 0;
    virtual size_t GetSize() const = 0;
    virtual size_t GetStride() const = 0;
    virtual uint64_t GetNativeHandle() const = 0;
};

} // namespace starlight::rhi
