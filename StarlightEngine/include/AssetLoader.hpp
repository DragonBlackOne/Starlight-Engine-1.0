// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <string>
#include <memory>
#include "Renderer.hpp"

namespace starlight {
    class AssetLoader {
    public:
        // Model Loading
        static std::shared_ptr<Mesh> LoadOBJ(const std::string& path);
        static std::shared_ptr<Mesh> LoadGLTF(const std::string& path);

        // Texture Loading
        static uint32_t LoadTexture(const std::string& path);
        static uint32_t CreateCheckerboardTexture(int width, int height, int cellSize);

        // Mesh Optimization (meshoptimizer - MIT)
        // Reorders indices for GPU vertex cache, reduces overdraw, and optimizes vertex fetch order.
        static void OptimizeMesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    };
}
