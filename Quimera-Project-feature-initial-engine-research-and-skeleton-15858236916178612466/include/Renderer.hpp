// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include "RenderBackend.hpp"
#include <GL/glew.h>
#include <iostream>
#include "Shader.hpp"
#include "Skybox.hpp"
#include "MaterialComponent.hpp"
#include <filesystem>
#include "ShadowMap.hpp"

namespace Vamos {
    class GLRenderer : public IRenderer {
    public:
        void Initialize() override {
            std::cout << "[RENDERER] Init: glewInit" << std::endl; std::cout.flush();
            if (glewInit() != GLEW_OK) {
                std::cerr << "Falha ao inicializar GLEW" << std::endl; std::cerr.flush();
                return;
            }
            glEnable(GL_DEPTH_TEST);

            std::cout << "[RENDERER] Init: defaultShader" << std::endl; std::cout.flush();
            defaultShader = std::make_unique<Shader>("assets/shaders/default.vert", "assets/shaders/default.frag");
            
            std::cout << "[RENDERER] Init: shadowMap" << std::endl; std::cout.flush();
            shadowMap = std::make_unique<ShadowMap>();
            shadowMap->Initialize();

            std::cout << "[RENDERER] Init: skybox" << std::endl; std::cout.flush();
            // Tenta carregar o skybox padrão se existir
            std::string skyPath = "assets/textures/skybox_panorama.png";
            if (std::filesystem::exists(skyPath)) {
                skybox = std::make_unique<Skybox>(skyPath);
            }
            std::cout << "[RENDERER] Init: Finalizado" << std::endl;
        }

        void Render(const RenderView& view) override {
            // 1. Shadow Pass
            glm::vec3 lightDir = glm::normalize(glm::vec3(-10.0f, -10.0f, -10.0f));
            shadowMap->BeginShadowPass(lightDir, glm::vec3(0.0f));
            
            for (const auto& cmd : view.commands) {
                if (!cmd.mesh->initialized) SetupMesh(*cmd.mesh);
                shadowMap->RenderMesh(cmd.model);
                
                glBindVertexArray(cmd.mesh->vao);
                if (!cmd.mesh->lods.empty() && cmd.mesh->lodEbos[0] != 0) {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cmd.mesh->lodEbos[0]);
                    glDrawElements(GL_TRIANGLES, (GLsizei)cmd.mesh->lods[0].size(), GL_UNSIGNED_INT, 0);
                } else {
                    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(cmd.mesh->vertices.size() / 5));
                }
            }
            shadowMap->EndShadowPass(800, 600); // TODO: pass actual viewport size

            // 2. Main Pass
            glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (skybox) {
                skybox->Render(view.viewMatrix, view.projectionMatrix);
            }

            if (!defaultShader) return;
            defaultShader->Use();
            defaultShader->SetMat4("view", view.viewMatrix);
            defaultShader->SetMat4("projection", view.projectionMatrix);
            
            // Luz e Sombra
            defaultShader->SetVec3("u_LightPos", glm::vec3(10.0f, 10.0f, 10.0f));
            defaultShader->SetVec3("u_ViewPos", glm::vec3(0.0f, 0.0f, 5.0f)); // TODO: Get from view
            defaultShader->SetMat4("u_LightSpaceMatrix", shadowMap->GetLightSpaceMatrix());
            
            shadowMap->BindShadowMapTexture(5);
            defaultShader->SetInt("u_ShadowMap", 5);

            for (const auto& cmd : view.commands) {
                if (!cmd.mesh->initialized) {
                    SetupMesh(*cmd.mesh);
                }

                int lodIndex = 0; 
                if (lodIndex >= (int)cmd.mesh->lods.size()) lodIndex = 0;

                // Aplicar Material PBR
                if (cmd.material) {
                    defaultShader->SetVec4("u_BaseColor", cmd.material->baseColor);
                    defaultShader->SetBool("u_HasAlbedoMap", cmd.material->hasAlbedo);
                    defaultShader->SetFloat("u_Metallic", cmd.material->metallic);
                    defaultShader->SetFloat("u_Roughness", cmd.material->roughness);
                    defaultShader->SetFloat("u_AO", cmd.material->ao);
                    
                    // Fallback para legado
                    defaultShader->SetVec4("u_Color", cmd.material->color);
                    defaultShader->SetBool("u_HasTexture", cmd.material->hasTexture);
                }

                // Aplicar Animação
                defaultShader->SetBool("u_HasAnimation", cmd.hasAnimation);
                if (cmd.hasAnimation && !cmd.skinningMatrices.empty()) {
                    defaultShader->SetMat4Array("u_BoneMatrices", cmd.skinningMatrices);
                }

                defaultShader->SetMat4("model", cmd.model);
                glBindVertexArray(cmd.mesh->vao);
                
                if (!cmd.mesh->lods.empty() && cmd.mesh->lodEbos[lodIndex] != 0) {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cmd.mesh->lodEbos[lodIndex]);
                    glDrawElements(GL_TRIANGLES, (GLsizei)cmd.mesh->lods[lodIndex].size(), GL_UNSIGNED_INT, 0);
                } else {
                    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(cmd.mesh->vertices.size() / 5));
                }
            }
        }

        void Shutdown() override {
            std::cout << "[Renderer] OpenGL Desligado" << std::endl;
        }

    private:
        void SetupMesh(Mesh& mesh) {
            if(mesh.vao == 0) glGenVertexArrays(1, &mesh.vao);
            if(mesh.vbo == 0) glGenBuffers(1, &mesh.vbo);
            if(mesh.boneVbo == 0) glGenBuffers(1, &mesh.boneVbo);

            glBindVertexArray(mesh.vao);
            
            // VBO Principal (Vértices + UVs)
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
            glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            // Bone Data VBO
            if (!mesh.boneData.empty()) {
                glBindBuffer(GL_ARRAY_BUFFER, mesh.boneVbo);
                glBufferData(GL_ARRAY_BUFFER, mesh.boneData.size() * sizeof(Mesh::BoneData), mesh.boneData.data(), GL_STATIC_DRAW);

                // Bone IDs (Location 2 - ivec4)
                glVertexAttribIPointer(2, 4, GL_UNSIGNED_INT, sizeof(Mesh::BoneData), (void*)0);
                glEnableVertexAttribArray(2);
                
                // Bone Weights (Location 3 - vec4)
                glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Mesh::BoneData), (void*)(4 * sizeof(unsigned int)));
                glEnableVertexAttribArray(3);
            }

            // Gerar EBOs para cada LOD
            for (size_t i = 0; i < mesh.lods.size() && i < 5; i++) {
                if (mesh.lodEbos[i] == 0) glGenBuffers(1, &mesh.lodEbos[i]);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.lodEbos[i]);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.lods[i].size() * sizeof(unsigned int), mesh.lods[i].data(), GL_STATIC_DRAW);
            }

            mesh.initialized = true;
        }

        std::unique_ptr<Shader> defaultShader;
        std::unique_ptr<Skybox> skybox;
        std::unique_ptr<ShadowMap> shadowMap;
    };
}
