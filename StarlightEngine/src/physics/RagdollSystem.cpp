#include "RagdollSystem.hpp"
#include "PhysicsSystem.hpp"
#include "AnimationComponent.hpp"
#include "Components.hpp"
#include "Engine.hpp"

namespace starlight {

    void RagdollSystem::OnUpdate(float dt) {
        if (Engine::IsInitialized()) {
            auto activeScene = Engine::Get().GetSceneStack().Active();
            if (activeScene) {
                Update(activeScene->GetRegistry(), dt);
            }
        }
    }

    void RagdollSystem::Update(entt::registry& registry, float dt) {
        (void)dt;
        if (!Engine::IsInitialized()) return;
        auto physicsSys = Engine::Get().GetSystem<PhysicsSystem>();
        if (!physicsSys || !physicsSys->GetSystem()) return;

        auto& bodyInterface = physicsSys->GetBodyInterface();

        auto view = registry.view<RagdollComponent, SkeletalAnimationComponent, TransformComponent>();
        for (auto entity : view) {
            auto& ragdoll = view.get<RagdollComponent>(entity);
            if (!ragdoll.active) continue;

            auto& anim = view.get<SkeletalAnimationComponent>(entity);
            auto& trans = view.get<TransformComponent>(entity);

            glm::mat4 invRoot = glm::inverse(trans.GetMatrix());

            for (const auto& bone : ragdoll.bones) {
                if (bone.jointIndex >= 0 && bone.jointIndex < (int)anim.boneMatrices.size()) {
                    if (!bone.bodyID.IsInvalid()) {
                        JPH::RVec3 pos = bodyInterface.GetPosition(bone.bodyID);
                        JPH::Quat rot = bodyInterface.GetRotation(bone.bodyID);
                        
                        glm::vec3 glmPos = glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ());
                        glm::quat glmRot = glm::quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ());

                        glm::mat4 boneWorld = glm::translate(glm::mat4(1.0f), glmPos) * glm::mat4_cast(glmRot);
                        glm::mat4 boneModel = invRoot * boneWorld;

                        if (ragdoll.blendWeight >= 1.0f) {
                            anim.boneMatrices[bone.jointIndex] = boneModel;
                        } else if (ragdoll.blendWeight > 0.0f) {
                            anim.boneMatrices[bone.jointIndex] = anim.boneMatrices[bone.jointIndex] * (1.0f - ragdoll.blendWeight) + boneModel * ragdoll.blendWeight;
                        }
                    }
                }
            }
        }
    }

}
