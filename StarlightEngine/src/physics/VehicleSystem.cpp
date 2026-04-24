// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "VehicleSystem.hpp"
#include "Components.hpp"
#include "PhysicsSystem.hpp"
#include "Engine.hpp"
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

namespace starlight {

    void VehicleSystem::Update(entt::registry& registry, float dt) {
        (void)dt;
        auto view = registry.view<VehicleComponent, TransformComponent, PhysicsComponent>();
        auto* physics = Engine::Get().GetPhysics().GetSystem();

        for (auto entity : view) {
            auto& t = view.get<TransformComponent>(entity);
            auto& v = view.get<VehicleComponent>(entity);

            for (auto& wheel : v.wheels) {
                // World position of the wheel attachment
                glm::vec3 worldWheelPos = glm::vec3(t.worldMatrix * glm::vec4(wheel.attachmentPoint, 1.0f));
                glm::vec3 down = glm::vec3(t.worldMatrix * glm::vec4(0, -1, 0, 0));

                JPH::RRayCast ray{ 
                    JPH::Vec3(worldWheelPos.x, worldWheelPos.y, worldWheelPos.z), 
                    JPH::Vec3(down.x * wheel.suspensionLength, down.y * wheel.suspensionLength, down.z * wheel.suspensionLength) 
                };
                
                JPH::RayCastResult result;
                bool hit = physics->GetNarrowPhaseQuery().CastRay(ray, result);

                if (hit) {
                    wheel.isGrounded = true;
                    // Apply suspension force to body...
                    // Update wheel model pos for renderer
                    float hitDist = result.mFraction * wheel.suspensionLength;
                    wheel.transform = glm::translate(glm::mat4(1.0f), wheel.attachmentPoint + glm::vec3(0, -hitDist, 0));
                } else {
                    wheel.isGrounded = false;
                    wheel.transform = glm::translate(glm::mat4(1.0f), wheel.attachmentPoint + glm::vec3(0, -wheel.suspensionLength, 0));
                }
            }
        }
    }

}
