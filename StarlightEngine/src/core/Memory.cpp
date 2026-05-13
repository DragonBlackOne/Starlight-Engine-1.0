// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#include "Memory.hpp"
#include "Log.hpp"
#include <cstdlib>
#include <algorithm>

namespace starlight {

    // --- Linear Allocator ---

    LinearAllocator::LinearAllocator(size_t size) : m_size(size) {
        m_start = std::malloc(size);
        m_offset = 0;
    }

    LinearAllocator::~LinearAllocator() {
        std::free(m_start);
    }

    void* LinearAllocator::Allocate(size_t size, size_t alignment) {
        size_t currentAddr = reinterpret_cast<size_t>(m_start) + m_offset;
        size_t padding = (alignment - (currentAddr % alignment)) % alignment;
        
        if (m_offset + padding + size > m_size) {
            return nullptr;
        }

        m_offset += padding;
        void* ptr = reinterpret_cast<void*>(reinterpret_cast<size_t>(m_start) + m_offset);
        m_offset += size;
        return ptr;
    }

    void LinearAllocator::Clear() {
        m_offset = 0;
    }

    // --- Pool Allocator ---

    PoolAllocator::PoolAllocator(size_t objectSize, size_t objectCount) 
        : m_objectSize(std::max(objectSize, sizeof(Node))), m_totalSize(m_objectSize * objectCount) {
        
        m_start = std::malloc(m_totalSize);
        Clear();
    }

    PoolAllocator::~PoolAllocator() {
        std::free(m_start);
    }

    void* PoolAllocator::Allocate(size_t size, size_t alignment) {
        (void)size; (void)alignment;
        if (!m_freeList) return nullptr;

        void* ptr = m_freeList;
        m_freeList = m_freeList->next;
        return ptr;
    }

    void PoolAllocator::Deallocate(void* ptr) {
        Node* node = static_cast<Node*>(ptr);
        node->next = m_freeList;
        m_freeList = node;
    }

    void PoolAllocator::Clear() {
        m_freeList = static_cast<Node*>(m_start);
        Node* curr = m_freeList;
        for (size_t i = 0; i < (m_totalSize / m_objectSize) - 1; ++i) {
            curr->next = reinterpret_cast<Node*>(reinterpret_cast<size_t>(curr) + m_objectSize);
            curr = curr->next;
        }
        curr->next = nullptr;
    }

    // --- Memory Manager ---

    MemoryManager* MemoryManager::s_instance = nullptr;

    void MemoryManager::Initialize() {
        s_instance = new MemoryManager();
        s_instance->m_frameAllocator = std::make_unique<LinearAllocator>(16 * 1024 * 1024); // 16MB per frame
        Log::Info("MemoryManager: Subsystem initialized (16MB Frame Budget).");
    }

    void MemoryManager::Shutdown() {
        delete s_instance;
        s_instance = nullptr;
    }

    void* MemoryManager::AllocateFrame(size_t size, size_t alignment) {
        std::lock_guard<std::mutex> lock(s_instance->m_mutex);
        return s_instance->m_frameAllocator->Allocate(size, alignment);
    }

    void MemoryManager::ClearFrame() {
        std::lock_guard<std::mutex> lock(s_instance->m_mutex);
        s_instance->m_frameAllocator->Clear();
    }

}
