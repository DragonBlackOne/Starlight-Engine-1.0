#pragma once
#include "core/Memory/Allocator.hpp"

namespace starlight {

/**
 * @brief LIFO (stack) allocator with scoped rewind support.
 *
 * Allocation bumps a mark pointer; Deallocate pops the most recent block.
 * Supports scoped rollback via Mark()/Rewind(mark) so temporary scratch buffers
 * can be released in bulk with zero overhead. Is LinearAllocator without the
 * "only Clear()" limitation - individual LIFO frees are allowed.
 *
 * Layout of a single allocation block:
 *
 *   [Block header][padding][payload...]
 *
 * The payload is aligned to the requested alignment; the header stores the
 * absolute offset of its own start so Deallocate/Rewind are exact.
 */
class StackAllocator : public IAllocator {
public:
    explicit StackAllocator(size_t size, IAllocator* backing = nullptr);
    ~StackAllocator() override;

    StackAllocator(const StackAllocator&) = delete;
    StackAllocator& operator=(const StackAllocator&) = delete;

    void* Allocate(size_t size, size_t alignment = kDefaultAlignment) override;
    void Deallocate(void* ptr) override;
    void Clear() override;

    /**
     * @brief Opaque marker used to rewind the stack to an earlier offset.
     */
    using Mark = size_t;

    Mark GetMark() const {
        return m_offset;
    }

    /**
     * @brief Rewind the stack to a previously captured mark, freeing everything
     *        allocated since then. Does nothing if `mark > m_offset`.
     */
    void Rewind(Mark mark) {
        if (mark >= m_offset) {
            return;
        }
        // Pop blocks whose payload starts on or after the target mark.
        while (m_top && m_top->payload >= mark) {
            m_top = m_top->prev;
        }
        m_offset = mark;
    }

    size_t GetUsedMemory() const {
        return m_offset;
    }
    size_t GetTotalSize() const {
        return m_size;
    }
    size_t GetRemainingMemory() const {
        return m_size - m_offset;
    }

private:
    struct Block {
        Block* prev;     // previous block or nullptr
        size_t prevEnd;  // allocator offset before this block (restoration point)
        size_t payload;  // offset of the payload within the buffer
    };

    void* m_start = nullptr;
    void* m_rawStart = nullptr;
    IAllocator* m_backing = nullptr;
    size_t m_size = 0;
    size_t m_offset = 0;
    Block* m_top = nullptr;
    bool m_ownsMemory = false;
};

}  // namespace starlight
