#include "core/Memory/StackAllocator.hpp"
#include <algorithm>
#include <cstdlib>
#include "Log.hpp"
#include "core/SIMDUtils.hpp"

namespace starlight {

StackAllocator::StackAllocator(size_t size, IAllocator* backing) : m_backing(backing), m_size(size) {
    const size_t align = simd::kAlignmentAVX;
    if (m_backing) {
        m_rawStart = m_backing->Allocate(size + align, align);
        m_ownsMemory = false;
    } else {
        m_rawStart = std::malloc(size + align);
        m_ownsMemory = true;
    }
    if (!m_rawStart) {
        Log::Error("StackAllocator: Failed to allocate {} bytes!", size);
        m_start = nullptr;
        return;
    }
    m_start = reinterpret_cast<void*>(simd::AlignUpAddr(reinterpret_cast<uintptr_t>(m_rawStart), align));
    m_offset = 0;
    m_top = nullptr;
}

StackAllocator::~StackAllocator() {
    if (m_ownsMemory) {
        std::free(m_rawStart);
    } else if (m_rawStart && m_backing) {
        m_backing->Deallocate(m_rawStart);
    }
}

void* StackAllocator::Allocate(size_t size, size_t alignment) {
    if (!m_start) {
        return nullptr;
    }
    const size_t align = std::max(alignment, sizeof(void*));
    const uintptr_t base = reinterpret_cast<uintptr_t>(m_start);

    // Place the header first (aligned to pointer size), then align the payload.
    size_t headerOffset = AlignUp(m_offset, sizeof(void*));
    size_t payloadOffset = AlignUp(headerOffset + sizeof(Block), align);

    if (payloadOffset + size > m_size) {
        return nullptr;
    }

    Block* block = reinterpret_cast<Block*>(base + headerOffset);
    block->prev = m_top;
    block->prevEnd = m_offset;
    block->payload = payloadOffset;
    m_top = block;

    m_offset = payloadOffset + size;
    return reinterpret_cast<void*>(base + payloadOffset);
}

void StackAllocator::Deallocate(void* ptr) {
    if (!ptr || !m_top) {
        return;
    }
    const uintptr_t base = reinterpret_cast<uintptr_t>(m_start);
    const uintptr_t payloadAddr = base + m_top->payload;
    if (reinterpret_cast<uintptr_t>(ptr) != payloadAddr) {
        // Only the top (most recent) block can be freed individually.
        return;
    }
    m_offset = m_top->prevEnd;
    m_top = m_top->prev;
}

void StackAllocator::Clear() {
    m_offset = 0;
    m_top = nullptr;
}

}  // namespace starlight
