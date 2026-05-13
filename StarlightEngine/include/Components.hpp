#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <map>
#pragma warning(push, 0)
#include <codeanalysis/warnings.h>
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#pragma warning(pop)
#include <vector>
#include <memory>
#include "Shader.hpp"

namespace starlight {
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
        uint32_t textureID = 0; // Legacy / Fallback
        bool useTexture = false;
        
        // PBR Properties
        glm::vec3 albedo = {1.0f, 1.0f, 1.0f};
        float metallic = 0.0f;
        float roughness = 0.5f;
        float ao = 1.0f;
        
        // PBR Texture Maps (0 = none)
        uint32_t albedoMap = 0;
        uint32_t normalMap = 0;
        uint32_t metallicMap = 0;
        uint32_t roughnessMap = 0;
        uint32_t aoMap = 0;

        bool isPBR = false;
        bool isTransparent = false;

        void Apply() {
            if (!shader) return;
            shader->Use();
            
            if (isPBR) {
                // Apply PBR uniform values
                shader->SetVec3U("albedo", albedo);
                shader->SetFloatU("metallic", metallic);
                shader->SetFloatU("roughness", roughness);
                shader->SetFloatU("ao", ao);

                // Bind PBR Maps if they exist
                int textureUnit = 0;

                shader->SetIntU("hasAlbedoMap", albedoMap ? 1 : 0);
                if (albedoMap) {
                    glActiveTexture(GL_TEXTURE0 + textureUnit);
                    glBindTexture(GL_TEXTURE_2D, albedoMap);
                    shader->SetIntU("albedoMap", textureUnit);
                    textureUnit++;
                }

                shader->SetIntU("hasNormalMap", normalMap ? 1 : 0);
                if (normalMap) {
                    glActiveTexture(GL_TEXTURE0 + textureUnit);
                    glBindTexture(GL_TEXTURE_2D, normalMap);
                    shader->SetIntU("normalMap", textureUnit);
                    textureUnit++;
                }

                shader->SetIntU("hasMetallicMap", metallicMap ? 1 : 0);
                if (metallicMap) {
                    glActiveTexture(GL_TEXTURE0 + textureUnit);
                    glBindTexture(GL_TEXTURE_2D, metallicMap);
                    shader->SetIntU("metallicMap", textureUnit);
                    textureUnit++;
                }

                shader->SetIntU("hasRoughnessMap", roughnessMap ? 1 : 0);
                if (roughnessMap) {
                    glActiveTexture(GL_TEXTURE0 + textureUnit);
                    glBindTexture(GL_TEXTURE_2D, roughnessMap);
                    shader->SetIntU("roughnessMap", textureUnit);
                    textureUnit++;
                }

                shader->SetIntU("hasAOMap", aoMap ? 1 : 0);
                if (aoMap) {
                    glActiveTexture(GL_TEXTURE0 + textureUnit);
                    glBindTexture(GL_TEXTURE_2D, aoMap);
                    shader->SetIntU("aoMap", textureUnit);
                    textureUnit++;
                }
            } else {
                // Legacy Rendering
                shader->SetVec3U("uColor", color);
                shader->SetIntU("uUseTexture", useTexture ? 1 : 0);
                if (useTexture) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, textureID);
                    shader->SetIntU("uTexture", 0);
                }
            }
        }
    };

    struct MeshComponent {
        Material material;
        std::shared_ptr<Mesh> mesh;
        float boundingRadius = 1.0f;
        bool isVisible = true;
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
