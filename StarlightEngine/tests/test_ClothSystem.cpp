#include <gtest/gtest.h>
#include <cmath>
#include "ClothSystem.hpp"

using namespace starlight;

class ClothSystemTest : public ::testing::Test {
protected:
    static bool AllPositionsFinite(const std::vector<ClothParticle>& particles) {
        for (auto& p : particles) {
            if (!std::isfinite(p.position.x) || !std::isfinite(p.position.y) || !std::isfinite(p.position.z))
                return false;
        }
        return true;
    }

    void SetUp() override {
        entity = registry.create();
        auto& cloth = registry.emplace<ClothComponent>(entity);
        cloth.width = 2;
        cloth.height = 2;

        for (int z = 0; z < 2; z++) {
            for (int x = 0; x < 2; x++) {
                ClothParticle p;
                p.position = glm::vec3((float)x, 5.0f, (float)z);
                p.oldPosition = p.position;
                cloth.particles.push_back(p);
            }
        }

        cloth.constraints.push_back({0, 1, 1.0f});
        cloth.constraints.push_back({1, 3, 1.0f});
        cloth.constraints.push_back({3, 2, 1.0f});
        cloth.constraints.push_back({2, 0, 1.0f});
    }

    entt::registry registry;
    entt::entity entity;
};

TEST_F(ClothSystemTest, UpdateDoesNotCrash) {
    ClothSystem::Update(registry, 0.016f);
}

TEST_F(ClothSystemTest, PositionsStayFiniteAfterSingleFrame) {
    ClothSystem::Update(registry, 0.016f);
    auto& cloth = registry.get<ClothComponent>(entity);
    EXPECT_TRUE(AllPositionsFinite(cloth.particles));
}

TEST_F(ClothSystemTest, AllParticlesPinnedDontMove) {
    auto& cloth = registry.get<ClothComponent>(entity);
    for (auto& p : cloth.particles) p.pinned = true;
    auto initialPositions = cloth.particles;

    for (int i = 0; i < 10; i++) {
        ClothSystem::Update(registry, 0.016f);
    }

    EXPECT_TRUE(AllPositionsFinite(cloth.particles));
    for (size_t i = 0; i < cloth.particles.size(); i++) {
        EXPECT_FLOAT_EQ(cloth.particles[i].position.x, initialPositions[i].position.x);
        EXPECT_FLOAT_EQ(cloth.particles[i].position.y, initialPositions[i].position.y);
    }
}

TEST_F(ClothSystemTest, PinnedParticleStaysInPlace) {
    auto& cloth = registry.get<ClothComponent>(entity);
    cloth.particles[0].pinned = true;
    glm::vec3 initialPos = cloth.particles[0].position;

    ClothSystem::Update(registry, 0.016f);

    EXPECT_TRUE(AllPositionsFinite(cloth.particles));
    EXPECT_FLOAT_EQ(cloth.particles[0].position.x, initialPos.x);
    EXPECT_FLOAT_EQ(cloth.particles[0].position.y, initialPos.y);
}

TEST_F(ClothSystemTest, ZeroDtDoesNothing) {
    auto& cloth = registry.get<ClothComponent>(entity);
    auto positions = cloth.particles;

    ClothSystem::Update(registry, 0.0f);

    for (size_t i = 0; i < cloth.particles.size(); i++) {
        EXPECT_FLOAT_EQ(cloth.particles[i].position.x, positions[i].position.x);
    }
}

TEST_F(ClothSystemTest, EmptyRegistryDoesNotCrash) {
    entt::registry reg;
    ClothSystem::Update(reg, 1.0f);
}

TEST_F(ClothSystemTest, EntityWithoutClothDoesNotCrash) {
    entt::registry reg;
    entt::entity e = reg.create();
    (void)e;
    ClothSystem::Update(reg, 1.0f);
}

TEST(ClothComponentTest, DefaultParticleNotPinned) {
    ClothParticle p;
    EXPECT_FALSE(p.pinned);
}

TEST(ClothComponentTest, ParticleDefaultPositionIsZero) {
    ClothParticle p;
    EXPECT_FLOAT_EQ(p.position.x, 0.0f);
    EXPECT_FLOAT_EQ(p.position.y, 0.0f);
    EXPECT_FLOAT_EQ(p.position.z, 0.0f);
}

TEST(ClothComponentTest, ConstraintRestLength) {
    ClothConstraint c{0, 1, 2.5f};
    EXPECT_EQ(c.p1, 0);
    EXPECT_EQ(c.p2, 1);
    EXPECT_FLOAT_EQ(c.restLength, 2.5f);
}
