#include "core/Memory/DoubleBufferAllocator.hpp"

namespace starlight {

DoubleBufferAllocator::DoubleBufferAllocator(size_t bufferSize, IAllocator* backing) {
    m_buffers[0] = std::make_unique<LinearAllocator>(bufferSize, backing);
    m_buffers[1] = std::make_unique<LinearAllocator>(bufferSize, backing);
    m_active = 0;
}

DoubleBufferAllocator::~DoubleBufferAllocator() = default;

void* DoubleBufferAllocator::Allocate(size_t size, size_t alignment) {
    return m_buffers[m_active]->Allocate(size, alignment);
}

void DoubleBufferAllocator::Swap() {
    m_buffers[1 - m_active]->Clear();
    m_active = 1 - m_active;
}

}  // namespace starlight
