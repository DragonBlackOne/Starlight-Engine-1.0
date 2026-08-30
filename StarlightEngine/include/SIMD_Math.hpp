#pragma once

// ============================================================================
// SIMD_Math.hpp (compatibility shim)
// ----------------------------------------------------------------------------
// Historically provided an inline starlight::simd::Mat4Mul. That functionality
// now lives in core/SIMDUtils.hpp (same public API: simd::Mat4Mul). This shim
// keeps dependent code compiling unchanged.
// ============================================================================

#include "core/SIMDUtils.hpp"
