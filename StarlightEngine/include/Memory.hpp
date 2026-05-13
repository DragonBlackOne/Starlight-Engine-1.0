// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <mutex>

namespace starlight {

    /**
     * @brief Base interface for all custom allocators.
     */
    class IAllocator {
    public:
        virtual ~IAllocator() = default;
        virtual void* Allocate(size_t size, size_t alignment = 8) = 0;
        virtual void Deallocate(void* ptr) = 0;
        virtual void Clear() = 0;
    };

    /**
     * @brief Fast, sequential allocator. Good for frame-based or temporary data.
     */
    class LinearAllocator : public IAllocator {
    public:
        LinearAllocator(size_t size);
        ~LinearAllocator();

        void* Allocate(size_t size, size_t alignment = 8) override;
        void Deallocate(void* ptr) override { /* No-op for linear */ (void)ptr; }
        void Clear() override;

        size_t GetUsedMemory() const { return m_offset; }
        size_t GetTotalSize() const { return m_size; }

    private:
        void* m_start = nullptr;
        size_t m_size = 0;
        size_t m_offset = 0;
    };

    /**
     * @brief Allocator for fixed-size objects. High performance, zero fragmentation.
     */
    class PoolAllocator : public IAllocator {
    public:
        PoolAllocator(size_t objectSize, size_t objectCount);
        ~PoolAllocator();

        void* Allocate(size_t size, size_t alignment = 8) override;
        void Deallocate(void* ptr) override;
        void Clear() override;

    private:
        struct Node {
            Node* next;
        };

        void* m_start = nullptr;
        size_t m_objectSize = 0;
        size_t m_totalSize = 0;
        Node* m_freeList = nullptr;
    };

    /**
     * @brief Global Memory Manager for Phase 4 evolution.
     */
    class MemoryManager {
    public:
        static void Initialize();
        static void Shutdown();

        static void* AllocateFrame(size_t size, size_t alignment = 8);
        static void ClearFrame();

        static MemoryManager& Get() { return *s_instance; }

    private:
        static MemoryManager* s_instance;
        std::unique_ptr<LinearAllocator> m_frameAllocator;
        std::mutex m_mutex;
    };

}
