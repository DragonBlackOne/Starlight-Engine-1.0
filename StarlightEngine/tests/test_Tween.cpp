#include <gtest/gtest.h>
#include "Tween.hpp"

using namespace starlight;

TEST(TweenTest, StartActivatesTween) {
    Tween tween;
    float value = 0.0f;
    tween.Start(&value, 0.0f, 100.0f, 1.0f);
    EXPECT_TRUE(tween.active);
    EXPECT_FLOAT_EQ(tween.startValue, 0.0f);
    EXPECT_FLOAT_EQ(tween.endValue, 100.0f);
}

TEST(TweenTest, UpdateInterpolatesLinear) {
    Tween tween;
    float value = 0.0f;
    tween.Start(&value, 0.0f, 100.0f, 1.0f);

    tween.Update(0.5f);
    EXPECT_FLOAT_EQ(value, 50.0f);
    EXPECT_TRUE(tween.active);
}

TEST(TweenTest, UpdateCompletesAtEnd) {
    Tween tween;
    float value = 0.0f;
    tween.Start(&value, 0.0f, 100.0f, 1.0f);

    tween.Update(1.0f);
    EXPECT_FLOAT_EQ(value, 100.0f);
    EXPECT_FALSE(tween.active);
}

TEST(TweenTest, UpdateClampsAtEndValue) {
    Tween tween;
    float value = 0.0f;
    tween.Start(&value, 0.0f, 100.0f, 1.0f);

    tween.Update(2.0f);
    EXPECT_FLOAT_EQ(value, 100.0f);
    EXPECT_FALSE(tween.active);
}

TEST(TweenTest, CustomEasing) {
    Tween tween;
    float value = 0.0f;
    tween.Start(&value, 0.0f, 100.0f, 1.0f, Easing::InQuad);

    tween.Update(0.5f);
    EXPECT_FLOAT_EQ(value, 100.0f * (0.5f * 0.5f));
    EXPECT_TRUE(tween.active);
}

TEST(TweenTest, MultiplePartialUpdates) {
    Tween tween;
    float value = 0.0f;
    tween.Start(&value, 0.0f, 200.0f, 2.0f);

    tween.Update(0.5f);
    EXPECT_FLOAT_EQ(value, 50.0f);
    tween.Update(0.5f);
    EXPECT_FLOAT_EQ(value, 100.0f);
    tween.Update(0.5f);
    EXPECT_FLOAT_EQ(value, 150.0f);
}

TEST(TweenTest, InactiveTweenDoesNothing) {
    Tween tween;
    float value = 42.0f;
    tween.active = false;
    tween.target = &value;
    tween.startValue = 0.0f;
    tween.endValue = 100.0f;
    tween.duration = 1.0f;

    tween.Update(0.5f);
    EXPECT_FLOAT_EQ(value, 42.0f);
}

TEST(TweenTest, NullTargetDoesNotCrash) {
    Tween tween;
    tween.Start(nullptr, 0.0f, 100.0f, 1.0f);
    tween.Update(0.5f);
}

TEST(TweenSystemTest, AddAndProcessTweens) {
    TweenSystem system;
    float v1 = 0.0f, v2 = 0.0f;

    Tween t1, t2;
    t1.Start(&v1, 0.0f, 100.0f, 1.0f);
    t2.Start(&v2, 0.0f, 200.0f, 2.0f);

    system.AddTween(t1);
    system.AddTween(t2);

    system.Update(0.5f);
    EXPECT_FLOAT_EQ(v1, 50.0f);
    EXPECT_FLOAT_EQ(v2, 50.0f);
}

TEST(TweenSystemTest, CompletedTweensAreRemoved) {
    TweenSystem system;
    float value = 0.0f;

    Tween tween;
    tween.Start(&value, 0.0f, 100.0f, 0.1f);
    system.AddTween(tween);

    system.Update(0.2f);
    EXPECT_FLOAT_EQ(value, 100.0f);
}

TEST(TweenSystemTest, MultipleTweensDifferentDurations) {
    TweenSystem system;
    float fast = 0.0f, slow = 0.0f;

    Tween t1, t2;
    t1.Start(&fast, 0.0f, 100.0f, 0.5f);
    t2.Start(&slow, 0.0f, 100.0f, 2.0f);
    system.AddTween(t1);
    system.AddTween(t2);

    system.Update(0.5f);
    EXPECT_FLOAT_EQ(fast, 100.0f);
    EXPECT_FLOAT_EQ(slow, 25.0f);

    system.Update(1.5f);
    EXPECT_FLOAT_EQ(slow, 100.0f);
}

TEST(TweenTest, ZeroDurationDoesNotNan) {
    Tween tween;
    float value = 0.0f;
    tween.Start(&value, 0.0f, 100.0f, 0.0f);
    EXPECT_TRUE(tween.active);
    
    tween.Update(0.1f);
    EXPECT_FLOAT_EQ(value, 100.0f);
    EXPECT_FALSE(tween.active);
    EXPECT_FALSE(std::isnan(value));
}

TEST(TweenSystemTest, ZeroDurationEcsTween) {
    entt::registry reg;
    entt::entity entity = reg.create();
    auto& trans = reg.emplace<TransformComponent>(entity);
    trans.position = glm::vec3(0.0f);
    
    TweenSystem system;
    EcsTween et;
    et.Start(entity, EcsTween::Type::Position, glm::vec3(0.0f), glm::vec3(10.0f, 20.0f, 30.0f), 0.0f);
    system.AddEcsTween(et);
    
    system.Update(0.1f, reg);
    
    auto& transUpdated = reg.get<TransformComponent>(entity);
    EXPECT_FLOAT_EQ(transUpdated.position.x, 10.0f);
    EXPECT_FLOAT_EQ(transUpdated.position.y, 20.0f);
    EXPECT_FLOAT_EQ(transUpdated.position.z, 30.0f);
}
