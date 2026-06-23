#include "FootIK.hpp"
#include "Engine.hpp"
#include "Components.hpp"
#include "PhysicsSystem.hpp"
#pragma warning(push, 0)
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/RayCast.h>
#pragma warning(pop)

namespace starlight {

    FootIKSystem::FootIKSystem() {}
    FootIKSystem::~FootIKSystem() {}

    void FootIKSystem::OnUpdate(float dt) {
        (void)dt;
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (activeScene) {
            ResolveIK(activeScene->GetRegistry());
        }
    }

    void FootIKSystem::ResolveIK(entt::registry& reg) {
        auto physicsSys = Engine::Get().GetSystem<PhysicsSystem>();
        if (!physicsSys || !physicsSys->GetSystem()) return;

        JPH::PhysicsSystem* physics = physicsSys->GetSystem();
        const JPH::NarrowPhaseQuery& query = physics->GetNarrowPhaseQuery();

        auto view = reg.view<FootIKComponent, TransformComponent>();
        view.each([&query](auto& ik, const auto& trans) {
            if (!ik.enabled) return;

            // Firing Jolt raycasts vertically down from knee height above foot positions
            glm::vec3 leftHip = trans.position + glm::vec3(-0.3f, 0.5f, 0.0f);
            glm::vec3 rightHip = trans.position + glm::vec3(0.3f, 0.5f, 0.0f);

            // Left Foot Raycast
            {
                JPH::RRayCast ray(
                    JPH::Vec3(leftHip.x, leftHip.y, leftHip.z),
                    JPH::Vec3(0.0f, -2.0f, 0.0f)
                );
                JPH::RayCastResult hit;
                if (query.CastRay(ray, hit, JPH::BroadPhaseLayerFilter(), JPH::ObjectLayerFilter(), JPH::BodyFilter())) {
                    float hitY = leftHip.y - hit.mFraction * 2.0f;
                    ik.leftFootOffset.y = hitY - trans.position.y;
                } else {
                    ik.leftFootOffset = glm::vec3(0.0f);
                }
            }

            // Right Foot Raycast
            {
                JPH::RRayCast ray(
                    JPH::Vec3(rightHip.x, rightHip.y, rightHip.z),
                    JPH::Vec3(0.0f, -2.0f, 0.0f)
                );
                JPH::RayCastResult hit;
                if (query.CastRay(ray, hit, JPH::BroadPhaseLayerFilter(), JPH::ObjectLayerFilter(), JPH::BodyFilter())) {
                    float hitY = rightHip.y - hit.mFraction * 2.0f;
                    ik.rightFootOffset.y = hitY - trans.position.y;
                } else {
                    ik.rightFootOffset = glm::vec3(0.0f);
                }
            }
        });
    }

}
