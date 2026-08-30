#pragma once
#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// SIMD intrinsics, guarded so 32-bit / non-x86 builds still compile and fall
// back to scalar paths.
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
#define STARLIGHT_SIMD_SSE 1
#endif
#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
#define STARLIGHT_SIMD_SSE 2
#endif
#if defined(STARLIGHT_SIMD_SSE)
#include <immintrin.h>
#endif

namespace starlight::simd {

// ---------------------------------------------------------------------------
// Runtime ISA detection
// ---------------------------------------------------------------------------
enum class Isa : uint32_t {
    None = 0,
    SSE = 1 << 0,
    SSE2 = 1 << 1,
    SSE3 = 1 << 2,
    SSSE3 = 1 << 3,
    SSE4_1 = 1 << 4,
    SSE4_2 = 1 << 5,
    AVX = 1 << 6,
    AVX2 = 1 << 7,
    FMA = 1 << 8,
};

/**
 * @brief Query the highest instruction set extension available on the host CPU.
 *        Result is cached after the first call.
 */
Isa GetHighestIsa();

constexpr size_t kAlignmentSSE = 16;
constexpr size_t kAlignmentAVX = 32;

// ---------------------------------------------------------------------------
// Alignment helpers
// ---------------------------------------------------------------------------
inline uintptr_t AlignUpAddr(uintptr_t addr, size_t alignment) {
    const uintptr_t mask = static_cast<uintptr_t>(alignment - 1);
    return (addr + mask) & ~mask;
}

template<typename T>
inline T* AlignUpPtr(T* ptr, size_t alignment) {
    return reinterpret_cast<T*>(AlignUpAddr(reinterpret_cast<uintptr_t>(ptr), alignment));
}

// ---------------------------------------------------------------------------
// SSE 4x4 matrix multiplication (column-major, matches glm::mat4 layout)
// ---------------------------------------------------------------------------
inline void Mat4Mul(float* out, const float* a, const float* b) {
#if defined(STARLIGHT_SIMD_SSE)
    __m128 r0 = _mm_loadu_ps(a);
    __m128 r1 = _mm_loadu_ps(a + 4);
    __m128 r2 = _mm_loadu_ps(a + 8);
    __m128 r3 = _mm_loadu_ps(a + 12);

    for (int i = 0; i < 4; ++i) {
        __m128 bc0 = _mm_set1_ps(b[i * 4 + 0]);
        __m128 bc1 = _mm_set1_ps(b[i * 4 + 1]);
        __m128 bc2 = _mm_set1_ps(b[i * 4 + 2]);
        __m128 bc3 = _mm_set1_ps(b[i * 4 + 3]);

        __m128 row = _mm_add_ps(
            _mm_add_ps(_mm_mul_ps(r0, bc0), _mm_mul_ps(r1, bc1)), _mm_add_ps(_mm_mul_ps(r2, bc2), _mm_mul_ps(r3, bc3)));
        _mm_storeu_ps(out + i * 4, row);
    }
#else
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += a[k * 4 + j] * b[i * 4 + k];
            }
            out[i * 4 + j] = sum;
        }
    }
#endif
}

inline glm::mat4 Mat4Mul(const glm::mat4& lhs, const glm::mat4& rhs) {
    glm::mat4 out;
    Mat4Mul(glm::value_ptr(out), glm::value_ptr(lhs), glm::value_ptr(rhs));
    return out;
}

// ---------------------------------------------------------------------------
// Quaternion operations (w, x, y, z layout as in glm::quat)
//
// NOTE: The Hamilton product / dot use the scalar glm implementation for
// guaranteed correctness; they are trivially SIMD-capable and can be
// accelerated in a follow-up without changing the API.
// ---------------------------------------------------------------------------
inline glm::quat QuatMul(const glm::quat& lhs, const glm::quat& rhs) {
    return lhs * rhs;
}

inline float QuatDot(const glm::quat& lhs, const glm::quat& rhs) {
    return glm::dot(lhs, rhs);
}

inline glm::quat QuatNormalize(const glm::quat& q) {
    return glm::normalize(q);
}

}  // namespace starlight::simd
