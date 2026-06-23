#include <gtest/gtest.h>
#include "LODSystem.hpp"
#include "LODComponent.hpp"
#include "Components.hpp"

using namespace starlight;

class LODSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        entity = registry.create();
        auto& tc = registry.emplace<TransformComponent>(entity);
        tc.position = glm::vec3(0.0f);

        auto& lod = registry.emplace<LODComponent>(entity);
        lod.AddLevel(nullptr, 10.0f);
        lod.AddLevel(nullptr, 50.0f);
        lod.AddLevel(nullptr, 100.0f);
    }

    entt::registry registry;
    entt::entity entity;
};

TEST_F(LODSystemTest, InitialLODLevelIsZero) {
    auto& lod = registry.get<LODComponent>(entity);
    EXPECT_EQ(lod.currentLevel, 0);
}

TEST_F(LODSystemTest, LODZeroAtClosestDistance) {
    LODSystem::Update(registry);
    auto& lod = registry.get<LODComponent>(entity);
    EXPECT_EQ(lod.currentLevel, 0);
}

TEST_F(LODSystemTest, EntityWithoutLODDoesNotCrash) {
    entt::registry reg;
    auto e = reg.create();
    reg.emplace<TransformComponent>(e);
    LODSystem::Update(reg);
}

TEST_F(LODSystemTest, EntityWithoutTransformDoesNotCrash) {
    entt::registry reg;
    auto e = reg.create();
    reg.emplace<LODComponent>(e);
    LODSystem::Update(reg);
}

TEST_F(LODSystemTest, EmptyRegistryDoesNotCrash) {
    entt::registry reg;
    LODSystem::Update(reg);
}

TEST(LODComponentTest, AddLevel) {
    LODComponent lod;
    EXPECT_EQ(lod.levels.size(), 0u);
    lod.AddLevel(nullptr, 10.0f);
    ASSERT_EQ(lod.levels.size(), 1u);
    EXPECT_FLOAT_EQ(lod.levels[0].distance, 10.0f);
}

TEST(LODComponentTest, MultipleLevels) {
    LODComponent lod;
    lod.AddLevel(nullptr, 10.0f);
    lod.AddLevel(nullptr, 50.0f);
    lod.AddLevel(nullptr, 100.0f);
    EXPECT_EQ(lod.levels.size(), 3u);
}

TEST(LODComponentTest, CurrentLevelDefaultsToZero) {
    LODComponent lod;
    EXPECT_EQ(lod.currentLevel, 0);
}

TEST(LODComponentTest, LevelDistancesAscending) {
    LODComponent lod;
    lod.AddLevel(nullptr, 10.0f);
    lod.AddLevel(nullptr, 50.0f);
    lod.AddLevel(nullptr, 100.0f);
    for (size_t i = 1; i < lod.levels.size(); i++) {
        EXPECT_GT(lod.levels[i].distance, lod.levels[i - 1].distance);
    }
}
