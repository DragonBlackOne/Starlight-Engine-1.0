#pragma once
#include "core/Memory/Allocator.hpp"

namespace starlight {

/**
 * @brief Fast, sequential bump allocator.
 *
 * Ideal for frame-scoped or temporary data: allocation is a single offset bump,
 * deallocation is a no-op, and Clear() rewinds the bump pointer. Backing memory
 * is a single contiguous block acquired from the CRT by default (or from a
 * supplied parent IAllocator for chained arenas).
 */
class LinearAllocator : public IAllocator {
public:
    explicit LinearAllocator(size_t size, IAllocator* backing = nullptr);
    ~LinearAllocator() override;

    LinearAllocator(const LinearAllocator&) = delete;
    LinearAllocator& operator=(const LinearAllocator&) = delete;

    void* Allocate(size_t size, size_t alignment = kDefaultAlignment) override;
    void Deallocate(void* ptr) override {
        // No-op for a linear allocator.
        (void)ptr;
    }
    void Clear() override;

    size_t GetUsedMemory() const {
        return m_offset;
    }
    size_t GetTotalSize() const {
        return m_size;
    }
    size_t GetRemainingMemory() const {
        return m_size - m_offset;
    }
    bool InBounds(const void* ptr) const;

private:
    void* m_start = nullptr;
    void* m_rawStart = nullptr;
    IAllocator* m_backing = nullptr;
    size_t m_size = 0;
    size_t m_offset = 0;
    bool m_ownsMemory = false;
};

}  // namespace starlight
