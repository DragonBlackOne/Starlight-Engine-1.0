#include "Terrain.hpp"
#include <iostream>

namespace titan {

    Terrain::Terrain(int width, int depth, float scale) 
        : m_width(width), m_depth(depth), m_scale(scale), m_noise(2026) {
        GenerateMesh();
    }

    Terrain::~Terrain() {
        if (m_vao) glDeleteVertexArrays(1, &m_vao);
        if (m_vbo) glDeleteBuffers(1, &m_vbo);
        if (m_ebo) glDeleteBuffers(1, &m_ebo);
    }

    void Terrain::GenerateMesh() {
        int vert_count = m_width * m_depth;
        m_indexCount = (m_width - 1) * (m_depth - 1) * 6;

        std::vector<float> vertices;
        vertices.reserve(vert_count * 11); // Pos(3), Norm(3), UV(2), Tangent(3)

        for (int z = 0; z < m_depth; z++) {
            for (int x = 0; x < m_width; x++) {
                float fx = x * m_scale;
                float fz = z * m_scale;
                
                // Octave Perlin for industrial feel
                float fy = m_noise.OctavePerlin(fx * 0.1f, 0, fz * 0.1f, 4, 0.5f) * 15.0f;

                // Position
                vertices.push_back(fx);
                vertices.push_back(fy);
                vertices.push_back(fz);

                // Normal (Numerical Gradient)
                float eps = 0.1f;
                float hL = m_noise.OctavePerlin((fx - eps) * 0.1f, 0, fz * 0.1f, 4, 0.5f) * 15.0f;
                float hR = m_noise.OctavePerlin((fx + eps) * 0.1f, 0, fz * 0.1f, 4, 0.5f) * 15.0f;
                float hD = m_noise.OctavePerlin(fx * 0.1f, 0, (fz - eps) * 0.1f, 4, 0.5f) * 15.0f;
                float hU = m_noise.OctavePerlin(fx * 0.1f, 0, (fz + eps) * 0.1f, 4, 0.5f) * 15.0f;
                
                glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 2.0f * eps, hD - hU));
                vertices.push_back(normal.x);
                vertices.push_back(normal.y);
                vertices.push_back(normal.z);

                // UV
                vertices.push_back((float)x / 10.0f);
                vertices.push_back((float)z / 10.0f);

                // Tangent (Approximate)
                glm::vec3 tangent = glm::normalize(glm::vec3(1.0f, hR - fy, 0.0f));
                vertices.push_back(tangent.x);
                vertices.push_back(tangent.y);
                vertices.push_back(tangent.z);
            }
        }

        std::vector<unsigned int> indices;
        indices.reserve(m_indexCount);
        for (int z = 0; z < m_depth - 1; z++) {
            for (int x = 0; x < m_width - 1; x++) {
                int topLeft = (z * m_width) + x;
                int topRight = topLeft + 1;
                int bottomLeft = ((z + 1) * m_width) + x;
                int bottomRight = bottomLeft + 1;

                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Standardized PBR Layout (Pos:0, Norm:1, UV:2, Tang:3)
        glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3); glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));

        glBindVertexArray(0);
    }

    void Terrain::Render() {
        if (!m_vao) return;
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    float Terrain::GetHeight(float x, float z) const {
        return m_noise.OctavePerlin(x * 0.1f, 0, z * 0.1f, 4, 0.5f) * 15.0f;
    }
}
