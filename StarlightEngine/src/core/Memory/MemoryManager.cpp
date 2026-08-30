#include "core/Memory/MemoryManager.hpp"
#include <functional>
#include <thread>
#include "Log.hpp"

namespace starlight {

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
    s_tlsAllocator = nullptr;
}

LinearAllocator* MemoryManager::GetThreadAllocator() {
    if (!s_instance || s_tlsAllocator) {
        return s_tlsAllocator;
    }
    std::lock_guard<std::mutex> lock(s_instance->m_registryMutex);
    auto allocator = std::make_unique<LinearAllocator>(4 * 1024 * 1024);  // 4MB per thread
    s_tlsAllocator = allocator.get();
    s_instance->m_threadAllocators.push_back(std::move(allocator));
    Log::Info("MemoryManager: Registered new LinearAllocator for Thread ID {}.",
        (size_t)std::hash<std::thread::id>{}(std::this_thread::get_id()));
    return s_tlsAllocator;
}

void* MemoryManager::AllocateFrame(size_t size, size_t alignment) {
    LinearAllocator* alloc = GetThreadAllocator();
    if (!alloc) {
        return nullptr;
    }
    return alloc->Allocate(size, alignment);
}

void MemoryManager::ClearFrame() {
    if (!s_instance) {
        return;
    }
    std::lock_guard<std::mutex> lock(s_instance->m_registryMutex);
    for (auto& allocator : s_instance->m_threadAllocators) {
        allocator->Clear();
    }
}

size_t MemoryManager::GetTotalUsedMemory() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_registryMutex));
    size_t total = 0;
    for (const auto& alloc : m_threadAllocators) {
        if (alloc) {
            total += alloc->GetUsedMemory();
        }
    }
    return total;
}

size_t MemoryManager::GetTotalCapacity() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_registryMutex));
    size_t total = 0;
    for (const auto& alloc : m_threadAllocators) {
        if (alloc) {
            total += alloc->GetTotalSize();
        }
    }
    return total;
}

size_t MemoryManager::GetAllocatorCount() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_registryMutex));
    return m_threadAllocators.size();
}

}  // namespace starlight
