#pragma once
#include <string>
#include <memory>
#include "Renderer.hpp"

namespace starlight {
    struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        bool valid = false;
    };

    class AssetLoader {
    public:
        // Model Loading (CPU only - No GPU allocation)
        static MeshData LoadOBJ(const std::string& path);
        static MeshData LoadGLTF(const std::string& path);

        // Texture Loading
        static uint32_t LoadTexture(const std::string& path, bool removeCheckered = false);
        static uint32_t LoadCubemap(const std::vector<std::string>& faces);
        static uint32_t CreateCheckerboardTexture(int width, int height, int cellSize);
        
        // Procedural 2D Game Assets
        static uint32_t CreateProceduralIsometricTile(const glm::vec3& color);
        static uint32_t CreateProceduralBuilding(const glm::vec3& color);

        static void OptimizeMesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
        static MeshData SimplifyMesh(const MeshData& input, float targetRatio);
    };
}
