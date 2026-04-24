// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "terrain.h"
#include "noise.h"
#include <stdlib.h>
#include <stdio.h>

void terrain_init(Terrain* terrain, int width, int depth, float scale) {
    int vert_count = width * depth;
    int tri_count = (width - 1) * (depth - 1) * 2;
    terrain->index_count = tri_count * 3;

    // 16 floats per vertex: Pos(3), Norm(3), UV(2), Joints(4), Weights(4)
    float* vertices = (float*)calloc(vert_count * 16, sizeof(float));
    unsigned int* indices = (unsigned int*)malloc(terrain->index_count * sizeof(unsigned int));

    for (int z = 0; z < depth; z++) {
        for (int x = 0; x < width; x++) {
            int idx = (x + z * width);
            float xPos = x * scale;
            float zPos = z * scale;
            float yPos = simplex_noise(xPos, zPos) * 10.0f; // More height for drama

            // Pos
            vertices[idx * 16 + 0] = xPos;
            vertices[idx * 16 + 1] = yPos;
            vertices[idx * 16 + 2] = zPos;
            
            // Smooth Normal Calculation (Gradient Approximation)
            float hL = simplex_noise(xPos - scale, zPos) * 10.0f;
            float hR = simplex_noise(xPos + scale, zPos) * 10.0f;
            float hD = simplex_noise(xPos, zPos - scale) * 10.0f;
            float hU = simplex_noise(xPos, zPos + scale) * 10.0f;
            
            vec3 normal = {hL - hR, 2.0f, hD - hU};
            glm_vec3_normalize(normal);

            vertices[idx * 16 + 3] = normal[0];
            vertices[idx * 16 + 4] = normal[1];
            vertices[idx * 16 + 5] = normal[2];

            // UV
            vertices[idx * 16 + 6] = (float)x / 10.0f; // Smaller tiling
            vertices[idx * 16 + 7] = (float)z / 10.0f;

            vertices[idx * 16 + 8] = -1.0f; 
            vertices[idx * 16 + 12] = 0.0f;
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

    glGenVertexArrays(1, &terrain->vao);
    glGenBuffers(1, &terrain->vbo);
    glGenBuffers(1, &terrain->ebo);

    glBindVertexArray(terrain->vao);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->vbo);
    glBufferData(GL_ARRAY_BUFFER, vert_count * 16 * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrain->index_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // Standardized PBR Layout
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 16 * 4, (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 16 * 4, (void*)(3 * 4));
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 16 * 4, (void*)(6 * 4));
    glEnableVertexAttribArray(3); glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 16 * 4, (void*)(3 * 4)); // Fallback tangent
    glEnableVertexAttribArray(4); glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * 4, (void*)(8 * 4));
    glEnableVertexAttribArray(5); glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 16 * 4, (void*)(12 * 4));

    glBindVertexArray(0);

    free(vertices);
    free(indices);
    printf("[TERRAIN] Procedural mesh %dx%d generated. VAO: %u, EBO: %u, Indices: %d\n", 
        width, depth, terrain->vao, terrain->ebo, terrain->index_count);
}

void terrain_draw(Terrain* terrain, GLuint shader_program) {
    if (terrain->vao == 0) return;
    glUseProgram(shader_program);
    glUniform1i(glGetUniformLocation(shader_program, "hasSkin"), 0);
    glBindVertexArray(terrain->vao);
    glDrawElements(GL_TRIANGLES, terrain->index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void terrain_destroy(Terrain* terrain) {
    glDeleteVertexArrays(1, &terrain->vao);
    glDeleteBuffers(1, &terrain->vbo);
    glDeleteBuffers(1, &terrain->ebo);
}
