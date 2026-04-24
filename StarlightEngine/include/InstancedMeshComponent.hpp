// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Mesh.hpp"
#include "Shader.hpp"

namespace starlight {

    struct InstancedMeshComponent {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Shader> shader;
        std::vector<glm::mat4> instanceMatrices;
        
        uint32_t instanceVBO = 0;
        bool bufferDirty = true;
        
        static constexpr int MAX_INSTANCES = 10000;

        void UpdateBuffer() {
            if (instanceVBO == 0) {
                glGenBuffers(1, &instanceVBO);
                glBindVertexArray(mesh->GetVAO());
                glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
                glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
                
                // Set up instance matrix attributes (slots 6, 7, 8, 9)
                for (int i = 0; i < 4; i++) {
                    glEnableVertexAttribArray(6 + i);
                    glVertexAttribPointer(6 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
                    glVertexAttribDivisor(6 + i, 1);
                }
                glBindVertexArray(0);
            }

            if (bufferDirty && !instanceMatrices.empty()) {
                glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, instanceMatrices.size() * sizeof(glm::mat4), instanceMatrices.data());
                bufferDirty = false;
            }
        }
    };

}
