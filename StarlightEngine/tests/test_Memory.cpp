#include <gtest/gtest.h>
#include "Memory.hpp"

using namespace starlight;

class LinearAllocatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        allocator = std::make_unique<LinearAllocator>(1024);
    }

    std::unique_ptr<LinearAllocator> allocator;
};

TEST_F(LinearAllocatorTest, InitialState) {
    EXPECT_EQ(allocator->GetUsedMemory(), 0u);
    EXPECT_EQ(allocator->GetTotalSize(), 1024u);
}

TEST_F(LinearAllocatorTest, AllocateBasic) {
    void* ptr = allocator->Allocate(64);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(allocator->GetUsedMemory(), 64u);
}

TEST_F(LinearAllocatorTest, AllocateAlignment) {
    void* ptr1 = allocator->Allocate(1);
    ASSERT_NE(ptr1, nullptr);
    void* ptr2 = allocator->Allocate(16, 16);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 16, 0u);
}

TEST_F(LinearAllocatorTest, AllocateMultiple) {
    allocator->Allocate(128);
    allocator->Allocate(256);
    allocator->Allocate(64);
    EXPECT_EQ(allocator->GetUsedMemory(), 448u);
}

TEST_F(LinearAllocatorTest, AllocateExactSize) {
    void* ptr = allocator->Allocate(1024);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(allocator->GetUsedMemory(), 1024u);
}

TEST_F(LinearAllocatorTest, AllocateOverCapacityReturnsNull) {
    allocator->Allocate(1024);
    void* ptr = allocator->Allocate(1);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(LinearAllocatorTest, ClearResetsOffset) {
    allocator->Allocate(512);
    allocator->Clear();
    EXPECT_EQ(allocator->GetUsedMemory(), 0u);
}

TEST_F(LinearAllocatorTest, AllocateAfterClearSucceeds) {
    allocator->Allocate(1024);
    allocator->Clear();
    void* ptr = allocator->Allocate(512);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(allocator->GetUsedMemory(), 512u);
}

TEST_F(LinearAllocatorTest, DeallocateIsNoOp) {
    void* ptr = allocator->Allocate(64);
    allocator->Deallocate(ptr);
    EXPECT_EQ(allocator->GetUsedMemory(), 64u);
}

TEST_F(LinearAllocatorTest, ZeroSizeAllocation) {
    void* ptr = allocator->Allocate(0);
    EXPECT_NE(ptr, nullptr);
}

class PoolAllocatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        allocator = std::make_unique<PoolAllocator>(64, 10);
    }

    std::unique_ptr<PoolAllocator> allocator;
};

TEST_F(PoolAllocatorTest, AllocateBasic) {
    void* ptr = allocator->Allocate(64);
    ASSERT_NE(ptr, nullptr);
}

TEST_F(PoolAllocatorTest, AllocateAllSlots) {
    std::vector<void*> ptrs;
    for (int i = 0; i < 10; i++) {
        void* ptr = allocator->Allocate(64);
        ASSERT_NE(ptr, nullptr) << "at slot " << i;
        ptrs.push_back(ptr);
    }
    EXPECT_EQ(allocator->Allocate(64), nullptr);
}

TEST_F(PoolAllocatorTest, AllocateReturnsUniquePointers) {
    void* p1 = allocator->Allocate(64);
    void* p2 = allocator->Allocate(64);
    EXPECT_NE(p1, p2);
}

TEST_F(PoolAllocatorTest, DeallocateReusesSlot) {
    void* p1 = allocator->Allocate(64);
    allocator->Deallocate(p1);
    void* p2 = allocator->Allocate(64);
    EXPECT_EQ(p1, p2);
}

TEST_F(PoolAllocatorTest, DeallocateAndReuseMultiple) {
    std::vector<void*> ptrs;
    for (int i = 0; i < 10; i++) {
        ptrs.push_back(allocator->Allocate(64));
    }
    EXPECT_EQ(allocator->Allocate(64), nullptr);

    for (auto p : ptrs) {
        allocator->Deallocate(p);
    }

    for (int i = 0; i < 10; i++) {
        void* p = allocator->Allocate(64);
        ASSERT_NE(p, nullptr) << "at slot " << i;
    }
}

TEST_F(PoolAllocatorTest, ClearRebuildsFreeList) {
    allocator->Allocate(64);
    allocator->Allocate(64);
    allocator->Clear();

    for (int i = 0; i < 10; i++) {
        ASSERT_NE(allocator->Allocate(64), nullptr) << "at slot " << i;
    }
}

TEST_F(PoolAllocatorTest, AllocateSmallerThanObjectSize) {
    void* ptr = allocator->Allocate(16);
    ASSERT_NE(ptr, nullptr);
}

TEST_F(PoolAllocatorTest, AllocateZeroSizedPool) {
    PoolAllocator smallPool(4, 1);
    void* ptr = smallPool.Allocate(4);
    ASSERT_NE(ptr, nullptr);
}

TEST(LinearAllocatorEdgeTest, LargeAlignment) {
    LinearAllocator alloc(4096);
    void* ptr = alloc.Allocate(1, 4096);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % 4096, 0u);
}

TEST(LinearAllocatorEdgeTest, ZeroSizeConstructor) {
    LinearAllocator alloc(0);
    EXPECT_EQ(alloc.GetTotalSize(), 0u);
    EXPECT_EQ(alloc.GetUsedMemory(), 0u);
}
