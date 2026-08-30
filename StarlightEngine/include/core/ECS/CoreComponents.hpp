#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>

namespace starlight::ecs {

struct UUIDComponent {
    uint64_t uuid = 0;
    UUIDComponent(uint64_t id = 0) : uuid(id) {}
    operator uint64_t() const { return uuid; }
};

struct TagComponent {
    std::string tag = "Entity";
    TagComponent() = default;
    TagComponent(const std::string& t) : tag(t) {}
};

struct TransformComponent {
    glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f }; // W, X, Y, Z
    glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

    glm::mat4 localMatrix{ 1.0f };
    glm::mat4 worldMatrix{ 1.0f };
    bool isDirty = true;

    TransformComponent() = default;
    TransformComponent(const glm::vec3& pos) : position(pos) { UpdateLocalMatrix(); }

    void SetPosition(const glm::vec3& pos) {
        position = pos;
        isDirty = true;
    }

    void SetRotation(const glm::quat& rot) {
        rotation = rot;
        isDirty = true;
    }

    void SetScale(const glm::vec3& s) {
        scale = s;
        isDirty = true;
    }

    void UpdateLocalMatrix() {
        localMatrix = glm::translate(glm::mat4(1.0f), position) *
                      glm::mat4_cast(rotation) *
                      glm::scale(glm::mat4(1.0f), scale);
    }

    glm::mat4 GetMatrix() const { return worldMatrix; }
};

struct RelationshipComponent {
    entt::entity parent{ entt::null };
    std::vector<entt::entity> children;

    RelationshipComponent() = default;
};

} // namespace starlight::ecs
