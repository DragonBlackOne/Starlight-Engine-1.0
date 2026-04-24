// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "TerrainSystem.hpp"
#include <glad/glad.h>
#include <glm/gtc/noise.hpp>
#include "Log.hpp"

namespace titan {

    Terrain TerrainSystem::CreateProcedural(int width, int depth, float scale) {
        Terrain terrain;
        int vertCount = width * depth;
        int triCount = (width - 1) * (depth - 1) * 2;
        terrain.indexCount = triCount * 3;

        std::vector<float> vertices(vertCount * 16, 0.0f);
        std::vector<uint32_t> indices(terrain.indexCount);

        for (int z = 0; z < depth; z++) {
            for (int x = 0; x < width; x++) {
                int idx = (x + z * width);
                float xPos = x * scale;
                float zPos = z * scale;
                
                // Use GLM noise for portability
                float yPos = glm::simplex(glm::vec2(xPos * 0.1f, zPos * 0.1f)) * 5.0f;

                // Pos
                vertices[idx * 16 + 0] = xPos;
                vertices[idx * 16 + 1] = yPos;
                vertices[idx * 16 + 2] = zPos;
                
                // Normal approximation
                float hL = glm::simplex(glm::vec2((xPos - scale) * 0.1f, zPos * 0.1f)) * 5.0f;
                float hR = glm::simplex(glm::vec2((xPos + scale) * 0.1f, zPos * 0.1f)) * 5.0f;
                float hD = glm::simplex(glm::vec2(xPos * 0.1f, (zPos - scale) * 0.1f)) * 5.0f;
                float hU = glm::simplex(glm::vec2(xPos * 0.1f, (zPos + scale) * 0.1f)) * 5.0f;
                
                glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 2.0f, hD - hU));
                vertices[idx * 16 + 3] = normal.x;
                vertices[idx * 16 + 4] = normal.y;
                vertices[idx * 16 + 5] = normal.z;

                // UV
                vertices[idx * 16 + 6] = (float)x / 5.0f;
                vertices[idx * 16 + 7] = (float)z / 5.0f;
            }
        }

        int i = 0;
        for (int z = 0; z < depth - 1; z++) {
            for (int x = 0; x < width - 1; x++) {
                int topLeft = (z * width) + x;
                int topRight = topLeft + 1;
                int bottomLeft = ((z + 1) * width) + x;
                int bottomRight = bottomLeft + 1;

                indices[i++] = topLeft;
                indices[i++] = bottomLeft;
                indices[i++] = topRight;

                indices[i++] = topRight;
                indices[i++] = bottomLeft;
                indices[i++] = bottomRight;
            }
        }

        glGenVertexArrays(1, &terrain.vao);
        glGenBuffers(1, &terrain.vbo);
        glGenBuffers(1, &terrain.ebo);

        glBindVertexArray(terrain.vao);
        glBindBuffer(GL_ARRAY_BUFFER, terrain.vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

        // Position
        glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 16 * 4, (void*)0);
        // Normal
        glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 16 * 4, (void*)(3 * 4));
        // UV
        glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 16 * 4, (void*)(6 * 4));

        glBindVertexArray(0);
        Log::Info("TerrainSystem: Generated " + std::to_string(width) + "x" + std::to_string(depth) + " procedural mesh.");
        
        return terrain;
    }

    void TerrainSystem::Render(const Terrain& terrain, uint32_t shader) {
        if (terrain.vao == 0) return;
        glUseProgram(shader);
        glBindVertexArray(terrain.vao);
        glDrawElements(GL_TRIANGLES, terrain.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void TerrainSystem::Destroy(Terrain& terrain) {
        glDeleteVertexArrays(1, &terrain.vao);
        glDeleteBuffers(1, &terrain.vbo);
        glDeleteBuffers(1, &terrain.ebo);
        terrain.vao = 0;
    }
}
