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

        /**
         * @brief Vectorized frustum culling check for an AABB using AVX2.
         * @param planes Array of 6 planes (each has 4 floats: normal.x, y, z, distance).
         * @param minBounds AABB min bounds (3 floats: x, y, z).
         * @param maxBounds AABB max bounds (3 floats: x, y, z).
         * @return true if the AABB is inside/intersects the frustum, false if culled.
         */
        bool IsAABBInFrustum(const float* planes, const float* minBounds, const float* maxBounds);
    }
}
