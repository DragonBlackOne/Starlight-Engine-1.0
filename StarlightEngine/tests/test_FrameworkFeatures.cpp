#include <gtest/gtest.h>
#include "Tween.hpp"
#include "Renderer2D.hpp"
#include "Components.hpp"
#include <glad/glad.h>

using namespace starlight;

TEST(FrameworkFeaturesTest, EcsTweenUpdatesTransform) {
    entt::registry reg;

    auto e = reg.create();
    auto& trans = reg.emplace<TransformComponent>(e);
    trans.position = glm::vec3(0.0f);
    trans.scale = glm::vec3(1.0f);

    TweenSystem tweenSys;
    EcsTween et;
    et.Start(e, EcsTween::Type::Position, glm::vec3(0.0f), glm::vec3(10.0f, 20.0f, 30.0f), 2.0f, Easing::Linear);
    tweenSys.AddEcsTween(et);

    // Update na metade (1.0s / 2.0s = 0.5)
    tweenSys.Update(1.0f, reg);

    EXPECT_NEAR(trans.position.x, 5.0f, 0.001f);
    EXPECT_NEAR(trans.position.y, 10.0f, 0.001f);
    EXPECT_NEAR(trans.position.z, 15.0f, 0.001f);

    // Update até o fim (mais 1.0s = total 2.0s)
    tweenSys.Update(1.0f, reg);
    EXPECT_NEAR(trans.position.x, 10.0f, 0.001f);
    EXPECT_NEAR(trans.position.y, 20.0f, 0.001f);
    EXPECT_NEAR(trans.position.z, 30.0f, 0.001f);
}

TEST(FrameworkFeaturesTest, EcsTweenDanglingEntityGracefulCleanup) {
    entt::registry reg;

    auto e = reg.create();
    reg.emplace<TransformComponent>(e);

    TweenSystem tweenSys;
    EcsTween et;
    et.Start(e, EcsTween::Type::Position, glm::vec3(0.0f), glm::vec3(10.0f), 1.0f, Easing::Linear);
    tweenSys.AddEcsTween(et);

    // Destruir entidade do registry
    reg.destroy(e);

    // Update não deve crashar e deve limpar o tween de forma segura
    EXPECT_NO_THROW(tweenSys.Update(0.5f, reg));
}

TEST(FrameworkFeaturesTest, Renderer2DDrawPrimitivesCompileCheck) {
    // Se glad não foi inicializado, pulamos chamadas do OpenGL
    if (glGenBuffers == nullptr) {
        GTEST_SKIP() << "No active OpenGL context. Skipping Renderer2D primitives execution test.";
        return;
    }

    Renderer2D::Initialize();
    Renderer2D::BeginBatch();
    Renderer2D::DrawLine({0.0f, 0.0f}, {100.0f, 100.0f}, 2.0f, {1.0f, 1.0f, 1.0f, 1.0f});
    Renderer2D::DrawCircle({50.0f, 50.0f}, 20.0f, {1.0f, 0.0f, 0.0f, 1.0f});
    Renderer2D::DrawString("Hello SDF", {0.0f, 0.0f}, 1.0f, {1.0f, 1.0f, 1.0f, 1.0f}, true);
    Renderer2D::DrawString("Hello Bitmap", {0.0f, 10.0f}, 1.0f, {1.0f, 1.0f, 1.0f, 1.0f}, false);
    Renderer2D::EndBatch();
    Renderer2D::Shutdown();
}
