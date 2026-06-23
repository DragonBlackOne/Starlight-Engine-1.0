#include <gtest/gtest.h>
#include "MeshDeformSystem.hpp"
#include "Components.hpp"

using namespace starlight;

class MeshDeformSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        entity = registry.create();
        auto& t = registry.emplace<TransformComponent>(entity);
        t.UpdateLocalMatrix();
        auto& mdc = registry.emplace<MeshDeformComponent>(entity);
        mdc.originalVertices = {
            glm::vec3(-1, 0, 0),
            glm::vec3( 1, 0, 0),
            glm::vec3(-1, 2, 0),
            glm::vec3( 1, 2, 0)
        };
        mdc.currentOffsets.resize(4, glm::vec3(0.0f));
    }

    entt::registry registry;
    entt::entity entity;
};

TEST_F(MeshDeformSystemTest, ApplyDamageCreatesOffsets) {
    auto& mdc = registry.get<MeshDeformComponent>(entity);
    MeshDeformSystem::ApplyDamage(registry, entity, glm::vec3(0.5f, 0.5f, 0.0f), 10.0f);
    bool hasOffset = false;
    for (const auto& offset : mdc.currentOffsets) {
        if (glm::length(offset) > 0.0f) hasOffset = true;
    }
    EXPECT_TRUE(hasOffset);
}

TEST_F(MeshDeformSystemTest, DamageNearerVerticesGetMoreOffset) {
    auto& mdc = registry.get<MeshDeformComponent>(entity);
    MeshDeformSystem::ApplyDamage(registry, entity, glm::vec3(0.0f, 0.0f, 0.0f), 10.0f);

    float dist0 = glm::length(mdc.currentOffsets[0]);
    float dist3 = glm::length(mdc.currentOffsets[3]);
    EXPECT_GE(dist0, dist3);
}

TEST_F(MeshDeformSystemTest, ZeroImpulseNoDamage) {
    auto& mdc = registry.get<MeshDeformComponent>(entity);
    MeshDeformSystem::ApplyDamage(registry, entity, glm::vec3(0.5f, 0.5f, 0.0f), 0.0f);

    for (const auto& offset : mdc.currentOffsets) {
        EXPECT_FLOAT_EQ(glm::length(offset), 0.0f);
    }
}

TEST_F(MeshDeformSystemTest, RecoveryOverTime) {
    auto& mdc = registry.get<MeshDeformComponent>(entity);
    MeshDeformSystem::ApplyDamage(registry, entity, glm::vec3(0.5f, 0.5f, 0.0f), 20.0f);

    glm::vec3 offsetBefore = mdc.currentOffsets[2];
    MeshDeformSystem::Update(registry, 0.016f);
    glm::vec3 offsetAfter = mdc.currentOffsets[2];
    EXPECT_LT(glm::length(offsetAfter), glm::length(offsetBefore));
}

TEST_F(MeshDeformSystemTest, FullRecovery) {
    auto& mdc = registry.get<MeshDeformComponent>(entity);
    MeshDeformSystem::ApplyDamage(registry, entity, glm::vec3(0.5f, 0.5f, 0.0f), 10.0f);

    for (int i = 0; i < 1000; i++) {
        MeshDeformSystem::Update(registry, 0.016f);
    }

    for (const auto& offset : mdc.currentOffsets) {
        EXPECT_NEAR(glm::length(offset), 0.0f, 0.01f);
    }
}

TEST_F(MeshDeformSystemTest, StrengthScaleFactor) {
    auto entityDefault = registry.create();
    auto& tDef = registry.emplace<TransformComponent>(entityDefault);
    tDef.UpdateLocalMatrix();
    auto& def = registry.emplace<MeshDeformComponent>(entityDefault);
    def.originalVertices = { glm::vec3(1, 0, 0) };
    def.currentOffsets.resize(1, glm::vec3(0.0f));
    def.strength = 1.0f;

    auto entityStrong = registry.create();
    auto& tStr = registry.emplace<TransformComponent>(entityStrong);
    tStr.UpdateLocalMatrix();
    auto& str = registry.emplace<MeshDeformComponent>(entityStrong);
    str.originalVertices = { glm::vec3(1, 0, 0) };
    str.currentOffsets.resize(1, glm::vec3(0.0f));
    str.strength = 2.0f;

    MeshDeformSystem::ApplyDamage(registry, entityDefault, glm::vec3(0.0f, 0.0f, 0.0f), 10.0f);
    MeshDeformSystem::ApplyDamage(registry, entityStrong, glm::vec3(0.0f, 0.0f, 0.0f), 10.0f);

    EXPECT_GT(glm::length(str.currentOffsets[0]), glm::length(def.currentOffsets[0]));
}

TEST_F(MeshDeformSystemTest, UnknownEntityDoesNotCrash) {
    MeshDeformSystem::ApplyDamage(registry, entt::null, glm::vec3(0, 0, 0), 10.0f);
}

TEST_F(MeshDeformSystemTest, UpdateWithoutDamageDoesNothing) {
    auto& mdc = registry.get<MeshDeformComponent>(entity);
    MeshDeformSystem::Update(registry, 1.0f);
    for (const auto& offset : mdc.currentOffsets) {
        EXPECT_FLOAT_EQ(glm::length(offset), 0.0f);
    }
}

TEST_F(MeshDeformSystemTest, EmptyRegistryUpdate) {
    entt::registry reg;
    MeshDeformSystem::Update(reg, 1.0f);
}

TEST_F(MeshDeformSystemTest, EmptyRegistryApplyDamage) {
    entt::registry reg;
    MeshDeformSystem::ApplyDamage(reg, reg.create(), glm::vec3(0, 0, 0), 10.0f);
}
