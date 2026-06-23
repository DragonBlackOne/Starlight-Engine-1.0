#include <gtest/gtest.h>
#include "RagdollSystem.hpp"
#include "AnimationComponent.hpp"
#include "Components.hpp"

using namespace starlight;

TEST(RagdollSystemTest, GracefulNullPhysicsReturn) {
    entt::registry reg;

    auto e = reg.create();
    auto& trans = reg.emplace<TransformComponent>(e);
    trans.position = glm::vec3(0.0f);

    auto& anim = reg.emplace<SkeletalAnimationComponent>(e);
    anim.boneMatrices.resize(1);
    anim.boneMatrices[0] = glm::mat4(1.0f);

    auto& ragdoll = reg.emplace<RagdollComponent>(e);
    ragdoll.active = true;
    
    RagdollBone bone;
    bone.jointIndex = 0;
    ragdoll.bones.push_back(bone);

    // Should return gracefully since PhysicsSystem/Engine is not active in this test runner
    EXPECT_NO_THROW(RagdollSystem::Update(reg, 0.016f));
}

TEST(RagdollSystemTest, DeactivatedRagdollSkipped) {
    entt::registry reg;

    auto e = reg.create();
    auto& trans = reg.emplace<TransformComponent>(e);
    trans.position = glm::vec3(0.0f);

    auto& anim = reg.emplace<SkeletalAnimationComponent>(e);
    anim.boneMatrices.resize(1);
    anim.boneMatrices[0] = glm::mat4(1.0f);

    auto& ragdoll = reg.emplace<RagdollComponent>(e);
    ragdoll.active = false; // Disabled
    
    RagdollBone bone;
    bone.jointIndex = 0;
    ragdoll.bones.push_back(bone);

    EXPECT_NO_THROW(RagdollSystem::Update(reg, 0.016f));
}
