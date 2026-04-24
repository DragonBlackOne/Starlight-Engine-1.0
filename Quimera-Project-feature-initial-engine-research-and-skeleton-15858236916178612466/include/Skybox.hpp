// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>
#include <memory>
#include "Shader.hpp"
#include "stb_image.h"

namespace Vamos {

    class Skybox {
    public:
        Skybox(const std::string& texturePath) {
            shader = std::make_unique<Shader>("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
            LoadPanorama(texturePath);
            SetupMesh();
        }

        void Render(const glm::mat4& view, const glm::mat4& projection) {
            glDepthFunc(GL_LEQUAL);
            shader->Use();
            // Remove translation from view matrix
            glm::mat4 skyView = glm::mat4(glm::mat3(view));
            shader->SetMat4("view", skyView);
            shader->SetMat4("projection", projection);

            glBindVertexArray(skyVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, panoramaTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS);
        }

    private:
        void LoadPanorama(const std::string& path) {
            glGenTextures(1, &panoramaTexture);
            glBindTexture(GL_TEXTURE_2D, panoramaTexture);

            int width, height, nrChannels;
            unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                stbi_image_free(data);
            } else {
                std::cerr << "Falha ao carregar textura do Skybox: " << path << std::endl;
            }
        }

        void SetupMesh() {
            float skyboxVertices[] = {
                -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
                 1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
            };

            glGenVertexArrays(1, &skyVAO);
            glGenBuffers(1, &skyVBO);
            glBindVertexArray(skyVAO);
            glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        }

        unsigned int skyVAO, skyVBO;
        unsigned int panoramaTexture;
        std::unique_ptr<Shader> shader;
    };
}
