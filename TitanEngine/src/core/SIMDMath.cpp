// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "SIMDMath.hpp"
#include <immintrin.h>
#include <glm/gtc/type_ptr.hpp>

namespace titan {
    namespace simd {

        void TransformPoints(const glm::mat4& model, const float* in_points, float* out_points, int count) {
            const float* m = glm::value_ptr(model);
            
            // Carregar colunas da matriz nos registradores YMM (256-bit)
            __m256 col1 = _mm256_set_ps(m[3], m[2], m[1], m[0], m[3], m[2], m[1], m[0]);
            __m256 col2 = _mm256_set_ps(m[7], m[6], m[5], m[4], m[7], m[6], m[5], m[4]);
            __m256 col3 = _mm256_set_ps(m[11], m[10], m[9], m[8], m[11], m[10], m[9], m[8]);
            __m256 col4 = _mm256_set_ps(m[15], m[14], m[13], m[12], m[15], m[14], m[13], m[12]);

            for (int i = 0; i < count; i += 2) { 
                __m256 points = _mm256_loadu_ps(&in_points[i * 4]);
                
                // Shuffle para multiplicar cada componente pela coluna correspondente
                __m256 res = _mm256_mul_ps(_mm256_shuffle_ps(points, points, _MM_SHUFFLE(0, 0, 0, 0)), col1);
                res = _mm256_add_ps(_mm256_mul_ps(_mm256_shuffle_ps(points, points, _MM_SHUFFLE(1, 1, 1, 1)), col2), res);
                res = _mm256_add_ps(_mm256_mul_ps(_mm256_shuffle_ps(points, points, _MM_SHUFFLE(2, 2, 2, 2)), col3), res);
                res = _mm256_add_ps(_mm256_mul_ps(_mm256_shuffle_ps(points, points, _MM_SHUFFLE(3, 3, 3, 3)), col4), res);
                
                _mm256_storeu_ps(&out_points[i * 4], res);
            }
        }

        void DotProduct8x(const float* a, const float* b, float* results) {
            for (int i = 0; i < 8; ++i) {
                __m256 v_a = _mm256_loadu_ps(&a[i * 8]);
                __m256 v_b = _mm256_loadu_ps(&b[i * 8]);
                __m256 res = _mm256_mul_ps(v_a, v_b);
                
                __m128 lo = _mm256_extractf128_ps(res, 0);
                __m128 hi = _mm256_extractf128_ps(res, 1);
                __m128 sum = _mm_add_ps(lo, hi);
                sum = _mm_hadd_ps(sum, sum);
                sum = _mm_hadd_ps(sum, sum);
                _mm_store_ss(&results[i], sum);
            }
        }
    }
}
