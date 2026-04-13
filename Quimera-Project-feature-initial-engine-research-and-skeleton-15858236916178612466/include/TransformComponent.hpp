#pragma once
#include "godot/core/math/vector3.h"
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Vamos {
    struct Transform {
        Vector3 position;
        Vector3 rotation; // Euler angles in degrees
        Vector3 scale;

        entt::entity parent = entt::null;

        Transform(Vector3 pos = Vector3(0.0f, 0.0f, 0.0f),
                  Vector3 rot = Vector3(0.0f, 0.0f, 0.0f),
                  Vector3 scl = Vector3(1.0f, 1.0f, 1.0f))
            : position(pos), rotation(rot), scale(scl) {}

        glm::mat4 GetLocalMatrix() const {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
            model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
            model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
            model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
            model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));
            return model;
        }

        glm::mat4 GetWorldMatrix(const entt::registry& registry) const {
            if (parent == entt::null) {
                return GetLocalMatrix();
            }
            
            if (registry.valid(parent) && registry.all_of<Transform>(parent)) {
                return registry.get<Transform>(parent).GetWorldMatrix(registry) * GetLocalMatrix();
            }
            
            return GetLocalMatrix();
        }

        Vector3 GetForward() const {
            Vector3 forward;
            forward.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
            forward.y = sin(glm::radians(rotation.x));
            forward.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
            return forward.normalized();
        }
    };
}
