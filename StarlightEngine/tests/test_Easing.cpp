#include <gtest/gtest.h>
#include "Tween.hpp"

using namespace starlight;

struct EasingFunc {
    const char* name;
    std::function<float(float)> func;
    bool canOvershoot = false;
};

class EasingTest : public ::testing::TestWithParam<EasingFunc> {
protected:
    static constexpr float kInputs[] = {
        0.0f, 0.001f, 0.01f, 0.05f, 0.1f, 0.15f, 0.2f, 0.25f,
        0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.75f, 0.8f, 0.85f,
        0.9f, 0.95f, 0.99f, 0.999f, 1.0f
    };
};

TEST_P(EasingTest, Boundaries) {
    auto& param = GetParam();
    EXPECT_FLOAT_EQ(param.func(0.0f), 0.0f) << param.name << " at t=0";
    EXPECT_FLOAT_EQ(param.func(1.0f), 1.0f) << param.name << " at t=1";
}

TEST_P(EasingTest, Range) {
    auto& param = GetParam();
    for (float t : kInputs) {
        float v = param.func(t);
        EXPECT_GE(v, param.canOvershoot ? -1.5f : 0.0f) << param.name << " at t=" << t;
        EXPECT_LE(v, param.canOvershoot ? 1.5f : 1.0f) << param.name << " at t=" << t;
    }
}

TEST_P(EasingTest, MonotonicNonDecreasing) {
    auto& param = GetParam();
    if (param.canOvershoot) return;
    float prev = 0.0f;
    for (float t : kInputs) {
        float v = param.func(t);
        EXPECT_GE(v + 1e-6f, prev) << param.name << " not monotonic at t=" << t;
        prev = v;
    }
}

TEST_P(EasingTest, MidpointConsistency) {
    auto& param = GetParam();
    float half = param.func(0.5f);
    EXPECT_GT(half, 0.0f);
    if (!param.canOvershoot) {
        EXPECT_LT(half, 1.0f);
    }
}

static const EasingFunc kEasingFuncs[] = {
    {"Linear",     Easing::Linear},
    {"InQuad",     Easing::InQuad},
    {"OutQuad",    Easing::OutQuad},
    {"InOutQuad",  Easing::InOutQuad},
    {"InCubic",    Easing::InCubic},
    {"OutCubic",   Easing::OutCubic},
    {"InOutCubic", Easing::InOutCubic},
    {"InExpo",     Easing::InExpo},
    {"OutExpo",    Easing::OutExpo},
    {"OutElastic", Easing::OutElastic, true},
    {"OutBounce",  Easing::OutBounce, true},
};

INSTANTIATE_TEST_SUITE_P(AllEasingFunctions, EasingTest,
    ::testing::ValuesIn(kEasingFuncs),
    [](const auto& info) { return std::string(info.param.name); }
);

TEST(EasingTest, OutElasticOscillates) {
    float v1 = Easing::OutElastic(0.3f);
    float v2 = Easing::OutElastic(0.6f);
    EXPECT_GT(v2, v1);
    float v_end = Easing::OutElastic(1.0f);
    EXPECT_FLOAT_EQ(v_end, 1.0f);
}

TEST(EasingTest, OutBounceSteps) {
    float t1 = Easing::OutBounce(1.0f / 2.75f);
    EXPECT_FLOAT_EQ(t1, 7.5625f * (1.0f / 2.75f) * (1.0f / 2.75f));
    float t_end = Easing::OutBounce(1.0f);
    EXPECT_FLOAT_EQ(t_end, 1.0f);
}

TEST(EasingTest, LinearProportional) {
    EXPECT_FLOAT_EQ(Easing::Linear(0.25f), 0.25f);
    EXPECT_FLOAT_EQ(Easing::Linear(0.5f), 0.5f);
    EXPECT_FLOAT_EQ(Easing::Linear(0.75f), 0.75f);
}

TEST(EasingTest, InQuadAccelerates) {
    float v1 = Easing::InQuad(0.25f);
    float v2 = Easing::InQuad(0.5f);
    float v3 = Easing::InQuad(1.0f);
    EXPECT_LT(v1, 0.25f);
    EXPECT_FLOAT_EQ(v2, 0.25f);
    EXPECT_FLOAT_EQ(v3, 1.0f);
}

TEST(EasingTest, OutQuadDecelerates) {
    float v1 = Easing::OutQuad(0.75f);
    float mid = Easing::OutQuad(0.5f);
    EXPECT_FLOAT_EQ(mid, 0.75f);
    EXPECT_GT(v1, 0.5f);
    EXPECT_LT(v1, 1.0f);
    EXPECT_FLOAT_EQ(Easing::OutQuad(1.0f), 1.0f);
    EXPECT_FLOAT_EQ(Easing::OutQuad(0.0f), 0.0f);
}

TEST(EasingTest, InOutQuadSymmetry) {
    float half = Easing::InOutQuad(0.5f);
    EXPECT_FLOAT_EQ(half, 0.5f);
    float q1 = Easing::InOutQuad(0.25f);
    float q3 = Easing::InOutQuad(0.75f);
    EXPECT_NEAR(q1 + q3, 1.0f, 0.001f);
}

TEST(EasingTest, InOutCubicSymmetry) {
    float q1 = Easing::InOutCubic(0.25f);
    float q3 = Easing::InOutCubic(0.75f);
    EXPECT_NEAR(q1 + q3, 1.0f, 0.001f);
    EXPECT_FLOAT_EQ(Easing::InOutCubic(0.5f), 0.5f);
}
