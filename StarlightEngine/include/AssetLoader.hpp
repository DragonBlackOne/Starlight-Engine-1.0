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

        // Procedural 3D Geometry Generators
        static std::shared_ptr<Mesh> CreatePlaneMesh(float width, float depth, float tileU = 1.0f, float tileV = 1.0f);
        static std::shared_ptr<Mesh> CreateBoxMesh(float width, float height, float depth);
        static std::shared_ptr<Mesh> CreateCylinderMesh(float radius, float height, int segments = 16);
        static std::shared_ptr<Mesh> CreateSphereMesh(float radius, int rings = 12, int segments = 16);
        static std::shared_ptr<Mesh> CreateCapsuleMesh(float radius, float height, int rings = 8, int segments = 16);
        static std::shared_ptr<Mesh> CreateIcosphereMesh(float radius, int subdivisions = 2);
        static std::shared_ptr<Mesh> CreateTorusMesh(float majorRadius, float minorRadius, int radialSegments = 24, int tubularSegments = 16);
        static std::shared_ptr<Mesh> CreateWedgeMesh(float width, float height, float depth);
        static std::shared_ptr<Mesh> CreateHumanoidMesh(float scale = 1.0f);
        static std::shared_ptr<Mesh> CreateTerrainMesh(float width, float depth, int resolution = 32, float heightScale = 1.5f);

        static void OptimizeMesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
        static MeshData SimplifyMesh(const MeshData& input, float targetRatio);
    };
}
