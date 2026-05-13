// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace starlight {
    namespace simd {
        /**
         * @brief Transforma uma massa de pontos usando AVX2.
         * @param model Matriz de transformaÃƒÆ’Ã‚Â§ÃƒÆ’Ã‚Â£o.
         * @param in_points Ponteiro para array de floats (x,y,z,w).
         * @param out_points Ponteiro para array de saÃƒÆ’Ã‚Â­da.
         * @param count NÃƒÆ’Ã‚Âºmero de pontos (deve ser mÃƒÆ’Ã‚Âºltiplo de 2 para AVX).
         */
        void TransformPoints(const glm::mat4& model, const float* in_points, float* out_points, int count);

        /**
         * @brief Calcula o produto escalar de 8 pares de vetores simultaneamente.
         */
        void DotProduct8x(const float* a, const float* b, float* results);
    }
}
