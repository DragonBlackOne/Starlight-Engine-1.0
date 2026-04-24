// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <map>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <vector>
#include <memory>
#include "Shader.hpp"

namespace titan {
    class Mesh;

    struct TransformComponent {
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 scale = {1.0f, 1.0f, 1.0f};

        glm::mat4 localMatrix = glm::mat4(1.0f);
        glm::mat4 worldMatrix = glm::mat4(1.0f);

        void UpdateLocalMatrix() {
            localMatrix = glm::translate(glm::mat4(1.0f), position);
            localMatrix *= glm::mat4_cast(rotation); 
            localMatrix = glm::scale(localMatrix, scale);
        }

        glm::mat4 GetMatrix() const {
            return worldMatrix;
        }
    };

    struct CameraComponent {
        float fov = 60.0f;
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;
        bool primary = true;

        glm::mat4 projection;
        glm::mat4 view;
    };

    struct Material {
        std::shared_ptr<Shader> shader;
        // Basic properties
        glm::vec3 color = {1.0f, 1.0f, 1.0f};
        uint32_t textureID = 0;
        bool useTexture = false;
        
        // PBR Properties
        glm::vec3 albedo = {1.0f, 1.0f, 1.0f};
        float metallic = 0.0f;
        float roughness = 0.5f;
        float ao = 1.0f;
        
        bool isPBR = false;

        void Apply() {
            if (!shader) return;
            shader->Use();
            
            if (isPBR) {
                shader->SetVec3("albedo", albedo);
                shader->SetFloat("metallic", metallic);
                shader->SetFloat("roughness", roughness);
                shader->SetFloat("ao", ao);
            } else {
                shader->SetVec3("uColor", color);
                shader->SetInt("uUseTexture", useTexture ? 1 : 0);
                if (useTexture) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, textureID);
                    shader->SetInt("uTexture", 0);
                }
            }
        }
    };

    struct MeshComponent {
        std::shared_ptr<Mesh> mesh;
        Material material;
        float boundingRadius = 1.0f; // Para Frustum Culling esfÃ©rico rÃ¡pido
        bool isVisible = true;       // Escrita atÃ´mica ou bool para culling multithread
    };

    struct PointLightComponent {
        glm::vec3 color = {1.0f, 1.0f, 1.0f};
        float intensity = 100.0f;
    };

    struct PhysicsComponent {
        JPH::BodyID bodyID;
        PhysicsComponent(JPH::BodyID id) : bodyID(id) {}
        PhysicsComponent() : bodyID(JPH::BodyID()) {}
    };

    struct RetroComponent {
        float map_x = 0.0f;
        float map_y = 0.0f;
        float map_z = 0.5f;
        float horizon = 0.5f;
        float angle = 0.0f;
        float pitch = 1.0f;
        bool active = true;
        
        glm::vec3 skyColor = {0.05f, 0.05f, 0.1f};
        glm::vec3 groundColor1 = {0.2f, 0.5f, 0.2f};
        glm::vec3 groundColor2 = {0.3f, 0.6f, 0.3f};
    };
    struct TagComponent {
        std::string tag;
        TagComponent(const std::string& t) : tag(t) {}
        TagComponent() : tag("Entity") {}
    };

    struct RelationshipComponent {
        entt::entity parent = entt::null;
        std::vector<entt::entity> children;
    };
}
