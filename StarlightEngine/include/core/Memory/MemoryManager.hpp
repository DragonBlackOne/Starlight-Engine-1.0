#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include "core/Memory/LinearAllocator.hpp"

namespace starlight {

/**
 * @brief Global per-thread frame allocator registry.
 *
 * Each thread lazily gets its own frame LinearAllocator (default 4 MB) with a
 * thread_local fast path. ClearFrame() resets all registered thread allocators.
 * The registry itself is mutex-protected; the allocation fast path is lock-free.
 */
class MemoryManager {
public:
    static void Initialize();
    static void Shutdown();

    static void* AllocateFrame(size_t size, size_t alignment = 8);
    static void ClearFrame();

    static MemoryManager& Get() {
        return *s_instance;
    }

    /**
     * @brief Access this thread's frame allocator (creating it if needed).
     */
    static LinearAllocator* GetThreadAllocator();

    size_t GetTotalUsedMemory() const;
    size_t GetTotalCapacity() const;
    size_t GetAllocatorCount() const;

private:
    MemoryManager();
    ~MemoryManager();

    static MemoryManager* s_instance;

    // Registry for all thread-local allocators to allow global Clear()
    std::vector<std::unique_ptr<LinearAllocator>> m_threadAllocators;
    std::mutex m_registryMutex;

    // Fast thread-local access
    static thread_local LinearAllocator* s_tlsAllocator;
};

}  // namespace starlight
