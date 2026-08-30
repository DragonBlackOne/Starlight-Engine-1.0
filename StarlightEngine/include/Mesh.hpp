#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace starlight {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
        glm::ivec4 jointIndices; 
        glm::vec4 weights;      
    };

    class Mesh {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        ~Mesh();

        void Draw();
        void DrawInstanced(uint32_t instanceCount);
        uint32_t GetVAO() const { return m_vao; }
        uint32_t GetIndexCount() const { return m_indexCount; }

    private:
        uint32_t m_vao = 0;
        uint32_t m_vbo = 0;
        uint32_t m_ebo = 0;
        uint32_t m_indexCount = 0;
        void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    };
}
