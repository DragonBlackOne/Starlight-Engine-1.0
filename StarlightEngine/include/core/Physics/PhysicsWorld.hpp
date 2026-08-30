#pragma once
#include "PhysicsCommon.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace starlight::physics {

class IPhysicsWorld {
public:
    virtual ~IPhysicsWorld() = default;

    virtual void Step(float dt) = 0;
    virtual void SetGravity(const glm::vec3& gravity) = 0;
    virtual glm::vec3 GetGravity() const = 0;

    virtual uint32_t CreateBody(const glm::vec3& position, const RigidBodyDesc& desc) = 0;
    virtual void DestroyBody(uint32_t bodyID) = 0;

    virtual void SetBodyPosition(uint32_t bodyID, const glm::vec3& position) = 0;
    virtual glm::vec3 GetBodyPosition(uint32_t bodyID) const = 0;

    virtual void SetBodyLinearVelocity(uint32_t bodyID, const glm::vec3& velocity) = 0;
    virtual glm::vec3 GetBodyLinearVelocity(uint32_t bodyID) const = 0;

    virtual void ApplyForce(uint32_t bodyID, const glm::vec3& force) = 0;
    virtual void ApplyImpulse(uint32_t bodyID, const glm::vec3& impulse) = 0;

    virtual bool RayCast(const glm::vec3& from, const glm::vec3& to, RayCastHit& outHit) = 0;
};

/**
 * @brief High-performance Mock/Reference Physics World for multi-platform CPU simulation & verification.
 */
class SimulationPhysicsWorld : public IPhysicsWorld {
public:
    struct SimulatedBody {
        uint32_t id = 0;
        glm::vec3 position{ 0.0f };
        glm::vec3 velocity{ 0.0f };
        glm::vec3 force{ 0.0f };
        RigidBodyDesc desc{};
        bool active = true;
    };

    SimulationPhysicsWorld() : m_gravity(0.0f, -9.81f, 0.0f) {}

    void Step(float dt) override {
        for (auto& body : m_bodies) {
            if (!body.active || body.desc.bodyType == BodyType::Static) continue;

            if (body.desc.bodyType == BodyType::Dynamic) {
                // Apply Gravity & Forces
                glm::vec3 totalAccel = (m_gravity * body.desc.gravityScale) + (body.force / std::max(body.desc.mass, 0.001f));
                body.velocity += totalAccel * dt;

                // Damping
                body.velocity *= (1.0f - std::clamp(body.desc.linearDamping * dt, 0.0f, 1.0f));

                // Position integration
                body.position += body.velocity * dt;

                // Reset instantaneous forces
                body.force = glm::vec3(0.0f);
            }
        }
    }

    void SetGravity(const glm::vec3& gravity) override { m_gravity = gravity; }
    glm::vec3 GetGravity() const override { return m_gravity; }

    uint32_t CreateBody(const glm::vec3& position, const RigidBodyDesc& desc) override {
        uint32_t id = static_cast<uint32_t>(m_bodies.size() + 1);
        SimulatedBody b;
        b.id = id;
        b.position = position;
        b.desc = desc;
        m_bodies.push_back(b);
        return id;
    }

    void DestroyBody(uint32_t bodyID) override {
        for (auto& body : m_bodies) {
            if (body.id == bodyID) {
                body.active = false;
                break;
            }
        }
    }

    void SetBodyPosition(uint32_t bodyID, const glm::vec3& position) override {
        for (auto& body : m_bodies) {
            if (body.id == bodyID && body.active) {
                body.position = position;
                break;
            }
        }
    }

    glm::vec3 GetBodyPosition(uint32_t bodyID) const override {
        for (const auto& body : m_bodies) {
            if (body.id == bodyID && body.active) {
                return body.position;
            }
        }
        return glm::vec3(0.0f);
    }

    void SetBodyLinearVelocity(uint32_t bodyID, const glm::vec3& velocity) override {
        for (auto& body : m_bodies) {
            if (body.id == bodyID && body.active) {
                body.velocity = velocity;
                break;
            }
        }
    }

    glm::vec3 GetBodyLinearVelocity(uint32_t bodyID) const override {
        for (const auto& body : m_bodies) {
            if (body.id == bodyID && body.active) {
                return body.velocity;
            }
        }
        return glm::vec3(0.0f);
    }

    void ApplyForce(uint32_t bodyID, const glm::vec3& force) override {
        for (auto& body : m_bodies) {
            if (body.id == bodyID && body.active && body.desc.bodyType == BodyType::Dynamic) {
                body.force += force;
                break;
            }
        }
    }

    void ApplyImpulse(uint32_t bodyID, const glm::vec3& impulse) override {
        for (auto& body : m_bodies) {
            if (body.id == bodyID && body.active && body.desc.bodyType == BodyType::Dynamic) {
                body.velocity += impulse / std::max(body.desc.mass, 0.001f);
                break;
            }
        }
    }

    bool RayCast(const glm::vec3& from, const glm::vec3& to, RayCastHit& outHit) override {
        glm::vec3 dir = to - from;
        float maxDist = glm::length(dir);
        if (maxDist < 0.0001f) return false;
        glm::vec3 rayDir = dir / maxDist;

        float closestDist = maxDist;
        bool found = false;

        for (const auto& body : m_bodies) {
            if (!body.active) continue;

            // Simplified sphere intersection for ray test
            glm::vec3 oc = from - body.position;
            float b = glm::dot(oc, rayDir);
            float c = glm::dot(oc, oc) - (body.desc.radius * body.desc.radius);
            float discriminant = b * b - c;

            if (discriminant >= 0.0f) {
                float t = -b - std::sqrt(discriminant);
                if (t >= 0.0f && t < closestDist) {
                    closestDist = t;
                    outHit.hasHit = true;
                    outHit.distance = t;
                    outHit.point = from + rayDir * t;
                    outHit.normal = glm::normalize(outHit.point - body.position);
                    outHit.bodyID = body.id;
                    found = true;
                }
            }
        }

        return found;
    }

private:
    glm::vec3 m_gravity;
    std::vector<SimulatedBody> m_bodies;
};

} // namespace starlight::physics
