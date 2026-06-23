#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SIMD_Math.hpp"

using namespace starlight;

TEST(SIMDMathTest, Mat4MulIdentity) {
    float a[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    float b[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    float res[16];
    simd::Mat4Mul(res, a, b);

    for (int i = 0; i < 16; i++) {
        EXPECT_FLOAT_EQ(res[i], a[i]) << "at index " << i;
    }
}

TEST(SIMDMathTest, Mat4MulMatchesGlm) {
    glm::mat4 glm_a = glm::translate(glm::mat4(1.0f), glm::vec3(10, 20, 30));
    glm::mat4 glm_b = glm::rotate(glm::mat4(1.0f), 0.5f, glm::vec3(0, 1, 0));
    glm::mat4 glm_expected = glm_a * glm_b;

    float a[16], b[16], res[16];
    memcpy(a, glm::value_ptr(glm_a), 16 * sizeof(float));
    memcpy(b, glm::value_ptr(glm_b), 16 * sizeof(float));
    simd::Mat4Mul(res, a, b);

    for (int i = 0; i < 16; i++) {
        EXPECT_NEAR(res[i], glm::value_ptr(glm_expected)[i], 0.001f) << "at index " << i;
    }
}

TEST(SIMDMathTest, Mat4MulScale) {
    glm::mat4 glm_s = glm::scale(glm::mat4(1.0f), glm::vec3(2, 3, 4));
    glm::mat4 glm_t = glm::translate(glm::mat4(1.0f), glm::vec3(5, 6, 7));
    glm::mat4 glm_expected = glm_t * glm_s;

    float s[16], t[16], res[16];
    memcpy(s, glm::value_ptr(glm_s), 16 * sizeof(float));
    memcpy(t, glm::value_ptr(glm_t), 16 * sizeof(float));
    simd::Mat4Mul(res, t, s);

    for (int i = 0; i < 16; i++) {
        EXPECT_NEAR(res[i], glm::value_ptr(glm_expected)[i], 0.001f) << "at index " << i;
    }
}

TEST(SIMDMathTest, Mat4MulComposite) {
    glm::mat4 glm_m = glm::mat4(1.0f);
    glm_m = glm::translate(glm_m, glm::vec3(1, 2, 3));
    glm_m = glm::rotate(glm_m, 1.2f, glm::vec3(0, 1, 0));
    glm_m = glm::scale(glm_m, glm::vec3(2, 2, 2));

    glm::mat4 glm_n = glm::translate(glm::mat4(1.0f), glm::vec3(10, 20, 30));
    glm::mat4 glm_expected = glm_m * glm_n;

    float m[16], n[16], res[16];
    memcpy(m, glm::value_ptr(glm_m), 16 * sizeof(float));
    memcpy(n, glm::value_ptr(glm_n), 16 * sizeof(float));
    simd::Mat4Mul(res, m, n);

    for (int i = 0; i < 16; i++) {
        EXPECT_NEAR(res[i], glm::value_ptr(glm_expected)[i], 0.001f) << "at index " << i;
    }
}

TEST(SIMDMathTest, Mat4MulLargeValues) {
    glm::mat4 glm_a = glm::translate(glm::mat4(1.0f), glm::vec3(1000, 2000, 3000));
    glm::mat4 glm_b = glm::translate(glm::mat4(1.0f), glm::vec3(4000, 5000, 6000));
    glm::mat4 glm_expected = glm_a * glm_b;

    float a[16], b[16], res[16];
    memcpy(a, glm::value_ptr(glm_a), 16 * sizeof(float));
    memcpy(b, glm::value_ptr(glm_b), 16 * sizeof(float));
    simd::Mat4Mul(res, a, b);

    for (int i = 0; i < 16; i++) {
        EXPECT_NEAR(res[i], glm::value_ptr(glm_expected)[i], 0.001f) << "at index " << i;
    }
}

TEST(SIMDMathTest, Mat4MulProjection) {
    float fov = 3.14159f / 4.0f;
    float aspect = 16.0f / 9.0f;
    float nearP = 0.1f, farP = 100.0f;
    glm::mat4 glm_p = glm::perspective(fov, aspect, nearP, farP);
    glm::mat4 glm_v = glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 glm_expected = glm_p * glm_v;

    float p[16], v[16], res[16];
    memcpy(p, glm::value_ptr(glm_p), 16 * sizeof(float));
    memcpy(v, glm::value_ptr(glm_v), 16 * sizeof(float));
    simd::Mat4Mul(res, p, v);

    for (int i = 0; i < 16; i++) {
        EXPECT_NEAR(res[i], glm::value_ptr(glm_expected)[i], 0.001f) << "at index " << i;
    }
}
