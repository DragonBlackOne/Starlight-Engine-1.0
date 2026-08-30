#pragma once

// ============================================================================
// Memory.hpp (compatibility umbrella)
// ----------------------------------------------------------------------------
// Re-exports the Module 1 memory subsystem. The implementation now lives under
// core/Memory/ so that dependent code including the classic "Memory.hpp" path
// keeps working unchanged. New code should prefer the modular includes:
//   #include "core/Memory/Allocator.hpp"
//   #include "core/Memory/LinearAllocator.hpp"
//   #include "core/Memory/PoolAllocator.hpp"
//   #include "core/Memory/StackAllocator.hpp"
//   #include "core/Memory/DoubleBufferAllocator.hpp"
//   #include "core/Memory/MemoryManager.hpp"
// ============================================================================

#include "core/Memory/Allocator.hpp"
#include "core/Memory/LinearAllocator.hpp"
#include "core/Memory/PoolAllocator.hpp"
#include "core/Memory/StackAllocator.hpp"
#include "core/Memory/MemoryManager.hpp"
