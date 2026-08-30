#pragma once
#include "core/Memory/Allocator.hpp"

namespace starlight {

/**
 * @brief High-performance allocator for many fixed-size objects.
 *
 * Uses a singly-linked free list built over one contiguous block. Allocation
 * and deallocation are O(1) and produce zero fragmentation. Exposes optional
 * internal locking (default off) for single-threaded hot paths; enable
 * `threadSafe` when slots are shared across worker threads.
 */
class PoolAllocator : public IAllocator {
public:
    PoolAllocator(size_t objectSize, size_t objectCount, IAllocator* backing = nullptr, bool threadSafe = false);
    ~PoolAllocator() override;

    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator& operator=(const PoolAllocator&) = delete;

    void* Allocate(size_t size, size_t alignment = kDefaultAlignment) override;
    void Deallocate(void* ptr) override;
    void Clear() override;

    size_t GetObjectSize() const {
        return m_objectSize;
    }
    size_t GetCapacity() const {
        return m_objectCount;
    }
    size_t GetAvailableCount() const;

private:
    struct Node {
        Node* next;
    };

    void RebuildFreeList();

    void* m_start = nullptr;
    void* m_rawStart = nullptr;
    void* m_backingRaw = nullptr;
    IAllocator* m_backing = nullptr;
    size_t m_objectSize = 0;
    size_t m_objectCount = 0;
    size_t m_totalSize = 0;
    Node* m_freeList = nullptr;
    bool m_ownsMemory = false;
    bool m_threadSafe = false;
    void* m_lock = nullptr;
};

}  // namespace starlight
