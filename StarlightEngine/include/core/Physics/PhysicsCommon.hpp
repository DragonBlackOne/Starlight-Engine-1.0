#pragma once
#include <glm/glm.hpp>
#include <cstdint>
#include <string>

namespace starlight::physics {

enum class BodyType {
    Static = 0,
    Kinematic,
    Dynamic
};

enum class ShapeType {
    Box = 0,
    Sphere,
    Capsule,
    Cylinder,
    Mesh
};

struct RayCastHit {
    bool hasHit = false;
    glm::vec3 point{ 0.0f };
    glm::vec3 normal{ 0.0f, 1.0f, 0.0f };
    float distance = 0.0f;
    uint32_t bodyID = 0;
};

struct RigidBodyDesc {
    BodyType bodyType = BodyType::Dynamic;
    ShapeType shapeType = ShapeType::Box;
    glm::vec3 halfExtents{ 0.5f, 0.5f, 0.5f };
    float radius = 0.5f;
    float height = 1.0f;
    float mass = 1.0f;
    float friction = 0.5f;
    float restitution = 0.1f;
    float linearDamping = 0.05f;
    float angularDamping = 0.05f;
    float gravityScale = 1.0f;
    bool isSensor = false;
    uint16_t collisionLayer = 1;
    uint16_t collisionMask = 0xFFFF;
};

} // namespace starlight::physics
