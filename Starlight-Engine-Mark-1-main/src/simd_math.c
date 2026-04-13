#include "simd_math.h"
#include <immintrin.h>

void simd_transform_points(const mat4 model, const float* in_points, float* out_points, int count) {
    // Carregar colunas da matriz nos registradores YMM (256-bit)
    __m256 col1 = _mm256_set_ps(model[0][3], model[0][2], model[0][1], model[0][0], model[0][3], model[0][2], model[0][1], model[0][0]);
    __m256 col2 = _mm256_set_ps(model[1][3], model[1][2], model[1][1], model[1][0], model[1][3], model[1][2], model[1][1], model[1][0]);
    __m256 col3 = _mm256_set_ps(model[2][3], model[2][2], model[2][1], model[2][0], model[2][3], model[2][2], model[2][1], model[2][0]);
    __m256 col4 = _mm256_set_ps(model[3][3], model[3][2], model[3][1], model[3][0], model[3][3], model[3][2], model[3][1], model[3][0]);

    for (int i = 0; i < count; i += 2) { 
        __m256 points = _mm256_loadu_ps(&in_points[i * 4]);
        __m256 res = _mm256_mul_ps(_mm256_shuffle_ps(points, points, _MM_SHUFFLE(0, 0, 0, 0)), col1);
        res = _mm256_add_ps(_mm256_mul_ps(_mm256_shuffle_ps(points, points, _MM_SHUFFLE(1, 1, 1, 1)), col2), res);
        res = _mm256_add_ps(_mm256_mul_ps(_mm256_shuffle_ps(points, points, _MM_SHUFFLE(2, 2, 2, 2)), col3), res);
        res = _mm256_add_ps(_mm256_mul_ps(_mm256_shuffle_ps(points, points, _MM_SHUFFLE(3, 3, 3, 3)), col4), res);
        _mm256_storeu_ps(&out_points[i * 4], res);
    }
}

void simd_dot_product_8x(const float* a, const float* b, float* results) {
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
