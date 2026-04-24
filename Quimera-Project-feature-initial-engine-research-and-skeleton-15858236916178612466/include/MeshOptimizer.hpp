// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - Mesh Optimization (meshoptimizer)
// Licença: MIT | Origem: zeux/meshoptimizer
// ============================================================

#include <meshoptimizer.h>
#include <vector>
#include "Log.hpp"

namespace Vamos {
    class MeshOptimizer {
    public:
        // Otimizar cache de vértices (melhora performance GPU)
        static void OptimizeVertexCache(std::vector<unsigned int>& indices, size_t vertexCount) {
            meshopt_optimizeVertexCache(indices.data(), indices.data(), indices.size(), vertexCount);
        }

        // Otimizar overdraw
        static void OptimizeOverdraw(std::vector<unsigned int>& indices, const float* vertices,
                                     size_t vertexCount, size_t stride, float threshold = 1.05f) {
            meshopt_optimizeOverdraw(indices.data(), indices.data(), indices.size(),
                                    vertices, vertexCount, stride, threshold);
        }

        // Otimizar fetch de vértices
        static size_t OptimizeVertexFetch(std::vector<float>& vertices, std::vector<unsigned int>& indices,
                                          size_t vertexCount, size_t stride) {
            return meshopt_optimizeVertexFetch(vertices.data(), indices.data(), indices.size(),
                                               vertices.data(), vertexCount, stride);
        }

        // Simplificação de mesh (LOD)
        static std::vector<unsigned int> Simplify(const std::vector<unsigned int>& indices,
                                                   const float* vertices, size_t vertexCount,
                                                   size_t stride, size_t targetIndexCount,
                                                   float targetError = 1e-2f) {
            std::vector<unsigned int> result(indices.size());
            float error;
            size_t newCount = meshopt_simplify(result.data(), indices.data(), indices.size(),
                                               vertices, vertexCount, stride,
                                               targetIndexCount, targetError, 0, &error);
            result.resize(newCount);
            VAMOS_CORE_INFO("[MeshOpt] LOD: {0} -> {1} triângulos (erro: {2})",
                           indices.size() / 3, newCount / 3, error);
            return result;
        }

        // Gerar LODs automáticos
        struct LODLevel {
            std::vector<unsigned int> indices;
            float quality; // 0.0 = mais simplificado, 1.0 = original
        };

        static std::vector<LODLevel> GenerateLODs(const std::vector<unsigned int>& indices,
                                                   const float* vertices, size_t vertexCount,
                                                   size_t stride, int levels = 4) {
            std::vector<LODLevel> lods;
            lods.push_back({indices, 1.0f}); // LOD 0 = original

            for (int i = 1; i < levels; i++) {
                float quality = 1.0f - (float)i / levels;
                size_t target = (size_t)(indices.size() * quality);
                target = target - target % 3; // múltiplo de 3
                if (target < 3) target = 3;

                auto simplified = Simplify(indices, vertices, vertexCount, stride, target);
                lods.push_back({simplified, quality});
            }
            return lods;
        }
    };
}
