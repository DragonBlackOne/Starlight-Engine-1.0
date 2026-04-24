// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <immintrin.h>
#include <glm/glm.hpp>

namespace starlight {

    namespace simd {

        // Highly optimized 4x4 Matrix Multiply using SSE
        inline void Mat4Mul(float* res, const float* a, const float* b) {
            __m128 r0 = _mm_loadu_ps(a);
            __m128 r1 = _mm_loadu_ps(a + 4);
            __m128 r2 = _mm_loadu_ps(a + 8);
            __m128 r3 = _mm_loadu_ps(a + 12);

            for(int i=0; i<4; i++) {
                __m128 bc0 = _mm_set1_ps(b[i*4 + 0]);
                __m128 bc1 = _mm_set1_ps(b[i*4 + 1]);
                __m128 bc2 = _mm_set1_ps(b[i*4 + 2]);
                __m128 bc3 = _mm_set1_ps(b[i*4 + 3]);

                __m128 res_row = _mm_add_ps(
                    _mm_add_ps(_mm_mul_ps(r0, bc0), _mm_mul_ps(r1, bc1)),
                    _mm_add_ps(_mm_mul_ps(r2, bc2), _mm_mul_ps(r3, bc3))
                );
                _mm_storeu_ps(res + i*4, res_row);
            }
        }

    }

}
