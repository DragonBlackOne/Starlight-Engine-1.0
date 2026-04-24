// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace starlight {
    namespace simd {
        /**
         * @brief Transforma uma massa de pontos usando AVX2.
         * @param model Matriz de transformação.
         * @param in_points Ponteiro para array de floats (x,y,z,w).
         * @param out_points Ponteiro para array de saída.
         * @param count Número de pontos (deve ser múltiplo de 2 para AVX).
         */
        void TransformPoints(const glm::mat4& model, const float* in_points, float* out_points, int count);

        /**
         * @brief Calcula o produto escalar de 8 pares de vetores simultaneamente.
         */
        void DotProduct8x(const float* a, const float* b, float* results);
    }
}
