#include "core/Memory/PoolAllocator.hpp"
#include <algorithm>
#include <cstdlib>
#include "Log.hpp"
#include "core/SIMDUtils.hpp"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <pthread.h>
#endif

namespace starlight {

PoolAllocator::PoolAllocator(size_t objectSize, size_t objectCount, IAllocator* backing, bool threadSafe)
    : m_backing(backing)
    , m_objectSize(std::max(objectSize, sizeof(Node)))
    , m_objectCount(objectCount)
    , m_threadSafe(threadSafe) {
    const size_t align = simd::kAlignmentAVX;
    const size_t total = m_objectSize * objectCount + align;

    if (m_backing) {
        m_rawStart = m_backing->Allocate(total, align);
        m_ownsMemory = false;
    } else {
        m_rawStart = std::malloc(total);
        m_ownsMemory = true;
    }
    if (!m_rawStart) {
        Log::Error("PoolAllocator: Failed to allocate {} bytes!", total);
        m_start = nullptr;
        return;
    }
    m_start = reinterpret_cast<void*>(simd::AlignUpAddr(reinterpret_cast<uintptr_t>(m_rawStart), align));
    m_totalSize = m_objectSize * objectCount;

    if (m_threadSafe) {
#if defined(_WIN32)
        m_lock = new SRWLOCK();
        InitializeSRWLock(static_cast<SRWLOCK*>(m_lock));
#elif defined(__linux__) || defined(__APPLE__)
        m_lock = new pthread_mutex_t();
        pthread_mutex_init(static_cast<pthread_mutex_t*>(m_lock), nullptr);
#endif
    }
    RebuildFreeList();
}

PoolAllocator::~PoolAllocator() {
    if (m_threadSafe) {
#if defined(_WIN32)
        delete static_cast<SRWLOCK*>(m_lock);
#elif defined(__linux__) || defined(__APPLE__)
        pthread_mutex_destroy(static_cast<pthread_mutex_t*>(m_lock));
        delete static_cast<pthread_mutex_t*>(m_lock);
#endif
    }
    if (m_ownsMemory) {
        std::free(m_rawStart);
    } else if (m_rawStart && m_backing) {
        m_backing->Deallocate(m_rawStart);
    }
}

void PoolAllocator::RebuildFreeList() {
    const size_t count = m_objectCount;
    Node* prev = nullptr;
    Node* head = nullptr;
    for (size_t i = 0; i < count; ++i) {
        Node* node = reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(m_start) + i * m_objectSize);
        node->next = nullptr;
        if (prev) {
            prev->next = node;
        } else {
            head = node;
        }
        prev = node;
    }
    m_freeList = head;
}

void PoolAllocator::Clear() {
    RebuildFreeList();
}

void* PoolAllocator::Allocate(size_t size, size_t alignment) {
    (void)size;
    (void)alignment;
    if (!m_freeList) {
        return nullptr;
    }
    if (m_threadSafe) {
#if defined(_WIN32)
        AcquireSRWLockExclusive(static_cast<SRWLOCK*>(m_lock));
#elif defined(__linux__) || defined(__APPLE__)
        pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_lock));
#endif
    }
    void* ptr = m_freeList;
    m_freeList = m_freeList->next;
    if (m_threadSafe) {
#if defined(_WIN32)
        ReleaseSRWLockExclusive(static_cast<SRWLOCK*>(m_lock));
#elif defined(__linux__) || defined(__APPLE__)
        pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_lock));
#endif
    }
    return ptr;
}

void PoolAllocator::Deallocate(void* ptr) {
    if (!ptr) {
        return;
    }
    uintptr_t startAddr = reinterpret_cast<uintptr_t>(m_start);
    uintptr_t endAddr = startAddr + m_totalSize;
    uintptr_t ptrAddr = reinterpret_cast<uintptr_t>(ptr);
    if (ptrAddr < startAddr || ptrAddr >= endAddr) {
        Log::Error("PoolAllocator::Deallocate: Attempted to deallocate pointer {} outside pool boundaries!", ptr);
        return;
    }
    if (m_threadSafe) {
#if defined(_WIN32)
        AcquireSRWLockExclusive(static_cast<SRWLOCK*>(m_lock));
#elif defined(__linux__) || defined(__APPLE__)
        pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_lock));
#endif
    }
    Node* node = static_cast<Node*>(ptr);
    node->next = m_freeList;
    m_freeList = node;
    if (m_threadSafe) {
#if defined(_WIN32)
        ReleaseSRWLockExclusive(static_cast<SRWLOCK*>(m_lock));
#elif defined(__linux__) || defined(__APPLE__)
        pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_lock));
#endif
    }
}

size_t PoolAllocator::GetAvailableCount() const {
    size_t count = 0;
    Node* curr = m_freeList;
    while (curr) {
        ++count;
        curr = curr->next;
    }
    return count;
}

}  // namespace starlight
