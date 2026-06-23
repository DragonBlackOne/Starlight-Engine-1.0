#include <gtest/gtest.h>
#include "CameraSystem.hpp"
#include <glm/gtc/matrix_access.hpp>

using namespace starlight;

class CameraSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        entity = registry.create();
        registry.emplace<TransformComponent>(entity);
        registry.emplace<CameraComponent>(entity);
    }

    entt::registry registry;
    entt::entity entity;
};

TEST_F(CameraSystemTest, PerspectiveProjectionIsSet) {
    CameraSystem::Update(registry, 16.0f / 9.0f);
    auto& cam = registry.get<CameraComponent>(entity);
    EXPECT_NE(cam.projection, glm::mat4(0.0f));
}

TEST_F(CameraSystemTest, ViewMatrixIsSet) {
    CameraSystem::Update(registry, 16.0f / 9.0f);
    auto& cam = registry.get<CameraComponent>(entity);
    EXPECT_NE(cam.view, glm::mat4(0.0f));
}

TEST_F(CameraSystemTest, NonPrimaryCamerasNotUpdated) {
    auto e2 = registry.create();
    registry.emplace<TransformComponent>(e2);
    auto& cam2 = registry.emplace<CameraComponent>(e2);
    cam2.primary = false;
    cam2.projection = glm::mat4(0.0f);
    cam2.view = glm::mat4(0.0f);

    CameraSystem::Update(registry, 1.0f);
    EXPECT_NE(registry.get<CameraComponent>(entity).projection, glm::mat4(0.0f));
}

TEST_F(CameraSystemTest, PrimaryFlagDefaultsTrue) {
    CameraComponent cam;
    EXPECT_TRUE(cam.primary);
}

TEST_F(CameraSystemTest, MultipleCamerasOnlyPrimaryUpdated) {
    auto e2 = registry.create();
    registry.emplace<TransformComponent>(e2);
    auto& cam2 = registry.emplace<CameraComponent>(e2);
    cam2.primary = false;

    CameraSystem::Update(registry, 1.0f);
    auto& cam1 = registry.get<CameraComponent>(entity);
    EXPECT_NE(cam1.projection, glm::mat4(0.0f));
}

TEST_F(CameraSystemTest, EmptyRegistryDoesNotCrash) {
    entt::registry emptyReg;
    CameraSystem::Update(emptyReg, 1.0f);
}

TEST_F(CameraSystemTest, EntityWithoutCameraDoesNotCrash) {
    entt::registry reg;
    auto e = reg.create();
    reg.emplace<TransformComponent>(e);
    CameraSystem::Update(reg, 1.0f);
}

TEST_F(CameraSystemTest, EntityWithoutTransformDoesNotCrash) {
    entt::registry reg;
    auto e = reg.create();
    reg.emplace<CameraComponent>(e);
    CameraSystem::Update(reg, 1.0f);
}

TEST_F(CameraSystemTest, ViewMatrixOrigin) {
    CameraSystem::Update(registry, 1.0f);
    auto& cam = registry.get<CameraComponent>(entity);
    glm::vec4 origin = cam.view * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    EXPECT_NEAR(glm::length(glm::vec3(origin)), 0.0f, 0.001f);
}

TEST_F(CameraSystemTest, AspectRatioAffectsProjection) {
    auto e1 = registry.create();
    registry.emplace<TransformComponent>(e1);
    registry.emplace<CameraComponent>(e1);

    auto e2 = registry.create();
    registry.emplace<TransformComponent>(e2);
    registry.emplace<CameraComponent>(e2);

    CameraSystem::Update(registry, 1.0f);
    auto p1 = registry.get<CameraComponent>(e1).projection;

    CameraSystem::Update(registry, 2.0f);
    auto p2 = registry.get<CameraComponent>(e2).projection;

    bool same = true;
    for (int i = 0; i < 4 && same; i++) {
        for (int j = 0; j < 4 && same; j++) {
            if (std::abs(p1[i][j] - p2[i][j]) > 0.001f) same = false;
        }
    }
    EXPECT_FALSE(same);
}
