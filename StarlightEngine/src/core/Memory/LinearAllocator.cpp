#include "core/Memory/LinearAllocator.hpp"
#include <cstdlib>
#include "Log.hpp"
#include "core/SIMDUtils.hpp"

namespace starlight {

LinearAllocator::LinearAllocator(size_t size, IAllocator* backing) : m_backing(backing), m_size(size) {
    const size_t align = simd::kAlignmentAVX;
    if (m_backing) {
        m_rawStart = m_backing->Allocate(size + align, align);
        m_ownsMemory = false;
    } else {
        m_rawStart = std::malloc(size + align);
        m_ownsMemory = true;
    }
    if (!m_rawStart) {
        Log::Error("LinearAllocator: Failed to allocate {} bytes!", size);
        m_start = nullptr;
        return;
    }
    // Align the usable base to AVX (32) for predictable SIMD-friendly blocks.
    m_start = reinterpret_cast<void*>(simd::AlignUpAddr(reinterpret_cast<uintptr_t>(m_rawStart), align));
    m_offset = 0;
}

LinearAllocator::~LinearAllocator() {
    if (m_ownsMemory) {
        std::free(m_rawStart);
    } else if (m_rawStart && m_backing) {
        m_backing->Deallocate(m_rawStart);
    }
}

void* LinearAllocator::Allocate(size_t size, size_t alignment) {
    if (!m_start) {
        return nullptr;
    }
    const size_t padding = ComputePadding(reinterpret_cast<uintptr_t>(m_start), m_offset, alignment);
    const size_t newOffset = m_offset + padding;
    if (newOffset + size > m_size) {
        return nullptr;
    }
    m_offset = newOffset;
    void* ptr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_start) + m_offset);
    m_offset += size;
    return ptr;
}

void LinearAllocator::Clear() {
    m_offset = 0;
}

bool LinearAllocator::InBounds(const void* ptr) const {
    const uintptr_t p = reinterpret_cast<uintptr_t>(ptr);
    const uintptr_t start = reinterpret_cast<uintptr_t>(m_start);
    return p >= start && p < start + m_size;
}

}  // namespace starlight
