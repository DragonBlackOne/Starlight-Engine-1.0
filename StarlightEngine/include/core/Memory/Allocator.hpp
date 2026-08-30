#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>

namespace starlight {

/**
 * @brief Base interface for all custom allocators used by Module 1.
 *
 * Allocators are untyped, alignment-aware, and may be used directly or
 * adapted into std containers via StlAllocator<T>.
 */
class IAllocator {
public:
    virtual ~IAllocator() = default;

    /**
     * @return Pointer to a block of at least `size` bytes aligned to `alignment`.
     *         Returns nullptr on out-of-memory / capacity exhaustion.
     */
    virtual void* Allocate(size_t size, size_t alignment = kDefaultAlignment) = 0;

    /**
     * @brief Return a previously allocated pointer to the allocator.
     *        Some allocators (Linear/Stack) ignore this; the pointer and size
     *        bookkeeping may be handled by pool/free-list implementations.
     */
    virtual void Deallocate(void* ptr) = 0;

    /**
     * @brief Reset all allocations, allowing the backing memory to be reused.
     *        Ownership of the raw block is retained.
     */
    virtual void Clear() = 0;

    static constexpr size_t kDefaultAlignment = 8;
};

/**
 * @brief Fallback allocator backed by the CRT (malloc/free).
 *        Used as a baseline and as the backing source where no arena is supplied.
 */
class DefaultAllocator final : public IAllocator {
public:
    void* Allocate(size_t size, size_t alignment = kDefaultAlignment) override {
        size_t sizeWithHeader = size + alignment + sizeof(void*);
        void* raw = std::malloc(sizeWithHeader);
        if (!raw) {
            return nullptr;
        }
        uintptr_t rawAddr = reinterpret_cast<uintptr_t>(raw);
        uintptr_t alignedAddr = (rawAddr + sizeof(void*) + alignment - 1) & ~(static_cast<uintptr_t>(alignment - 1));
        void** headerPtr = reinterpret_cast<void**>(alignedAddr);
        headerPtr[-1] = raw;
        return reinterpret_cast<void*>(alignedAddr);
    }

    void Deallocate(void* ptr) override {
        if (!ptr) {
            return;
        }
        void** headerPtr = reinterpret_cast<void**>(ptr);
        std::free(headerPtr[-1]);
    }

    void Clear() override {
        // Nothing to do for a system allocator.
    }
};

/**
 * @brief STL allocator adapter that routes std container allocations through an
 *        IAllocator. Enables zero-copy use of arena/linear/stack memory in STL
 *        containers (vectors, maps) without changing container code.
 *
 * @tparam T Element type to allocate.
 */
template<typename T>
class StlAllocator {
public:
    using value_type = T;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::false_type;

    explicit StlAllocator(IAllocator* backing) : m_backing(backing) {
        assert(backing != nullptr);
    }

    template<typename U>
    StlAllocator(const StlAllocator<U>& other) : m_backing(other.GetBacking()) {}

    StlAllocator() = default;
    StlAllocator(const StlAllocator&) = default;
    StlAllocator& operator=(const StlAllocator&) = default;

    T* allocate(std::size_t n) {
        if (n == 0) {
            return nullptr;
        }
        // Ensure at least one element is allocated, and fall back to CRT if no backing set.
        if (!m_backing) {
            return static_cast<T*>(DefaultAllocator().Allocate(n * sizeof(T), alignof(T)));
        }
        return static_cast<T*>(m_backing->Allocate(n * sizeof(T), alignof(T)));
    }

    void deallocate(T* p, std::size_t /*n*/) noexcept {
        if (!p) {
            return;
        }
        if (m_backing) {
            m_backing->Deallocate(p);
        } else {
            DefaultAllocator().Deallocate(p);
        }
    }

    template<typename U>
    bool operator==(const StlAllocator<U>& other) const noexcept {
        return m_backing == other.GetBacking();
    }

    template<typename U>
    bool operator!=(const StlAllocator<U>& other) const noexcept {
        return !(*this == other);
    }

    IAllocator* GetBacking() const {
        return m_backing;
    }

private:
    IAllocator* m_backing = nullptr;
};

/**
 * @brief Helper to compute the next aligned offset at or above `value`.
 */
inline size_t AlignUp(size_t value, size_t alignment) {
    if (alignment == 0) {
        return value;
    }
    const size_t mask = alignment - 1;
    return (value + mask) & ~mask;
}

/**
 * @brief Returns required padding so that (base + currentOffset + padding) is
 *        aligned to `alignment` relative to base.
 */
inline size_t ComputePadding(uintptr_t baseAddr, size_t currentOffset, size_t alignment) {
    const uintptr_t currentAddr = baseAddr + currentOffset;
    const uintptr_t misalignment = currentAddr % alignment;
    return (misalignment == 0) ? 0 : (alignment - misalignment);
}

}  // namespace starlight
