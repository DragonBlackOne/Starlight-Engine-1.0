#include "Memory.hpp"
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <thread>
#include "Log.hpp"

namespace starlight {

// --- Linear Allocator ---

LinearAllocator::LinearAllocator(size_t size) : m_size(size) {
    m_start = std::malloc(size);
    if (!m_start) {
        Log::Error("LinearAllocator: Failed to allocate {} bytes!", size);
    }
    m_offset = 0;
}

LinearAllocator::~LinearAllocator() {
    std::free(m_start);
}

void* LinearAllocator::Allocate(size_t size, size_t alignment) {
    uintptr_t currentAddr = reinterpret_cast<uintptr_t>(m_start) + m_offset;
    uintptr_t padding = (alignment - (currentAddr % alignment)) % alignment;

    if (m_offset + padding + size > m_size) {
        return nullptr;
    }

    m_offset += (size_t)padding;
    void* ptr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_start) + m_offset);
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
    if (!m_start) {
        Log::Error("PoolAllocator: Failed to allocate {} bytes!", m_totalSize);
    }
    Clear();
}

PoolAllocator::~PoolAllocator() {
    std::free(m_start);
}

void* PoolAllocator::Allocate(size_t size, size_t alignment) {
    (void)size;
    (void)alignment;
    if (!m_freeList)
        return nullptr;

    void* ptr = m_freeList;
    m_freeList = m_freeList->next;
    return ptr;
}

void PoolAllocator::Deallocate(void* ptr) {
    if (!ptr)
        return;
    uintptr_t startAddr = reinterpret_cast<uintptr_t>(m_start);
    uintptr_t endAddr = startAddr + m_totalSize;
    uintptr_t ptrAddr = reinterpret_cast<uintptr_t>(ptr);
    if (ptrAddr < startAddr || ptrAddr >= endAddr) {
        Log::Error("PoolAllocator::Deallocate: Attempted to deallocate pointer {} outside pool boundaries!", ptr);
        return;
    }

    Node* node = static_cast<Node*>(ptr);
    node->next = m_freeList;
    m_freeList = node;
}

void PoolAllocator::Clear() {
    m_freeList = static_cast<Node*>(m_start);
    Node* curr = m_freeList;
    for (size_t i = 0; i < (m_totalSize / m_objectSize) - 1; ++i) {
        curr->next = reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(curr) + m_objectSize);
        curr = curr->next;
    }
    curr->next = nullptr;
}

// --- Memory Manager ---
MemoryManager* MemoryManager::s_instance = nullptr;
thread_local LinearAllocator* MemoryManager::s_tlsAllocator = nullptr;

MemoryManager::MemoryManager() {}
MemoryManager::~MemoryManager() {}

void MemoryManager::Initialize() {
    s_instance = new MemoryManager();
    Log::Info("MemoryManager: Subsystem initialized with Thread-Local fast-paths.");
}

void MemoryManager::Shutdown() {
    delete s_instance;
    s_instance = nullptr;
}

void* MemoryManager::AllocateFrame(size_t size, size_t alignment) {
    if (!s_instance)
        return nullptr;
    if (!s_tlsAllocator) {
        // Register new allocator for this thread
        std::lock_guard<std::mutex> lock(s_instance->m_registryMutex);
        auto allocator = std::make_unique<LinearAllocator>(4 * 1024 * 1024);  // 4MB per thread
        s_tlsAllocator = allocator.get();
        s_instance->m_threadAllocators.push_back(std::move(allocator));
        Log::Info("MemoryManager: Registered new LinearAllocator for Thread ID {}.",
            (size_t)std::hash<std::thread::id>{}(std::this_thread::get_id()));
    }

    return s_tlsAllocator->Allocate(size, alignment);
}

void MemoryManager::ClearFrame() {
    if (!s_instance)
        return;
    std::lock_guard<std::mutex> lock(s_instance->m_registryMutex);
    for (auto& allocator : s_instance->m_threadAllocators) {
        allocator->Clear();
    }
}

}  // namespace starlight
