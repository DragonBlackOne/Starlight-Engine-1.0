#ifndef SIMD_MATH_H
#define SIMD_MATH_H

#include <immintrin.h>
#include <cglm/cglm.h>

/**
 * @brief Multiplica um array de matrizes por um array de posicoes em paralelo (SIMD).
 * Processa 8 floats (2 vetores 3D + padding) por vez.
 */
void simd_transform_points(const mat4 model, const float* in_points, float* out_points, int count);

/**
 * @brief Calcula o produto escalar (dot product) de 8 pares de vetores simultaneamente.
 */
void simd_dot_product_8x(const float* a, const float* b, float* results);

#endif
