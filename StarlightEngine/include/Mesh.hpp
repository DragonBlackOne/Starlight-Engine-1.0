// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace starlight {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
        glm::ivec4 jointIndices; // New: Skeletal Animation
        glm::vec4 weights;      // New: Skeletal Animation
    };

    class Mesh {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        ~Mesh();

        void Draw();
        void DrawInstanced(uint32_t instanceCount);
        uint32_t GetVAO() const { return m_vao; }

    private:
        uint32_t m_vao, m_vbo, m_ebo;
        uint32_t m_indexCount;
        void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    };
}
