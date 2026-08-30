#pragma once
#include <memory>
#include "core/Memory/LinearAllocator.hpp"

namespace starlight {

/**
 * @brief Double-buffered frame allocator.
 *
 * Provides two bump allocators that alternate every frame. Populating frame N
 * happens while frame N-1 is still being read, so the transient data for the
 * next frame never overwrites what the renderer is still consuming. The classic
 * "frame cache" pattern used by AAA engines for per-frame scratch data.
 */
class DoubleBufferAllocator {
public:
    explicit DoubleBufferAllocator(size_t bufferSize, IAllocator* backing = nullptr);
    ~DoubleBufferAllocator();

    DoubleBufferAllocator(const DoubleBufferAllocator&) = delete;
    DoubleBufferAllocator& operator=(const DoubleBufferAllocator&) = delete;

    /**
     * @brief Allocate from the currently active buffer.
     * @return nullptr if the active buffer is exhausted (call Swap() and retry).
     */
    void* Allocate(size_t size, size_t alignment = IAllocator::kDefaultAlignment);

    /**
     * @brief Swap buffers at the end of a frame, resetting the now-free buffer.
     */
    void Swap();

    /**
     * @brief Convenience: allocate then swap, a common per-frame pattern.
     */
    void* AllocateFrame(size_t size, size_t alignment = IAllocator::kDefaultAlignment) {
        void* ptr = Allocate(size, alignment);
        Swap();
        return ptr;
    }

    LinearAllocator& GetActiveBuffer() {
        return *m_buffers[m_active];
    }
    const LinearAllocator& GetActiveBuffer() const {
        return *m_buffers[m_active];
    }

    size_t GetActiveUsed() const {
        return m_buffers[m_active]->GetUsedMemory();
    }

    size_t GetInactiveUsed() const {
        return m_buffers[1 - m_active]->GetUsedMemory();
    }

private:
    std::unique_ptr<LinearAllocator> m_buffers[2];
    uint32_t m_active = 0;
};

}  // namespace starlight
