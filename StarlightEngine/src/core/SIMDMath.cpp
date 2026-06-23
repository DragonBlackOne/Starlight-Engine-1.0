#include "SIMDMath.hpp"
#include <immintrin.h>
#include <glm/gtc/type_ptr.hpp>

namespace starlight {
    namespace simd {

        void TransformPoints(const glm::mat4& model, const float* in_points, float* out_points, int count) {
            const float* m = glm::value_ptr(model);
            
            // Carregar colunas da matriz nos registradores YMM (256-bit)
            __m256 col1 = _mm256_set_ps(m[3], m[2], m[1], m[0], m[3], m[2], m[1], m[0]);
            __m256 col2 = _mm256_set_ps(m[7], m[6], m[5], m[4], m[7], m[6], m[5], m[4]);
            __m256 col3 = _mm256_set_ps(m[11], m[10], m[9], m[8], m[11], m[10], m[9], m[8]);
            __m256 col4 = _mm256_set_ps(m[15], m[14], m[13], m[12], m[15], m[14], m[13], m[12]);

            int i = 0;
            for (; i < count - 1; i += 2) { 
                __m256 points = _mm256_loadu_ps(&in_points[i * 4]);
                
                // Shuffle para multiplicar cada componente pela coluna correspondente
                __m256 res = _mm256_mul_ps(_mm256_shuffle_ps(points, points, _MM_SHUFFLE(0, 0, 0, 0)), col1);
                res = _mm256_add_ps(_mm256_mul_ps(_mm256_shuffle_ps(points, points, _MM_SHUFFLE(1, 1, 1, 1)), col2), res);
                res = _mm256_add_ps(_mm256_mul_ps(_mm256_shuffle_ps(points, points, _MM_SHUFFLE(2, 2, 2, 2)), col3), res);
                res = _mm256_add_ps(_mm256_mul_ps(_mm256_shuffle_ps(points, points, _MM_SHUFFLE(3, 3, 3, 3)), col4), res);
                
                _mm256_storeu_ps(&out_points[i * 4], res);
            }

            if (i < count) {
                // Tratar o último ponto ímpar restante com segurança usando SSE de 128 bits
                __m128 col1_128 = _mm_set_ps(m[3], m[2], m[1], m[0]);
                __m128 col2_128 = _mm_set_ps(m[7], m[6], m[5], m[4]);
                __m128 col3_128 = _mm_set_ps(m[11], m[10], m[9], m[8]);
                __m128 col4_128 = _mm_set_ps(m[15], m[14], m[13], m[12]);

                __m128 point = _mm_loadu_ps(&in_points[i * 4]);
                __m128 res_128 = _mm_mul_ps(_mm_shuffle_ps(point, point, _MM_SHUFFLE(0, 0, 0, 0)), col1_128);
                res_128 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(point, point, _MM_SHUFFLE(1, 1, 1, 1)), col2_128), res_128);
                res_128 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(point, point, _MM_SHUFFLE(2, 2, 2, 2)), col3_128), res_128);
                res_128 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(point, point, _MM_SHUFFLE(3, 3, 3, 3)), col4_128), res_128);

                _mm_storeu_ps(&out_points[i * 4], res_128);
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

        bool IsAABBInFrustum(const float* planes, const float* minBounds, const float* maxBounds) {
            // Load plane components for 6 planes + 2 dummy planes to pad to 8 lanes
            alignas(32) float nx[8] = { planes[0], planes[4], planes[8],  planes[12], planes[16], planes[20], 0.0f, 0.0f };
            alignas(32) float ny[8] = { planes[1], planes[5], planes[9],  planes[13], planes[17], planes[21], 0.0f, 0.0f };
            alignas(32) float nz[8] = { planes[2], planes[6], planes[10], planes[14], planes[18], planes[22], 0.0f, 0.0f };
            alignas(32) float d[8]  = { planes[3], planes[7], planes[11], planes[15], planes[19], planes[23], 1e9f, 1e9f };

            __m256 plane_nx = _mm256_load_ps(nx);
            __m256 plane_ny = _mm256_load_ps(ny);
            __m256 plane_nz = _mm256_load_ps(nz);
            __m256 plane_d  = _mm256_load_ps(d);

            __m256 min_x = _mm256_set1_ps(minBounds[0]);
            __m256 max_x = _mm256_set1_ps(maxBounds[0]);
            __m256 min_y = _mm256_set1_ps(minBounds[1]);
            __m256 max_y = _mm256_set1_ps(maxBounds[1]);
            __m256 min_z = _mm256_set1_ps(minBounds[2]);
            __m256 max_z = _mm256_set1_ps(maxBounds[2]);

            __m256 zero = _mm256_setzero_ps();
            __m256 mask_x = _mm256_cmp_ps(plane_nx, zero, _CMP_GT_OQ);
            __m256 mask_y = _mm256_cmp_ps(plane_ny, zero, _CMP_GT_OQ);
            __m256 mask_z = _mm256_cmp_ps(plane_nz, zero, _CMP_GT_OQ);

            __m256 px = _mm256_blendv_ps(min_x, max_x, mask_x);
            __m256 py = _mm256_blendv_ps(min_y, max_y, mask_y);
            __m256 pz = _mm256_blendv_ps(min_z, max_z, mask_z);

            __m256 val = _mm256_fmadd_ps(plane_nx, px, plane_d);
            val = _mm256_fmadd_ps(plane_ny, py, val);
            val = _mm256_fmadd_ps(plane_nz, pz, val);

            __m256 cmp = _mm256_cmp_ps(val, zero, _CMP_LT_OQ);
            int mask = _mm256_movemask_ps(cmp);

            mask &= 0x3F;

            return mask == 0;
        }
    }
}
