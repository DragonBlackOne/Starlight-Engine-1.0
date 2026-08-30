#include <gtest/gtest.h>
#include "SIMD_Math.hpp"
#include "SIMDMath.hpp"
#include "Memory.hpp"
#include "Tween.hpp"
#include "InputSystem.hpp"
#include "NavigationSystem.hpp"
#include "EventSystem.hpp"

using namespace starlight;

// ============================================================================
// STRESS AND FUZZ TEST SUITE - 1000 TESTS
// Generated automatically to find edge cases, bugs, and overflows.
// ============================================================================

// --- 1. MATH STRESS TESTS (200 tests) ---
TEST(MathStressTest, VectorCommon_0) {
    glm::vec3 v1(0.0000f, 0.0000f, 0.0000f);
    glm::vec3 v2(0.0000f, 1.0000f, 0.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 0.0000f, 0.001f);
    EXPECT_NEAR(added.y, 1.0000f, 0.001f);
    EXPECT_NEAR(added.z, 0.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (0.0000f * 0.0000f) + (0.0000f * 1.0000f) + (0.0000f * 0.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_1) {
    glm::vec3 v1(1.0000f, 2.0000f, 3.0000f);
    glm::vec3 v2(0.5000f, 2.0000f, 1.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 1.5000f, 0.001f);
    EXPECT_NEAR(added.y, 4.0000f, 0.001f);
    EXPECT_NEAR(added.z, 4.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (1.0000f * 0.5000f) + (2.0000f * 2.0000f) + (3.0000f * 1.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_2) {
    glm::vec3 v1(2.0000f, 4.0000f, 6.0000f);
    glm::vec3 v2(1.0000f, 3.0000f, 3.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 3.0000f, 0.001f);
    EXPECT_NEAR(added.y, 7.0000f, 0.001f);
    EXPECT_NEAR(added.z, 9.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (2.0000f * 1.0000f) + (4.0000f * 3.0000f) + (6.0000f * 3.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_3) {
    glm::vec3 v1(3.0000f, 6.0000f, 9.0000f);
    glm::vec3 v2(1.5000f, 4.0000f, 4.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 4.5000f, 0.001f);
    EXPECT_NEAR(added.y, 10.0000f, 0.001f);
    EXPECT_NEAR(added.z, 13.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (3.0000f * 1.5000f) + (6.0000f * 4.0000f) + (9.0000f * 4.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_4) {
    glm::vec3 v1(4.0000f, 8.0000f, 12.0000f);
    glm::vec3 v2(2.0000f, 5.0000f, 6.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 6.0000f, 0.001f);
    EXPECT_NEAR(added.y, 13.0000f, 0.001f);
    EXPECT_NEAR(added.z, 18.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (4.0000f * 2.0000f) + (8.0000f * 5.0000f) + (12.0000f * 6.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_5) {
    glm::vec3 v1(5.0000f, 10.0000f, 15.0000f);
    glm::vec3 v2(2.5000f, 6.0000f, 7.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 7.5000f, 0.001f);
    EXPECT_NEAR(added.y, 16.0000f, 0.001f);
    EXPECT_NEAR(added.z, 22.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (5.0000f * 2.5000f) + (10.0000f * 6.0000f) + (15.0000f * 7.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_6) {
    glm::vec3 v1(6.0000f, 12.0000f, 18.0000f);
    glm::vec3 v2(3.0000f, 7.0000f, 9.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 9.0000f, 0.001f);
    EXPECT_NEAR(added.y, 19.0000f, 0.001f);
    EXPECT_NEAR(added.z, 27.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (6.0000f * 3.0000f) + (12.0000f * 7.0000f) + (18.0000f * 9.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_7) {
    glm::vec3 v1(7.0000f, 14.0000f, 21.0000f);
    glm::vec3 v2(3.5000f, 8.0000f, 10.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 10.5000f, 0.001f);
    EXPECT_NEAR(added.y, 22.0000f, 0.001f);
    EXPECT_NEAR(added.z, 31.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (7.0000f * 3.5000f) + (14.0000f * 8.0000f) + (21.0000f * 10.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_8) {
    glm::vec3 v1(8.0000f, 16.0000f, 24.0000f);
    glm::vec3 v2(4.0000f, 9.0000f, 12.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 12.0000f, 0.001f);
    EXPECT_NEAR(added.y, 25.0000f, 0.001f);
    EXPECT_NEAR(added.z, 36.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (8.0000f * 4.0000f) + (16.0000f * 9.0000f) + (24.0000f * 12.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_9) {
    glm::vec3 v1(9.0000f, 18.0000f, 27.0000f);
    glm::vec3 v2(4.5000f, 10.0000f, 13.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 13.5000f, 0.001f);
    EXPECT_NEAR(added.y, 28.0000f, 0.001f);
    EXPECT_NEAR(added.z, 40.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (9.0000f * 4.5000f) + (18.0000f * 10.0000f) + (27.0000f * 13.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_10) {
    glm::vec3 v1(10.0000f, 20.0000f, 30.0000f);
    glm::vec3 v2(5.0000f, 11.0000f, 15.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 15.0000f, 0.001f);
    EXPECT_NEAR(added.y, 31.0000f, 0.001f);
    EXPECT_NEAR(added.z, 45.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (10.0000f * 5.0000f) + (20.0000f * 11.0000f) + (30.0000f * 15.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_11) {
    glm::vec3 v1(11.0000f, 22.0000f, 33.0000f);
    glm::vec3 v2(5.5000f, 12.0000f, 16.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 16.5000f, 0.001f);
    EXPECT_NEAR(added.y, 34.0000f, 0.001f);
    EXPECT_NEAR(added.z, 49.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (11.0000f * 5.5000f) + (22.0000f * 12.0000f) + (33.0000f * 16.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_12) {
    glm::vec3 v1(12.0000f, 24.0000f, 36.0000f);
    glm::vec3 v2(6.0000f, 13.0000f, 18.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 18.0000f, 0.001f);
    EXPECT_NEAR(added.y, 37.0000f, 0.001f);
    EXPECT_NEAR(added.z, 54.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (12.0000f * 6.0000f) + (24.0000f * 13.0000f) + (36.0000f * 18.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_13) {
    glm::vec3 v1(13.0000f, 26.0000f, 39.0000f);
    glm::vec3 v2(6.5000f, 14.0000f, 19.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 19.5000f, 0.001f);
    EXPECT_NEAR(added.y, 40.0000f, 0.001f);
    EXPECT_NEAR(added.z, 58.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (13.0000f * 6.5000f) + (26.0000f * 14.0000f) + (39.0000f * 19.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_14) {
    glm::vec3 v1(14.0000f, 28.0000f, 42.0000f);
    glm::vec3 v2(7.0000f, 15.0000f, 21.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 21.0000f, 0.001f);
    EXPECT_NEAR(added.y, 43.0000f, 0.001f);
    EXPECT_NEAR(added.z, 63.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (14.0000f * 7.0000f) + (28.0000f * 15.0000f) + (42.0000f * 21.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_15) {
    glm::vec3 v1(15.0000f, 30.0000f, 45.0000f);
    glm::vec3 v2(7.5000f, 16.0000f, 22.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 22.5000f, 0.001f);
    EXPECT_NEAR(added.y, 46.0000f, 0.001f);
    EXPECT_NEAR(added.z, 67.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (15.0000f * 7.5000f) + (30.0000f * 16.0000f) + (45.0000f * 22.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_16) {
    glm::vec3 v1(16.0000f, 32.0000f, 48.0000f);
    glm::vec3 v2(8.0000f, 17.0000f, 24.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 24.0000f, 0.001f);
    EXPECT_NEAR(added.y, 49.0000f, 0.001f);
    EXPECT_NEAR(added.z, 72.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (16.0000f * 8.0000f) + (32.0000f * 17.0000f) + (48.0000f * 24.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_17) {
    glm::vec3 v1(17.0000f, 34.0000f, 51.0000f);
    glm::vec3 v2(8.5000f, 18.0000f, 25.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 25.5000f, 0.001f);
    EXPECT_NEAR(added.y, 52.0000f, 0.001f);
    EXPECT_NEAR(added.z, 76.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (17.0000f * 8.5000f) + (34.0000f * 18.0000f) + (51.0000f * 25.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_18) {
    glm::vec3 v1(18.0000f, 36.0000f, 54.0000f);
    glm::vec3 v2(9.0000f, 19.0000f, 27.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 27.0000f, 0.001f);
    EXPECT_NEAR(added.y, 55.0000f, 0.001f);
    EXPECT_NEAR(added.z, 81.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (18.0000f * 9.0000f) + (36.0000f * 19.0000f) + (54.0000f * 27.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_19) {
    glm::vec3 v1(19.0000f, 38.0000f, 57.0000f);
    glm::vec3 v2(9.5000f, 20.0000f, 28.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 28.5000f, 0.001f);
    EXPECT_NEAR(added.y, 58.0000f, 0.001f);
    EXPECT_NEAR(added.z, 85.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (19.0000f * 9.5000f) + (38.0000f * 20.0000f) + (57.0000f * 28.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_20) {
    glm::vec3 v1(20.0000f, 40.0000f, 60.0000f);
    glm::vec3 v2(10.0000f, 21.0000f, 30.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 30.0000f, 0.001f);
    EXPECT_NEAR(added.y, 61.0000f, 0.001f);
    EXPECT_NEAR(added.z, 90.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (20.0000f * 10.0000f) + (40.0000f * 21.0000f) + (60.0000f * 30.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_21) {
    glm::vec3 v1(21.0000f, 42.0000f, 63.0000f);
    glm::vec3 v2(10.5000f, 22.0000f, 31.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 31.5000f, 0.001f);
    EXPECT_NEAR(added.y, 64.0000f, 0.001f);
    EXPECT_NEAR(added.z, 94.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (21.0000f * 10.5000f) + (42.0000f * 22.0000f) + (63.0000f * 31.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_22) {
    glm::vec3 v1(22.0000f, 44.0000f, 66.0000f);
    glm::vec3 v2(11.0000f, 23.0000f, 33.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 33.0000f, 0.001f);
    EXPECT_NEAR(added.y, 67.0000f, 0.001f);
    EXPECT_NEAR(added.z, 99.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (22.0000f * 11.0000f) + (44.0000f * 23.0000f) + (66.0000f * 33.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_23) {
    glm::vec3 v1(23.0000f, 46.0000f, 69.0000f);
    glm::vec3 v2(11.5000f, 24.0000f, 34.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 34.5000f, 0.001f);
    EXPECT_NEAR(added.y, 70.0000f, 0.001f);
    EXPECT_NEAR(added.z, 103.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (23.0000f * 11.5000f) + (46.0000f * 24.0000f) + (69.0000f * 34.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_24) {
    glm::vec3 v1(24.0000f, 48.0000f, 72.0000f);
    glm::vec3 v2(12.0000f, 25.0000f, 36.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 36.0000f, 0.001f);
    EXPECT_NEAR(added.y, 73.0000f, 0.001f);
    EXPECT_NEAR(added.z, 108.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (24.0000f * 12.0000f) + (48.0000f * 25.0000f) + (72.0000f * 36.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_25) {
    glm::vec3 v1(25.0000f, 50.0000f, 75.0000f);
    glm::vec3 v2(12.5000f, 26.0000f, 37.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 37.5000f, 0.001f);
    EXPECT_NEAR(added.y, 76.0000f, 0.001f);
    EXPECT_NEAR(added.z, 112.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (25.0000f * 12.5000f) + (50.0000f * 26.0000f) + (75.0000f * 37.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_26) {
    glm::vec3 v1(26.0000f, 52.0000f, 78.0000f);
    glm::vec3 v2(13.0000f, 27.0000f, 39.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 39.0000f, 0.001f);
    EXPECT_NEAR(added.y, 79.0000f, 0.001f);
    EXPECT_NEAR(added.z, 117.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (26.0000f * 13.0000f) + (52.0000f * 27.0000f) + (78.0000f * 39.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_27) {
    glm::vec3 v1(27.0000f, 54.0000f, 81.0000f);
    glm::vec3 v2(13.5000f, 28.0000f, 40.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 40.5000f, 0.001f);
    EXPECT_NEAR(added.y, 82.0000f, 0.001f);
    EXPECT_NEAR(added.z, 121.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (27.0000f * 13.5000f) + (54.0000f * 28.0000f) + (81.0000f * 40.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_28) {
    glm::vec3 v1(28.0000f, 56.0000f, 84.0000f);
    glm::vec3 v2(14.0000f, 29.0000f, 42.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 42.0000f, 0.001f);
    EXPECT_NEAR(added.y, 85.0000f, 0.001f);
    EXPECT_NEAR(added.z, 126.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (28.0000f * 14.0000f) + (56.0000f * 29.0000f) + (84.0000f * 42.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_29) {
    glm::vec3 v1(29.0000f, 58.0000f, 87.0000f);
    glm::vec3 v2(14.5000f, 30.0000f, 43.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 43.5000f, 0.001f);
    EXPECT_NEAR(added.y, 88.0000f, 0.001f);
    EXPECT_NEAR(added.z, 130.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (29.0000f * 14.5000f) + (58.0000f * 30.0000f) + (87.0000f * 43.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_30) {
    glm::vec3 v1(30.0000f, 60.0000f, 90.0000f);
    glm::vec3 v2(15.0000f, 31.0000f, 45.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 45.0000f, 0.001f);
    EXPECT_NEAR(added.y, 91.0000f, 0.001f);
    EXPECT_NEAR(added.z, 135.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (30.0000f * 15.0000f) + (60.0000f * 31.0000f) + (90.0000f * 45.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_31) {
    glm::vec3 v1(31.0000f, 62.0000f, 93.0000f);
    glm::vec3 v2(15.5000f, 32.0000f, 46.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 46.5000f, 0.001f);
    EXPECT_NEAR(added.y, 94.0000f, 0.001f);
    EXPECT_NEAR(added.z, 139.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (31.0000f * 15.5000f) + (62.0000f * 32.0000f) + (93.0000f * 46.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_32) {
    glm::vec3 v1(32.0000f, 64.0000f, 96.0000f);
    glm::vec3 v2(16.0000f, 33.0000f, 48.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 48.0000f, 0.001f);
    EXPECT_NEAR(added.y, 97.0000f, 0.001f);
    EXPECT_NEAR(added.z, 144.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (32.0000f * 16.0000f) + (64.0000f * 33.0000f) + (96.0000f * 48.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_33) {
    glm::vec3 v1(33.0000f, 66.0000f, 99.0000f);
    glm::vec3 v2(16.5000f, 34.0000f, 49.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 49.5000f, 0.001f);
    EXPECT_NEAR(added.y, 100.0000f, 0.001f);
    EXPECT_NEAR(added.z, 148.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (33.0000f * 16.5000f) + (66.0000f * 34.0000f) + (99.0000f * 49.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_34) {
    glm::vec3 v1(34.0000f, 68.0000f, 102.0000f);
    glm::vec3 v2(17.0000f, 35.0000f, 51.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 51.0000f, 0.001f);
    EXPECT_NEAR(added.y, 103.0000f, 0.001f);
    EXPECT_NEAR(added.z, 153.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (34.0000f * 17.0000f) + (68.0000f * 35.0000f) + (102.0000f * 51.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_35) {
    glm::vec3 v1(35.0000f, 70.0000f, 105.0000f);
    glm::vec3 v2(17.5000f, 36.0000f, 52.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 52.5000f, 0.001f);
    EXPECT_NEAR(added.y, 106.0000f, 0.001f);
    EXPECT_NEAR(added.z, 157.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (35.0000f * 17.5000f) + (70.0000f * 36.0000f) + (105.0000f * 52.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_36) {
    glm::vec3 v1(36.0000f, 72.0000f, 108.0000f);
    glm::vec3 v2(18.0000f, 37.0000f, 54.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 54.0000f, 0.001f);
    EXPECT_NEAR(added.y, 109.0000f, 0.001f);
    EXPECT_NEAR(added.z, 162.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (36.0000f * 18.0000f) + (72.0000f * 37.0000f) + (108.0000f * 54.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_37) {
    glm::vec3 v1(37.0000f, 74.0000f, 111.0000f);
    glm::vec3 v2(18.5000f, 38.0000f, 55.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 55.5000f, 0.001f);
    EXPECT_NEAR(added.y, 112.0000f, 0.001f);
    EXPECT_NEAR(added.z, 166.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (37.0000f * 18.5000f) + (74.0000f * 38.0000f) + (111.0000f * 55.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_38) {
    glm::vec3 v1(38.0000f, 76.0000f, 114.0000f);
    glm::vec3 v2(19.0000f, 39.0000f, 57.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 57.0000f, 0.001f);
    EXPECT_NEAR(added.y, 115.0000f, 0.001f);
    EXPECT_NEAR(added.z, 171.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (38.0000f * 19.0000f) + (76.0000f * 39.0000f) + (114.0000f * 57.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_39) {
    glm::vec3 v1(39.0000f, 78.0000f, 117.0000f);
    glm::vec3 v2(19.5000f, 40.0000f, 58.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 58.5000f, 0.001f);
    EXPECT_NEAR(added.y, 118.0000f, 0.001f);
    EXPECT_NEAR(added.z, 175.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (39.0000f * 19.5000f) + (78.0000f * 40.0000f) + (117.0000f * 58.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_40) {
    glm::vec3 v1(40.0000f, 80.0000f, 120.0000f);
    glm::vec3 v2(20.0000f, 41.0000f, 60.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 60.0000f, 0.001f);
    EXPECT_NEAR(added.y, 121.0000f, 0.001f);
    EXPECT_NEAR(added.z, 180.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (40.0000f * 20.0000f) + (80.0000f * 41.0000f) + (120.0000f * 60.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_41) {
    glm::vec3 v1(41.0000f, 82.0000f, 123.0000f);
    glm::vec3 v2(20.5000f, 42.0000f, 61.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 61.5000f, 0.001f);
    EXPECT_NEAR(added.y, 124.0000f, 0.001f);
    EXPECT_NEAR(added.z, 184.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (41.0000f * 20.5000f) + (82.0000f * 42.0000f) + (123.0000f * 61.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_42) {
    glm::vec3 v1(42.0000f, 84.0000f, 126.0000f);
    glm::vec3 v2(21.0000f, 43.0000f, 63.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 63.0000f, 0.001f);
    EXPECT_NEAR(added.y, 127.0000f, 0.001f);
    EXPECT_NEAR(added.z, 189.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (42.0000f * 21.0000f) + (84.0000f * 43.0000f) + (126.0000f * 63.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_43) {
    glm::vec3 v1(43.0000f, 86.0000f, 129.0000f);
    glm::vec3 v2(21.5000f, 44.0000f, 64.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 64.5000f, 0.001f);
    EXPECT_NEAR(added.y, 130.0000f, 0.001f);
    EXPECT_NEAR(added.z, 193.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (43.0000f * 21.5000f) + (86.0000f * 44.0000f) + (129.0000f * 64.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_44) {
    glm::vec3 v1(44.0000f, 88.0000f, 132.0000f);
    glm::vec3 v2(22.0000f, 45.0000f, 66.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 66.0000f, 0.001f);
    EXPECT_NEAR(added.y, 133.0000f, 0.001f);
    EXPECT_NEAR(added.z, 198.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (44.0000f * 22.0000f) + (88.0000f * 45.0000f) + (132.0000f * 66.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_45) {
    glm::vec3 v1(45.0000f, 90.0000f, 135.0000f);
    glm::vec3 v2(22.5000f, 46.0000f, 67.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 67.5000f, 0.001f);
    EXPECT_NEAR(added.y, 136.0000f, 0.001f);
    EXPECT_NEAR(added.z, 202.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (45.0000f * 22.5000f) + (90.0000f * 46.0000f) + (135.0000f * 67.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_46) {
    glm::vec3 v1(46.0000f, 92.0000f, 138.0000f);
    glm::vec3 v2(23.0000f, 47.0000f, 69.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 69.0000f, 0.001f);
    EXPECT_NEAR(added.y, 139.0000f, 0.001f);
    EXPECT_NEAR(added.z, 207.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (46.0000f * 23.0000f) + (92.0000f * 47.0000f) + (138.0000f * 69.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_47) {
    glm::vec3 v1(47.0000f, 94.0000f, 141.0000f);
    glm::vec3 v2(23.5000f, 48.0000f, 70.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 70.5000f, 0.001f);
    EXPECT_NEAR(added.y, 142.0000f, 0.001f);
    EXPECT_NEAR(added.z, 211.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (47.0000f * 23.5000f) + (94.0000f * 48.0000f) + (141.0000f * 70.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_48) {
    glm::vec3 v1(48.0000f, 96.0000f, 144.0000f);
    glm::vec3 v2(24.0000f, 49.0000f, 72.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 72.0000f, 0.001f);
    EXPECT_NEAR(added.y, 145.0000f, 0.001f);
    EXPECT_NEAR(added.z, 216.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (48.0000f * 24.0000f) + (96.0000f * 49.0000f) + (144.0000f * 72.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_49) {
    glm::vec3 v1(49.0000f, 98.0000f, 147.0000f);
    glm::vec3 v2(24.5000f, 50.0000f, 73.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 73.5000f, 0.001f);
    EXPECT_NEAR(added.y, 148.0000f, 0.001f);
    EXPECT_NEAR(added.z, 220.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (49.0000f * 24.5000f) + (98.0000f * 50.0000f) + (147.0000f * 73.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_50) {
    glm::vec3 v1(50.0000f, 100.0000f, 150.0000f);
    glm::vec3 v2(25.0000f, 51.0000f, 75.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 75.0000f, 0.001f);
    EXPECT_NEAR(added.y, 151.0000f, 0.001f);
    EXPECT_NEAR(added.z, 225.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (50.0000f * 25.0000f) + (100.0000f * 51.0000f) + (150.0000f * 75.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_51) {
    glm::vec3 v1(51.0000f, 102.0000f, 153.0000f);
    glm::vec3 v2(25.5000f, 52.0000f, 76.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 76.5000f, 0.001f);
    EXPECT_NEAR(added.y, 154.0000f, 0.001f);
    EXPECT_NEAR(added.z, 229.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (51.0000f * 25.5000f) + (102.0000f * 52.0000f) + (153.0000f * 76.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_52) {
    glm::vec3 v1(52.0000f, 104.0000f, 156.0000f);
    glm::vec3 v2(26.0000f, 53.0000f, 78.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 78.0000f, 0.001f);
    EXPECT_NEAR(added.y, 157.0000f, 0.001f);
    EXPECT_NEAR(added.z, 234.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (52.0000f * 26.0000f) + (104.0000f * 53.0000f) + (156.0000f * 78.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_53) {
    glm::vec3 v1(53.0000f, 106.0000f, 159.0000f);
    glm::vec3 v2(26.5000f, 54.0000f, 79.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 79.5000f, 0.001f);
    EXPECT_NEAR(added.y, 160.0000f, 0.001f);
    EXPECT_NEAR(added.z, 238.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (53.0000f * 26.5000f) + (106.0000f * 54.0000f) + (159.0000f * 79.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_54) {
    glm::vec3 v1(54.0000f, 108.0000f, 162.0000f);
    glm::vec3 v2(27.0000f, 55.0000f, 81.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 81.0000f, 0.001f);
    EXPECT_NEAR(added.y, 163.0000f, 0.001f);
    EXPECT_NEAR(added.z, 243.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (54.0000f * 27.0000f) + (108.0000f * 55.0000f) + (162.0000f * 81.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_55) {
    glm::vec3 v1(55.0000f, 110.0000f, 165.0000f);
    glm::vec3 v2(27.5000f, 56.0000f, 82.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 82.5000f, 0.001f);
    EXPECT_NEAR(added.y, 166.0000f, 0.001f);
    EXPECT_NEAR(added.z, 247.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (55.0000f * 27.5000f) + (110.0000f * 56.0000f) + (165.0000f * 82.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_56) {
    glm::vec3 v1(56.0000f, 112.0000f, 168.0000f);
    glm::vec3 v2(28.0000f, 57.0000f, 84.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 84.0000f, 0.001f);
    EXPECT_NEAR(added.y, 169.0000f, 0.001f);
    EXPECT_NEAR(added.z, 252.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (56.0000f * 28.0000f) + (112.0000f * 57.0000f) + (168.0000f * 84.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_57) {
    glm::vec3 v1(57.0000f, 114.0000f, 171.0000f);
    glm::vec3 v2(28.5000f, 58.0000f, 85.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 85.5000f, 0.001f);
    EXPECT_NEAR(added.y, 172.0000f, 0.001f);
    EXPECT_NEAR(added.z, 256.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (57.0000f * 28.5000f) + (114.0000f * 58.0000f) + (171.0000f * 85.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_58) {
    glm::vec3 v1(58.0000f, 116.0000f, 174.0000f);
    glm::vec3 v2(29.0000f, 59.0000f, 87.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 87.0000f, 0.001f);
    EXPECT_NEAR(added.y, 175.0000f, 0.001f);
    EXPECT_NEAR(added.z, 261.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (58.0000f * 29.0000f) + (116.0000f * 59.0000f) + (174.0000f * 87.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_59) {
    glm::vec3 v1(59.0000f, 118.0000f, 177.0000f);
    glm::vec3 v2(29.5000f, 60.0000f, 88.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 88.5000f, 0.001f);
    EXPECT_NEAR(added.y, 178.0000f, 0.001f);
    EXPECT_NEAR(added.z, 265.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (59.0000f * 29.5000f) + (118.0000f * 60.0000f) + (177.0000f * 88.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_60) {
    glm::vec3 v1(60.0000f, 120.0000f, 180.0000f);
    glm::vec3 v2(30.0000f, 61.0000f, 90.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 90.0000f, 0.001f);
    EXPECT_NEAR(added.y, 181.0000f, 0.001f);
    EXPECT_NEAR(added.z, 270.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (60.0000f * 30.0000f) + (120.0000f * 61.0000f) + (180.0000f * 90.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_61) {
    glm::vec3 v1(61.0000f, 122.0000f, 183.0000f);
    glm::vec3 v2(30.5000f, 62.0000f, 91.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 91.5000f, 0.001f);
    EXPECT_NEAR(added.y, 184.0000f, 0.001f);
    EXPECT_NEAR(added.z, 274.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (61.0000f * 30.5000f) + (122.0000f * 62.0000f) + (183.0000f * 91.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_62) {
    glm::vec3 v1(62.0000f, 124.0000f, 186.0000f);
    glm::vec3 v2(31.0000f, 63.0000f, 93.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 93.0000f, 0.001f);
    EXPECT_NEAR(added.y, 187.0000f, 0.001f);
    EXPECT_NEAR(added.z, 279.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (62.0000f * 31.0000f) + (124.0000f * 63.0000f) + (186.0000f * 93.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_63) {
    glm::vec3 v1(63.0000f, 126.0000f, 189.0000f);
    glm::vec3 v2(31.5000f, 64.0000f, 94.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 94.5000f, 0.001f);
    EXPECT_NEAR(added.y, 190.0000f, 0.001f);
    EXPECT_NEAR(added.z, 283.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (63.0000f * 31.5000f) + (126.0000f * 64.0000f) + (189.0000f * 94.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_64) {
    glm::vec3 v1(64.0000f, 128.0000f, 192.0000f);
    glm::vec3 v2(32.0000f, 65.0000f, 96.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 96.0000f, 0.001f);
    EXPECT_NEAR(added.y, 193.0000f, 0.001f);
    EXPECT_NEAR(added.z, 288.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (64.0000f * 32.0000f) + (128.0000f * 65.0000f) + (192.0000f * 96.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_65) {
    glm::vec3 v1(65.0000f, 130.0000f, 195.0000f);
    glm::vec3 v2(32.5000f, 66.0000f, 97.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 97.5000f, 0.001f);
    EXPECT_NEAR(added.y, 196.0000f, 0.001f);
    EXPECT_NEAR(added.z, 292.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (65.0000f * 32.5000f) + (130.0000f * 66.0000f) + (195.0000f * 97.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_66) {
    glm::vec3 v1(66.0000f, 132.0000f, 198.0000f);
    glm::vec3 v2(33.0000f, 67.0000f, 99.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 99.0000f, 0.001f);
    EXPECT_NEAR(added.y, 199.0000f, 0.001f);
    EXPECT_NEAR(added.z, 297.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (66.0000f * 33.0000f) + (132.0000f * 67.0000f) + (198.0000f * 99.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_67) {
    glm::vec3 v1(67.0000f, 134.0000f, 201.0000f);
    glm::vec3 v2(33.5000f, 68.0000f, 100.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 100.5000f, 0.001f);
    EXPECT_NEAR(added.y, 202.0000f, 0.001f);
    EXPECT_NEAR(added.z, 301.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (67.0000f * 33.5000f) + (134.0000f * 68.0000f) + (201.0000f * 100.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_68) {
    glm::vec3 v1(68.0000f, 136.0000f, 204.0000f);
    glm::vec3 v2(34.0000f, 69.0000f, 102.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 102.0000f, 0.001f);
    EXPECT_NEAR(added.y, 205.0000f, 0.001f);
    EXPECT_NEAR(added.z, 306.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (68.0000f * 34.0000f) + (136.0000f * 69.0000f) + (204.0000f * 102.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_69) {
    glm::vec3 v1(69.0000f, 138.0000f, 207.0000f);
    glm::vec3 v2(34.5000f, 70.0000f, 103.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 103.5000f, 0.001f);
    EXPECT_NEAR(added.y, 208.0000f, 0.001f);
    EXPECT_NEAR(added.z, 310.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (69.0000f * 34.5000f) + (138.0000f * 70.0000f) + (207.0000f * 103.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_70) {
    glm::vec3 v1(70.0000f, 140.0000f, 210.0000f);
    glm::vec3 v2(35.0000f, 71.0000f, 105.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 105.0000f, 0.001f);
    EXPECT_NEAR(added.y, 211.0000f, 0.001f);
    EXPECT_NEAR(added.z, 315.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (70.0000f * 35.0000f) + (140.0000f * 71.0000f) + (210.0000f * 105.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_71) {
    glm::vec3 v1(71.0000f, 142.0000f, 213.0000f);
    glm::vec3 v2(35.5000f, 72.0000f, 106.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 106.5000f, 0.001f);
    EXPECT_NEAR(added.y, 214.0000f, 0.001f);
    EXPECT_NEAR(added.z, 319.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (71.0000f * 35.5000f) + (142.0000f * 72.0000f) + (213.0000f * 106.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_72) {
    glm::vec3 v1(72.0000f, 144.0000f, 216.0000f);
    glm::vec3 v2(36.0000f, 73.0000f, 108.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 108.0000f, 0.001f);
    EXPECT_NEAR(added.y, 217.0000f, 0.001f);
    EXPECT_NEAR(added.z, 324.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (72.0000f * 36.0000f) + (144.0000f * 73.0000f) + (216.0000f * 108.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_73) {
    glm::vec3 v1(73.0000f, 146.0000f, 219.0000f);
    glm::vec3 v2(36.5000f, 74.0000f, 109.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 109.5000f, 0.001f);
    EXPECT_NEAR(added.y, 220.0000f, 0.001f);
    EXPECT_NEAR(added.z, 328.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (73.0000f * 36.5000f) + (146.0000f * 74.0000f) + (219.0000f * 109.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_74) {
    glm::vec3 v1(74.0000f, 148.0000f, 222.0000f);
    glm::vec3 v2(37.0000f, 75.0000f, 111.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 111.0000f, 0.001f);
    EXPECT_NEAR(added.y, 223.0000f, 0.001f);
    EXPECT_NEAR(added.z, 333.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (74.0000f * 37.0000f) + (148.0000f * 75.0000f) + (222.0000f * 111.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_75) {
    glm::vec3 v1(75.0000f, 150.0000f, 225.0000f);
    glm::vec3 v2(37.5000f, 76.0000f, 112.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 112.5000f, 0.001f);
    EXPECT_NEAR(added.y, 226.0000f, 0.001f);
    EXPECT_NEAR(added.z, 337.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (75.0000f * 37.5000f) + (150.0000f * 76.0000f) + (225.0000f * 112.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_76) {
    glm::vec3 v1(76.0000f, 152.0000f, 228.0000f);
    glm::vec3 v2(38.0000f, 77.0000f, 114.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 114.0000f, 0.001f);
    EXPECT_NEAR(added.y, 229.0000f, 0.001f);
    EXPECT_NEAR(added.z, 342.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (76.0000f * 38.0000f) + (152.0000f * 77.0000f) + (228.0000f * 114.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_77) {
    glm::vec3 v1(77.0000f, 154.0000f, 231.0000f);
    glm::vec3 v2(38.5000f, 78.0000f, 115.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 115.5000f, 0.001f);
    EXPECT_NEAR(added.y, 232.0000f, 0.001f);
    EXPECT_NEAR(added.z, 346.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (77.0000f * 38.5000f) + (154.0000f * 78.0000f) + (231.0000f * 115.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_78) {
    glm::vec3 v1(78.0000f, 156.0000f, 234.0000f);
    glm::vec3 v2(39.0000f, 79.0000f, 117.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 117.0000f, 0.001f);
    EXPECT_NEAR(added.y, 235.0000f, 0.001f);
    EXPECT_NEAR(added.z, 351.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (78.0000f * 39.0000f) + (156.0000f * 79.0000f) + (234.0000f * 117.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_79) {
    glm::vec3 v1(79.0000f, 158.0000f, 237.0000f);
    glm::vec3 v2(39.5000f, 80.0000f, 118.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 118.5000f, 0.001f);
    EXPECT_NEAR(added.y, 238.0000f, 0.001f);
    EXPECT_NEAR(added.z, 355.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (79.0000f * 39.5000f) + (158.0000f * 80.0000f) + (237.0000f * 118.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_80) {
    glm::vec3 v1(80.0000f, 160.0000f, 240.0000f);
    glm::vec3 v2(40.0000f, 81.0000f, 120.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 120.0000f, 0.001f);
    EXPECT_NEAR(added.y, 241.0000f, 0.001f);
    EXPECT_NEAR(added.z, 360.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (80.0000f * 40.0000f) + (160.0000f * 81.0000f) + (240.0000f * 120.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_81) {
    glm::vec3 v1(81.0000f, 162.0000f, 243.0000f);
    glm::vec3 v2(40.5000f, 82.0000f, 121.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 121.5000f, 0.001f);
    EXPECT_NEAR(added.y, 244.0000f, 0.001f);
    EXPECT_NEAR(added.z, 364.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (81.0000f * 40.5000f) + (162.0000f * 82.0000f) + (243.0000f * 121.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_82) {
    glm::vec3 v1(82.0000f, 164.0000f, 246.0000f);
    glm::vec3 v2(41.0000f, 83.0000f, 123.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 123.0000f, 0.001f);
    EXPECT_NEAR(added.y, 247.0000f, 0.001f);
    EXPECT_NEAR(added.z, 369.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (82.0000f * 41.0000f) + (164.0000f * 83.0000f) + (246.0000f * 123.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_83) {
    glm::vec3 v1(83.0000f, 166.0000f, 249.0000f);
    glm::vec3 v2(41.5000f, 84.0000f, 124.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 124.5000f, 0.001f);
    EXPECT_NEAR(added.y, 250.0000f, 0.001f);
    EXPECT_NEAR(added.z, 373.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (83.0000f * 41.5000f) + (166.0000f * 84.0000f) + (249.0000f * 124.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_84) {
    glm::vec3 v1(84.0000f, 168.0000f, 252.0000f);
    glm::vec3 v2(42.0000f, 85.0000f, 126.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 126.0000f, 0.001f);
    EXPECT_NEAR(added.y, 253.0000f, 0.001f);
    EXPECT_NEAR(added.z, 378.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (84.0000f * 42.0000f) + (168.0000f * 85.0000f) + (252.0000f * 126.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_85) {
    glm::vec3 v1(85.0000f, 170.0000f, 255.0000f);
    glm::vec3 v2(42.5000f, 86.0000f, 127.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 127.5000f, 0.001f);
    EXPECT_NEAR(added.y, 256.0000f, 0.001f);
    EXPECT_NEAR(added.z, 382.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (85.0000f * 42.5000f) + (170.0000f * 86.0000f) + (255.0000f * 127.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_86) {
    glm::vec3 v1(86.0000f, 172.0000f, 258.0000f);
    glm::vec3 v2(43.0000f, 87.0000f, 129.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 129.0000f, 0.001f);
    EXPECT_NEAR(added.y, 259.0000f, 0.001f);
    EXPECT_NEAR(added.z, 387.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (86.0000f * 43.0000f) + (172.0000f * 87.0000f) + (258.0000f * 129.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_87) {
    glm::vec3 v1(87.0000f, 174.0000f, 261.0000f);
    glm::vec3 v2(43.5000f, 88.0000f, 130.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 130.5000f, 0.001f);
    EXPECT_NEAR(added.y, 262.0000f, 0.001f);
    EXPECT_NEAR(added.z, 391.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (87.0000f * 43.5000f) + (174.0000f * 88.0000f) + (261.0000f * 130.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_88) {
    glm::vec3 v1(88.0000f, 176.0000f, 264.0000f);
    glm::vec3 v2(44.0000f, 89.0000f, 132.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 132.0000f, 0.001f);
    EXPECT_NEAR(added.y, 265.0000f, 0.001f);
    EXPECT_NEAR(added.z, 396.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (88.0000f * 44.0000f) + (176.0000f * 89.0000f) + (264.0000f * 132.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_89) {
    glm::vec3 v1(89.0000f, 178.0000f, 267.0000f);
    glm::vec3 v2(44.5000f, 90.0000f, 133.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 133.5000f, 0.001f);
    EXPECT_NEAR(added.y, 268.0000f, 0.001f);
    EXPECT_NEAR(added.z, 400.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (89.0000f * 44.5000f) + (178.0000f * 90.0000f) + (267.0000f * 133.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_90) {
    glm::vec3 v1(90.0000f, 180.0000f, 270.0000f);
    glm::vec3 v2(45.0000f, 91.0000f, 135.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 135.0000f, 0.001f);
    EXPECT_NEAR(added.y, 271.0000f, 0.001f);
    EXPECT_NEAR(added.z, 405.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (90.0000f * 45.0000f) + (180.0000f * 91.0000f) + (270.0000f * 135.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_91) {
    glm::vec3 v1(91.0000f, 182.0000f, 273.0000f);
    glm::vec3 v2(45.5000f, 92.0000f, 136.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 136.5000f, 0.001f);
    EXPECT_NEAR(added.y, 274.0000f, 0.001f);
    EXPECT_NEAR(added.z, 409.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (91.0000f * 45.5000f) + (182.0000f * 92.0000f) + (273.0000f * 136.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_92) {
    glm::vec3 v1(92.0000f, 184.0000f, 276.0000f);
    glm::vec3 v2(46.0000f, 93.0000f, 138.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 138.0000f, 0.001f);
    EXPECT_NEAR(added.y, 277.0000f, 0.001f);
    EXPECT_NEAR(added.z, 414.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (92.0000f * 46.0000f) + (184.0000f * 93.0000f) + (276.0000f * 138.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_93) {
    glm::vec3 v1(93.0000f, 186.0000f, 279.0000f);
    glm::vec3 v2(46.5000f, 94.0000f, 139.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 139.5000f, 0.001f);
    EXPECT_NEAR(added.y, 280.0000f, 0.001f);
    EXPECT_NEAR(added.z, 418.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (93.0000f * 46.5000f) + (186.0000f * 94.0000f) + (279.0000f * 139.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_94) {
    glm::vec3 v1(94.0000f, 188.0000f, 282.0000f);
    glm::vec3 v2(47.0000f, 95.0000f, 141.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 141.0000f, 0.001f);
    EXPECT_NEAR(added.y, 283.0000f, 0.001f);
    EXPECT_NEAR(added.z, 423.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (94.0000f * 47.0000f) + (188.0000f * 95.0000f) + (282.0000f * 141.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_95) {
    glm::vec3 v1(95.0000f, 190.0000f, 285.0000f);
    glm::vec3 v2(47.5000f, 96.0000f, 142.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 142.5000f, 0.001f);
    EXPECT_NEAR(added.y, 286.0000f, 0.001f);
    EXPECT_NEAR(added.z, 427.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (95.0000f * 47.5000f) + (190.0000f * 96.0000f) + (285.0000f * 142.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_96) {
    glm::vec3 v1(96.0000f, 192.0000f, 288.0000f);
    glm::vec3 v2(48.0000f, 97.0000f, 144.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 144.0000f, 0.001f);
    EXPECT_NEAR(added.y, 289.0000f, 0.001f);
    EXPECT_NEAR(added.z, 432.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (96.0000f * 48.0000f) + (192.0000f * 97.0000f) + (288.0000f * 144.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_97) {
    glm::vec3 v1(97.0000f, 194.0000f, 291.0000f);
    glm::vec3 v2(48.5000f, 98.0000f, 145.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 145.5000f, 0.001f);
    EXPECT_NEAR(added.y, 292.0000f, 0.001f);
    EXPECT_NEAR(added.z, 436.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (97.0000f * 48.5000f) + (194.0000f * 98.0000f) + (291.0000f * 145.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_98) {
    glm::vec3 v1(98.0000f, 196.0000f, 294.0000f);
    glm::vec3 v2(49.0000f, 99.0000f, 147.0000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 147.0000f, 0.001f);
    EXPECT_NEAR(added.y, 295.0000f, 0.001f);
    EXPECT_NEAR(added.z, 441.0000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (98.0000f * 49.0000f) + (196.0000f * 99.0000f) + (294.0000f * 147.0000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, VectorCommon_99) {
    glm::vec3 v1(99.0000f, 198.0000f, 297.0000f);
    glm::vec3 v2(49.5000f, 100.0000f, 148.5000f);
    
    glm::vec3 added = v1 + v2;
    EXPECT_NEAR(added.x, 148.5000f, 0.001f);
    EXPECT_NEAR(added.y, 298.0000f, 0.001f);
    EXPECT_NEAR(added.z, 445.5000f, 0.001f);
    
    float dotProduct = glm::dot(v1, v2);
    float expectedDot = (99.0000f * 49.5000f) + (198.0000f * 100.0000f) + (297.0000f * 148.5000f);
    EXPECT_NEAR(dotProduct, expectedDot, 0.01f);
}
TEST(MathStressTest, SIMD_Mat4Mul_0) {
    float a[16] = {
        0.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 0.0000f, 0.001f);
    EXPECT_NEAR(res[5], 2.0000f, 0.001f);
    EXPECT_NEAR(res[10], 4.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_1) {
    float a[16] = {
        1.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 3.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 2.0000f, 0.001f);
    EXPECT_NEAR(res[5], 4.0000f, 0.001f);
    EXPECT_NEAR(res[10], 6.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_2) {
    float a[16] = {
        2.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 3.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 4.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 4.0000f, 0.001f);
    EXPECT_NEAR(res[5], 6.0000f, 0.001f);
    EXPECT_NEAR(res[10], 8.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_3) {
    float a[16] = {
        3.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 4.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 5.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 6.0000f, 0.001f);
    EXPECT_NEAR(res[5], 8.0000f, 0.001f);
    EXPECT_NEAR(res[10], 10.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_4) {
    float a[16] = {
        4.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 5.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 6.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 8.0000f, 0.001f);
    EXPECT_NEAR(res[5], 10.0000f, 0.001f);
    EXPECT_NEAR(res[10], 12.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_5) {
    float a[16] = {
        5.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 6.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 7.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 10.0000f, 0.001f);
    EXPECT_NEAR(res[5], 12.0000f, 0.001f);
    EXPECT_NEAR(res[10], 14.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_6) {
    float a[16] = {
        6.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 7.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 8.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 12.0000f, 0.001f);
    EXPECT_NEAR(res[5], 14.0000f, 0.001f);
    EXPECT_NEAR(res[10], 16.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_7) {
    float a[16] = {
        7.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 8.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 9.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 14.0000f, 0.001f);
    EXPECT_NEAR(res[5], 16.0000f, 0.001f);
    EXPECT_NEAR(res[10], 18.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_8) {
    float a[16] = {
        8.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 9.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 10.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 16.0000f, 0.001f);
    EXPECT_NEAR(res[5], 18.0000f, 0.001f);
    EXPECT_NEAR(res[10], 20.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_9) {
    float a[16] = {
        9.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 10.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 11.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 18.0000f, 0.001f);
    EXPECT_NEAR(res[5], 20.0000f, 0.001f);
    EXPECT_NEAR(res[10], 22.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_10) {
    float a[16] = {
        10.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 11.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 12.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 20.0000f, 0.001f);
    EXPECT_NEAR(res[5], 22.0000f, 0.001f);
    EXPECT_NEAR(res[10], 24.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_11) {
    float a[16] = {
        11.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 12.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 13.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 22.0000f, 0.001f);
    EXPECT_NEAR(res[5], 24.0000f, 0.001f);
    EXPECT_NEAR(res[10], 26.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_12) {
    float a[16] = {
        12.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 13.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 14.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 24.0000f, 0.001f);
    EXPECT_NEAR(res[5], 26.0000f, 0.001f);
    EXPECT_NEAR(res[10], 28.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_13) {
    float a[16] = {
        13.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 14.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 15.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 26.0000f, 0.001f);
    EXPECT_NEAR(res[5], 28.0000f, 0.001f);
    EXPECT_NEAR(res[10], 30.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_14) {
    float a[16] = {
        14.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 15.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 16.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 28.0000f, 0.001f);
    EXPECT_NEAR(res[5], 30.0000f, 0.001f);
    EXPECT_NEAR(res[10], 32.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_15) {
    float a[16] = {
        15.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 16.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 17.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 30.0000f, 0.001f);
    EXPECT_NEAR(res[5], 32.0000f, 0.001f);
    EXPECT_NEAR(res[10], 34.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_16) {
    float a[16] = {
        16.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 17.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 18.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 32.0000f, 0.001f);
    EXPECT_NEAR(res[5], 34.0000f, 0.001f);
    EXPECT_NEAR(res[10], 36.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_17) {
    float a[16] = {
        17.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 18.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 19.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 34.0000f, 0.001f);
    EXPECT_NEAR(res[5], 36.0000f, 0.001f);
    EXPECT_NEAR(res[10], 38.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_18) {
    float a[16] = {
        18.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 19.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 20.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 36.0000f, 0.001f);
    EXPECT_NEAR(res[5], 38.0000f, 0.001f);
    EXPECT_NEAR(res[10], 40.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_19) {
    float a[16] = {
        19.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 20.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 21.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 38.0000f, 0.001f);
    EXPECT_NEAR(res[5], 40.0000f, 0.001f);
    EXPECT_NEAR(res[10], 42.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_20) {
    float a[16] = {
        20.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 21.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 22.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 40.0000f, 0.001f);
    EXPECT_NEAR(res[5], 42.0000f, 0.001f);
    EXPECT_NEAR(res[10], 44.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_21) {
    float a[16] = {
        21.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 22.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 23.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 42.0000f, 0.001f);
    EXPECT_NEAR(res[5], 44.0000f, 0.001f);
    EXPECT_NEAR(res[10], 46.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_22) {
    float a[16] = {
        22.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 23.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 24.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 44.0000f, 0.001f);
    EXPECT_NEAR(res[5], 46.0000f, 0.001f);
    EXPECT_NEAR(res[10], 48.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_23) {
    float a[16] = {
        23.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 24.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 25.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 46.0000f, 0.001f);
    EXPECT_NEAR(res[5], 48.0000f, 0.001f);
    EXPECT_NEAR(res[10], 50.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_24) {
    float a[16] = {
        24.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 25.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 26.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 48.0000f, 0.001f);
    EXPECT_NEAR(res[5], 50.0000f, 0.001f);
    EXPECT_NEAR(res[10], 52.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_25) {
    float a[16] = {
        25.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 26.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 27.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 50.0000f, 0.001f);
    EXPECT_NEAR(res[5], 52.0000f, 0.001f);
    EXPECT_NEAR(res[10], 54.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_26) {
    float a[16] = {
        26.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 27.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 28.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 52.0000f, 0.001f);
    EXPECT_NEAR(res[5], 54.0000f, 0.001f);
    EXPECT_NEAR(res[10], 56.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_27) {
    float a[16] = {
        27.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 28.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 29.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 54.0000f, 0.001f);
    EXPECT_NEAR(res[5], 56.0000f, 0.001f);
    EXPECT_NEAR(res[10], 58.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_28) {
    float a[16] = {
        28.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 29.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 30.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 56.0000f, 0.001f);
    EXPECT_NEAR(res[5], 58.0000f, 0.001f);
    EXPECT_NEAR(res[10], 60.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_29) {
    float a[16] = {
        29.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 30.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 31.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 58.0000f, 0.001f);
    EXPECT_NEAR(res[5], 60.0000f, 0.001f);
    EXPECT_NEAR(res[10], 62.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_30) {
    float a[16] = {
        30.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 31.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 32.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 60.0000f, 0.001f);
    EXPECT_NEAR(res[5], 62.0000f, 0.001f);
    EXPECT_NEAR(res[10], 64.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_31) {
    float a[16] = {
        31.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 32.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 33.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 62.0000f, 0.001f);
    EXPECT_NEAR(res[5], 64.0000f, 0.001f);
    EXPECT_NEAR(res[10], 66.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_32) {
    float a[16] = {
        32.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 33.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 34.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 64.0000f, 0.001f);
    EXPECT_NEAR(res[5], 66.0000f, 0.001f);
    EXPECT_NEAR(res[10], 68.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_33) {
    float a[16] = {
        33.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 34.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 35.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 66.0000f, 0.001f);
    EXPECT_NEAR(res[5], 68.0000f, 0.001f);
    EXPECT_NEAR(res[10], 70.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_34) {
    float a[16] = {
        34.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 35.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 36.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 68.0000f, 0.001f);
    EXPECT_NEAR(res[5], 70.0000f, 0.001f);
    EXPECT_NEAR(res[10], 72.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_35) {
    float a[16] = {
        35.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 36.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 37.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 70.0000f, 0.001f);
    EXPECT_NEAR(res[5], 72.0000f, 0.001f);
    EXPECT_NEAR(res[10], 74.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_36) {
    float a[16] = {
        36.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 37.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 38.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 72.0000f, 0.001f);
    EXPECT_NEAR(res[5], 74.0000f, 0.001f);
    EXPECT_NEAR(res[10], 76.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_37) {
    float a[16] = {
        37.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 38.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 39.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 74.0000f, 0.001f);
    EXPECT_NEAR(res[5], 76.0000f, 0.001f);
    EXPECT_NEAR(res[10], 78.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_38) {
    float a[16] = {
        38.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 39.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 40.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 76.0000f, 0.001f);
    EXPECT_NEAR(res[5], 78.0000f, 0.001f);
    EXPECT_NEAR(res[10], 80.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_39) {
    float a[16] = {
        39.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 40.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 41.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 78.0000f, 0.001f);
    EXPECT_NEAR(res[5], 80.0000f, 0.001f);
    EXPECT_NEAR(res[10], 82.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_40) {
    float a[16] = {
        40.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 41.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 42.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 80.0000f, 0.001f);
    EXPECT_NEAR(res[5], 82.0000f, 0.001f);
    EXPECT_NEAR(res[10], 84.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_41) {
    float a[16] = {
        41.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 42.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 43.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 82.0000f, 0.001f);
    EXPECT_NEAR(res[5], 84.0000f, 0.001f);
    EXPECT_NEAR(res[10], 86.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_42) {
    float a[16] = {
        42.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 43.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 44.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 84.0000f, 0.001f);
    EXPECT_NEAR(res[5], 86.0000f, 0.001f);
    EXPECT_NEAR(res[10], 88.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_43) {
    float a[16] = {
        43.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 44.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 45.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 86.0000f, 0.001f);
    EXPECT_NEAR(res[5], 88.0000f, 0.001f);
    EXPECT_NEAR(res[10], 90.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_44) {
    float a[16] = {
        44.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 45.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 46.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 88.0000f, 0.001f);
    EXPECT_NEAR(res[5], 90.0000f, 0.001f);
    EXPECT_NEAR(res[10], 92.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_45) {
    float a[16] = {
        45.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 46.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 47.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 90.0000f, 0.001f);
    EXPECT_NEAR(res[5], 92.0000f, 0.001f);
    EXPECT_NEAR(res[10], 94.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_46) {
    float a[16] = {
        46.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 47.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 48.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 92.0000f, 0.001f);
    EXPECT_NEAR(res[5], 94.0000f, 0.001f);
    EXPECT_NEAR(res[10], 96.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_47) {
    float a[16] = {
        47.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 48.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 49.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 94.0000f, 0.001f);
    EXPECT_NEAR(res[5], 96.0000f, 0.001f);
    EXPECT_NEAR(res[10], 98.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_48) {
    float a[16] = {
        48.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 49.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 50.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 96.0000f, 0.001f);
    EXPECT_NEAR(res[5], 98.0000f, 0.001f);
    EXPECT_NEAR(res[10], 100.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_Mat4Mul_49) {
    float a[16] = {
        49.0000f, 0.0f, 0.0f, 0.0f,
        0.0f, 50.0000f, 0.0f, 0.0f,
        0.0f, 0.0f, 51.0000f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float b[16] = {
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float res[16] = {0};
    starlight::simd::Mat4Mul(res, a, b);
    EXPECT_NEAR(res[0], 98.0000f, 0.001f);
    EXPECT_NEAR(res[5], 100.0000f, 0.001f);
    EXPECT_NEAR(res[10], 102.0000f, 0.001f);
    EXPECT_NEAR(res[15], 1.0f, 0.001f);
}
TEST(MathStressTest, SIMD_DotProduct8x_0) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 0.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_1) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 1.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_2) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 2.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_3) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 3.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_4) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 4.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_5) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 5.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_6) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 6.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_7) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 7.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_8) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 8.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_9) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 9.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_10) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 10.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_11) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 11.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_12) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 12.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_13) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 13.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_14) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 14.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_15) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 15.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_16) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 16.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_17) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 17.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_18) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 18.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_19) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 19.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_20) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 20.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_21) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 21.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_22) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 22.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_23) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 23.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_24) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 24.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_25) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 25.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_26) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 26.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_27) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 27.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_28) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 28.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_29) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 29.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_30) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 30.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_31) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 31.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_32) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 32.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_33) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 33.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_34) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 34.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_35) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 35.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_36) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 36.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_37) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 37.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_38) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 38.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_39) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 39.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_40) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 40.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_41) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 41.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_42) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 42.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_43) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 43.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_44) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 44.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_45) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 45.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_46) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 46.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_47) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 47.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_48) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 48.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}
TEST(MathStressTest, SIMD_DotProduct8x_49) {
    alignas(32) float a[64];
    alignas(32) float b[64];
    alignas(32) float results[8] = {0};
    for (int j = 0; j < 64; ++j) {
        a[j] = 49.0000f + static_cast<float>(j) * 0.1f;
        b[j] = 2.0f;
    }
    starlight::simd::DotProduct8x(a, b, results);
    for (int r = 0; r < 8; ++r) {
        float expected = 0.0f;
        for (int k = 0; k < 8; ++k) {
            expected += a[r * 8 + k] * b[r * 8 + k];
        }
        EXPECT_NEAR(results[r], expected, 0.05f);
    }
}

// --- 2. MEMORY ALLOCATOR STRESS TESTS (200 tests) ---
TEST(MemoryStressTest, LinearAllocator_0) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(1, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_1) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(2, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_2) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(3, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_3) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(4, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_4) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(5, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_5) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(6, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_6) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(7, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_7) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(8, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_8) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(9, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_9) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(10, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_10) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(11, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_11) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(12, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_12) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(13, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_13) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(14, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_14) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(15, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_15) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(16, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_16) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(17, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_17) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(18, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_18) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(19, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_19) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(20, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_20) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(21, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_21) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(22, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_22) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(23, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_23) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(24, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_24) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(25, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_25) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(26, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_26) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(27, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_27) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(28, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_28) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(29, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_29) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(30, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_30) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(31, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_31) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(32, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_32) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(1, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_33) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(2, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_34) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(3, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_35) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(4, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_36) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(5, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_37) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(6, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_38) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(7, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_39) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(8, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_40) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(9, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_41) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(10, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_42) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(11, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_43) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(12, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_44) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(13, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_45) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(14, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_46) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(15, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_47) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(16, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_48) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(17, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_49) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(18, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_50) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(19, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_51) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(20, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_52) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(21, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_53) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(22, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_54) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(23, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_55) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(24, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_56) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(25, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_57) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(26, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_58) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(27, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_59) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(28, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_60) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(29, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_61) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(30, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_62) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(31, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_63) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(32, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_64) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(1, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_65) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(2, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_66) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(3, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_67) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(4, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_68) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(5, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_69) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(6, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_70) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(7, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_71) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(8, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_72) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(9, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_73) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(10, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_74) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(11, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_75) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(12, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_76) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(13, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_77) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(14, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_78) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(15, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_79) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(16, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_80) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(17, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_81) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(18, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_82) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(19, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_83) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(20, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_84) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(21, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_85) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(22, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_86) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(23, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_87) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(24, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_88) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(25, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_89) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(26, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_90) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(27, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_91) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(28, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_92) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(29, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_93) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(30, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_94) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(31, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_95) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(32, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_96) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(1, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_97) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(2, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_98) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(3, 8);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, LinearAllocator_99) {
    LinearAllocator alloc(2048);
    void* ptr1 = alloc.Allocate(4, 16);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0u);
    
    void* ptr2 = alloc.Allocate(32, 8);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 8, 0u);
    
    void* ptrLarge = alloc.Allocate(3000, 8);
    EXPECT_EQ(ptrLarge, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_0) {
    PoolAllocator pool(8, 5);
    std::vector<void*> ptrs;
    for (int j = 0; j < 5; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_1) {
    PoolAllocator pool(16, 6);
    std::vector<void*> ptrs;
    for (int j = 0; j < 6; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_2) {
    PoolAllocator pool(24, 7);
    std::vector<void*> ptrs;
    for (int j = 0; j < 7; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_3) {
    PoolAllocator pool(32, 8);
    std::vector<void*> ptrs;
    for (int j = 0; j < 8; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_4) {
    PoolAllocator pool(8, 9);
    std::vector<void*> ptrs;
    for (int j = 0; j < 9; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_5) {
    PoolAllocator pool(16, 10);
    std::vector<void*> ptrs;
    for (int j = 0; j < 10; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_6) {
    PoolAllocator pool(24, 11);
    std::vector<void*> ptrs;
    for (int j = 0; j < 11; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_7) {
    PoolAllocator pool(32, 12);
    std::vector<void*> ptrs;
    for (int j = 0; j < 12; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_8) {
    PoolAllocator pool(8, 13);
    std::vector<void*> ptrs;
    for (int j = 0; j < 13; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_9) {
    PoolAllocator pool(16, 14);
    std::vector<void*> ptrs;
    for (int j = 0; j < 14; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_10) {
    PoolAllocator pool(24, 5);
    std::vector<void*> ptrs;
    for (int j = 0; j < 5; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_11) {
    PoolAllocator pool(32, 6);
    std::vector<void*> ptrs;
    for (int j = 0; j < 6; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_12) {
    PoolAllocator pool(8, 7);
    std::vector<void*> ptrs;
    for (int j = 0; j < 7; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_13) {
    PoolAllocator pool(16, 8);
    std::vector<void*> ptrs;
    for (int j = 0; j < 8; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_14) {
    PoolAllocator pool(24, 9);
    std::vector<void*> ptrs;
    for (int j = 0; j < 9; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_15) {
    PoolAllocator pool(32, 10);
    std::vector<void*> ptrs;
    for (int j = 0; j < 10; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_16) {
    PoolAllocator pool(8, 11);
    std::vector<void*> ptrs;
    for (int j = 0; j < 11; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_17) {
    PoolAllocator pool(16, 12);
    std::vector<void*> ptrs;
    for (int j = 0; j < 12; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_18) {
    PoolAllocator pool(24, 13);
    std::vector<void*> ptrs;
    for (int j = 0; j < 13; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_19) {
    PoolAllocator pool(32, 14);
    std::vector<void*> ptrs;
    for (int j = 0; j < 14; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_20) {
    PoolAllocator pool(8, 5);
    std::vector<void*> ptrs;
    for (int j = 0; j < 5; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_21) {
    PoolAllocator pool(16, 6);
    std::vector<void*> ptrs;
    for (int j = 0; j < 6; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_22) {
    PoolAllocator pool(24, 7);
    std::vector<void*> ptrs;
    for (int j = 0; j < 7; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_23) {
    PoolAllocator pool(32, 8);
    std::vector<void*> ptrs;
    for (int j = 0; j < 8; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_24) {
    PoolAllocator pool(8, 9);
    std::vector<void*> ptrs;
    for (int j = 0; j < 9; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_25) {
    PoolAllocator pool(16, 10);
    std::vector<void*> ptrs;
    for (int j = 0; j < 10; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_26) {
    PoolAllocator pool(24, 11);
    std::vector<void*> ptrs;
    for (int j = 0; j < 11; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_27) {
    PoolAllocator pool(32, 12);
    std::vector<void*> ptrs;
    for (int j = 0; j < 12; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_28) {
    PoolAllocator pool(8, 13);
    std::vector<void*> ptrs;
    for (int j = 0; j < 13; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_29) {
    PoolAllocator pool(16, 14);
    std::vector<void*> ptrs;
    for (int j = 0; j < 14; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_30) {
    PoolAllocator pool(24, 5);
    std::vector<void*> ptrs;
    for (int j = 0; j < 5; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_31) {
    PoolAllocator pool(32, 6);
    std::vector<void*> ptrs;
    for (int j = 0; j < 6; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_32) {
    PoolAllocator pool(8, 7);
    std::vector<void*> ptrs;
    for (int j = 0; j < 7; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_33) {
    PoolAllocator pool(16, 8);
    std::vector<void*> ptrs;
    for (int j = 0; j < 8; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_34) {
    PoolAllocator pool(24, 9);
    std::vector<void*> ptrs;
    for (int j = 0; j < 9; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_35) {
    PoolAllocator pool(32, 10);
    std::vector<void*> ptrs;
    for (int j = 0; j < 10; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_36) {
    PoolAllocator pool(8, 11);
    std::vector<void*> ptrs;
    for (int j = 0; j < 11; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_37) {
    PoolAllocator pool(16, 12);
    std::vector<void*> ptrs;
    for (int j = 0; j < 12; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_38) {
    PoolAllocator pool(24, 13);
    std::vector<void*> ptrs;
    for (int j = 0; j < 13; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_39) {
    PoolAllocator pool(32, 14);
    std::vector<void*> ptrs;
    for (int j = 0; j < 14; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_40) {
    PoolAllocator pool(8, 5);
    std::vector<void*> ptrs;
    for (int j = 0; j < 5; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_41) {
    PoolAllocator pool(16, 6);
    std::vector<void*> ptrs;
    for (int j = 0; j < 6; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_42) {
    PoolAllocator pool(24, 7);
    std::vector<void*> ptrs;
    for (int j = 0; j < 7; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_43) {
    PoolAllocator pool(32, 8);
    std::vector<void*> ptrs;
    for (int j = 0; j < 8; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_44) {
    PoolAllocator pool(8, 9);
    std::vector<void*> ptrs;
    for (int j = 0; j < 9; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_45) {
    PoolAllocator pool(16, 10);
    std::vector<void*> ptrs;
    for (int j = 0; j < 10; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_46) {
    PoolAllocator pool(24, 11);
    std::vector<void*> ptrs;
    for (int j = 0; j < 11; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_47) {
    PoolAllocator pool(32, 12);
    std::vector<void*> ptrs;
    for (int j = 0; j < 12; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_48) {
    PoolAllocator pool(8, 13);
    std::vector<void*> ptrs;
    for (int j = 0; j < 13; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_49) {
    PoolAllocator pool(16, 14);
    std::vector<void*> ptrs;
    for (int j = 0; j < 14; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_50) {
    PoolAllocator pool(24, 5);
    std::vector<void*> ptrs;
    for (int j = 0; j < 5; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_51) {
    PoolAllocator pool(32, 6);
    std::vector<void*> ptrs;
    for (int j = 0; j < 6; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_52) {
    PoolAllocator pool(8, 7);
    std::vector<void*> ptrs;
    for (int j = 0; j < 7; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_53) {
    PoolAllocator pool(16, 8);
    std::vector<void*> ptrs;
    for (int j = 0; j < 8; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_54) {
    PoolAllocator pool(24, 9);
    std::vector<void*> ptrs;
    for (int j = 0; j < 9; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_55) {
    PoolAllocator pool(32, 10);
    std::vector<void*> ptrs;
    for (int j = 0; j < 10; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_56) {
    PoolAllocator pool(8, 11);
    std::vector<void*> ptrs;
    for (int j = 0; j < 11; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_57) {
    PoolAllocator pool(16, 12);
    std::vector<void*> ptrs;
    for (int j = 0; j < 12; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_58) {
    PoolAllocator pool(24, 13);
    std::vector<void*> ptrs;
    for (int j = 0; j < 13; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_59) {
    PoolAllocator pool(32, 14);
    std::vector<void*> ptrs;
    for (int j = 0; j < 14; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_60) {
    PoolAllocator pool(8, 5);
    std::vector<void*> ptrs;
    for (int j = 0; j < 5; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_61) {
    PoolAllocator pool(16, 6);
    std::vector<void*> ptrs;
    for (int j = 0; j < 6; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_62) {
    PoolAllocator pool(24, 7);
    std::vector<void*> ptrs;
    for (int j = 0; j < 7; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_63) {
    PoolAllocator pool(32, 8);
    std::vector<void*> ptrs;
    for (int j = 0; j < 8; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_64) {
    PoolAllocator pool(8, 9);
    std::vector<void*> ptrs;
    for (int j = 0; j < 9; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_65) {
    PoolAllocator pool(16, 10);
    std::vector<void*> ptrs;
    for (int j = 0; j < 10; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_66) {
    PoolAllocator pool(24, 11);
    std::vector<void*> ptrs;
    for (int j = 0; j < 11; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_67) {
    PoolAllocator pool(32, 12);
    std::vector<void*> ptrs;
    for (int j = 0; j < 12; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_68) {
    PoolAllocator pool(8, 13);
    std::vector<void*> ptrs;
    for (int j = 0; j < 13; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_69) {
    PoolAllocator pool(16, 14);
    std::vector<void*> ptrs;
    for (int j = 0; j < 14; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_70) {
    PoolAllocator pool(24, 5);
    std::vector<void*> ptrs;
    for (int j = 0; j < 5; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_71) {
    PoolAllocator pool(32, 6);
    std::vector<void*> ptrs;
    for (int j = 0; j < 6; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_72) {
    PoolAllocator pool(8, 7);
    std::vector<void*> ptrs;
    for (int j = 0; j < 7; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_73) {
    PoolAllocator pool(16, 8);
    std::vector<void*> ptrs;
    for (int j = 0; j < 8; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_74) {
    PoolAllocator pool(24, 9);
    std::vector<void*> ptrs;
    for (int j = 0; j < 9; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_75) {
    PoolAllocator pool(32, 10);
    std::vector<void*> ptrs;
    for (int j = 0; j < 10; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_76) {
    PoolAllocator pool(8, 11);
    std::vector<void*> ptrs;
    for (int j = 0; j < 11; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_77) {
    PoolAllocator pool(16, 12);
    std::vector<void*> ptrs;
    for (int j = 0; j < 12; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_78) {
    PoolAllocator pool(24, 13);
    std::vector<void*> ptrs;
    for (int j = 0; j < 13; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_79) {
    PoolAllocator pool(32, 14);
    std::vector<void*> ptrs;
    for (int j = 0; j < 14; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_80) {
    PoolAllocator pool(8, 5);
    std::vector<void*> ptrs;
    for (int j = 0; j < 5; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_81) {
    PoolAllocator pool(16, 6);
    std::vector<void*> ptrs;
    for (int j = 0; j < 6; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_82) {
    PoolAllocator pool(24, 7);
    std::vector<void*> ptrs;
    for (int j = 0; j < 7; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_83) {
    PoolAllocator pool(32, 8);
    std::vector<void*> ptrs;
    for (int j = 0; j < 8; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_84) {
    PoolAllocator pool(8, 9);
    std::vector<void*> ptrs;
    for (int j = 0; j < 9; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_85) {
    PoolAllocator pool(16, 10);
    std::vector<void*> ptrs;
    for (int j = 0; j < 10; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_86) {
    PoolAllocator pool(24, 11);
    std::vector<void*> ptrs;
    for (int j = 0; j < 11; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_87) {
    PoolAllocator pool(32, 12);
    std::vector<void*> ptrs;
    for (int j = 0; j < 12; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_88) {
    PoolAllocator pool(8, 13);
    std::vector<void*> ptrs;
    for (int j = 0; j < 13; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_89) {
    PoolAllocator pool(16, 14);
    std::vector<void*> ptrs;
    for (int j = 0; j < 14; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_90) {
    PoolAllocator pool(24, 5);
    std::vector<void*> ptrs;
    for (int j = 0; j < 5; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_91) {
    PoolAllocator pool(32, 6);
    std::vector<void*> ptrs;
    for (int j = 0; j < 6; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_92) {
    PoolAllocator pool(8, 7);
    std::vector<void*> ptrs;
    for (int j = 0; j < 7; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_93) {
    PoolAllocator pool(16, 8);
    std::vector<void*> ptrs;
    for (int j = 0; j < 8; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_94) {
    PoolAllocator pool(24, 9);
    std::vector<void*> ptrs;
    for (int j = 0; j < 9; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_95) {
    PoolAllocator pool(32, 10);
    std::vector<void*> ptrs;
    for (int j = 0; j < 10; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_96) {
    PoolAllocator pool(8, 11);
    std::vector<void*> ptrs;
    for (int j = 0; j < 11; ++j) {
        void* p = pool.Allocate(8);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(8), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(8);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_97) {
    PoolAllocator pool(16, 12);
    std::vector<void*> ptrs;
    for (int j = 0; j < 12; ++j) {
        void* p = pool.Allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(16), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(16);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_98) {
    PoolAllocator pool(24, 13);
    std::vector<void*> ptrs;
    for (int j = 0; j < 13; ++j) {
        void* p = pool.Allocate(24);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(24), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(24);
    EXPECT_NE(pRetry, nullptr);
}
TEST(MemoryStressTest, PoolAllocator_99) {
    PoolAllocator pool(32, 14);
    std::vector<void*> ptrs;
    for (int j = 0; j < 14; ++j) {
        void* p = pool.Allocate(32);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    EXPECT_EQ(pool.Allocate(32), nullptr);
    for (void* p : ptrs) {
        pool.Deallocate(p);
    }
    void* pRetry = pool.Allocate(32);
    EXPECT_NE(pRetry, nullptr);
}

// --- 3. EASE/TWEEN STRESS TESTS (200 tests) ---
TEST(TweenStressTest, EasingCurves_0) {
    float t_val = 0.0000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_1) {
    float t_val = 0.0100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_2) {
    float t_val = 0.0200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_3) {
    float t_val = 0.0300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_4) {
    float t_val = 0.0400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_5) {
    float t_val = 0.0500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_6) {
    float t_val = 0.0600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_7) {
    float t_val = 0.0700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_8) {
    float t_val = 0.0800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_9) {
    float t_val = 0.0900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_10) {
    float t_val = 0.1000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_11) {
    float t_val = 0.1100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_12) {
    float t_val = 0.1200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_13) {
    float t_val = 0.1300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_14) {
    float t_val = 0.1400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_15) {
    float t_val = 0.1500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_16) {
    float t_val = 0.1600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_17) {
    float t_val = 0.1700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_18) {
    float t_val = 0.1800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_19) {
    float t_val = 0.1900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_20) {
    float t_val = 0.2000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_21) {
    float t_val = 0.2100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_22) {
    float t_val = 0.2200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_23) {
    float t_val = 0.2300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_24) {
    float t_val = 0.2400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_25) {
    float t_val = 0.2500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_26) {
    float t_val = 0.2600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_27) {
    float t_val = 0.2700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_28) {
    float t_val = 0.2800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_29) {
    float t_val = 0.2900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_30) {
    float t_val = 0.3000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_31) {
    float t_val = 0.3100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_32) {
    float t_val = 0.3200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_33) {
    float t_val = 0.3300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_34) {
    float t_val = 0.3400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_35) {
    float t_val = 0.3500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_36) {
    float t_val = 0.3600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_37) {
    float t_val = 0.3700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_38) {
    float t_val = 0.3800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_39) {
    float t_val = 0.3900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_40) {
    float t_val = 0.4000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_41) {
    float t_val = 0.4100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_42) {
    float t_val = 0.4200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_43) {
    float t_val = 0.4300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_44) {
    float t_val = 0.4400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_45) {
    float t_val = 0.4500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_46) {
    float t_val = 0.4600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_47) {
    float t_val = 0.4700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_48) {
    float t_val = 0.4800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_49) {
    float t_val = 0.4900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_50) {
    float t_val = 0.5000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_51) {
    float t_val = 0.5100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_52) {
    float t_val = 0.5200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_53) {
    float t_val = 0.5300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_54) {
    float t_val = 0.5400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_55) {
    float t_val = 0.5500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_56) {
    float t_val = 0.5600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_57) {
    float t_val = 0.5700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_58) {
    float t_val = 0.5800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_59) {
    float t_val = 0.5900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_60) {
    float t_val = 0.6000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_61) {
    float t_val = 0.6100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_62) {
    float t_val = 0.6200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_63) {
    float t_val = 0.6300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_64) {
    float t_val = 0.6400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_65) {
    float t_val = 0.6500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_66) {
    float t_val = 0.6600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_67) {
    float t_val = 0.6700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_68) {
    float t_val = 0.6800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_69) {
    float t_val = 0.6900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_70) {
    float t_val = 0.7000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_71) {
    float t_val = 0.7100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_72) {
    float t_val = 0.7200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_73) {
    float t_val = 0.7300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_74) {
    float t_val = 0.7400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_75) {
    float t_val = 0.7500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_76) {
    float t_val = 0.7600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_77) {
    float t_val = 0.7700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_78) {
    float t_val = 0.7800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_79) {
    float t_val = 0.7900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_80) {
    float t_val = 0.8000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_81) {
    float t_val = 0.8100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_82) {
    float t_val = 0.8200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_83) {
    float t_val = 0.8300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_84) {
    float t_val = 0.8400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_85) {
    float t_val = 0.8500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_86) {
    float t_val = 0.8600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_87) {
    float t_val = 0.8700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_88) {
    float t_val = 0.8800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_89) {
    float t_val = 0.8900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_90) {
    float t_val = 0.9000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_91) {
    float t_val = 0.9100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_92) {
    float t_val = 0.9200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_93) {
    float t_val = 0.9300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_94) {
    float t_val = 0.9400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_95) {
    float t_val = 0.9500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_96) {
    float t_val = 0.9600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_97) {
    float t_val = 0.9700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_98) {
    float t_val = 0.9800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_99) {
    float t_val = 0.9900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_100) {
    float t_val = 0.0000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_101) {
    float t_val = 0.0100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_102) {
    float t_val = 0.0200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_103) {
    float t_val = 0.0300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_104) {
    float t_val = 0.0400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_105) {
    float t_val = 0.0500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_106) {
    float t_val = 0.0600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_107) {
    float t_val = 0.0700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_108) {
    float t_val = 0.0800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_109) {
    float t_val = 0.0900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_110) {
    float t_val = 0.1000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_111) {
    float t_val = 0.1100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_112) {
    float t_val = 0.1200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_113) {
    float t_val = 0.1300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_114) {
    float t_val = 0.1400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_115) {
    float t_val = 0.1500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_116) {
    float t_val = 0.1600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_117) {
    float t_val = 0.1700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_118) {
    float t_val = 0.1800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_119) {
    float t_val = 0.1900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_120) {
    float t_val = 0.2000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_121) {
    float t_val = 0.2100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_122) {
    float t_val = 0.2200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_123) {
    float t_val = 0.2300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_124) {
    float t_val = 0.2400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_125) {
    float t_val = 0.2500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_126) {
    float t_val = 0.2600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_127) {
    float t_val = 0.2700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_128) {
    float t_val = 0.2800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_129) {
    float t_val = 0.2900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_130) {
    float t_val = 0.3000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_131) {
    float t_val = 0.3100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_132) {
    float t_val = 0.3200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_133) {
    float t_val = 0.3300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_134) {
    float t_val = 0.3400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_135) {
    float t_val = 0.3500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_136) {
    float t_val = 0.3600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_137) {
    float t_val = 0.3700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_138) {
    float t_val = 0.3800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_139) {
    float t_val = 0.3900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_140) {
    float t_val = 0.4000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_141) {
    float t_val = 0.4100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_142) {
    float t_val = 0.4200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_143) {
    float t_val = 0.4300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_144) {
    float t_val = 0.4400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_145) {
    float t_val = 0.4500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_146) {
    float t_val = 0.4600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_147) {
    float t_val = 0.4700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_148) {
    float t_val = 0.4800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_149) {
    float t_val = 0.4900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_150) {
    float t_val = 0.5000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_151) {
    float t_val = 0.5100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_152) {
    float t_val = 0.5200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_153) {
    float t_val = 0.5300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_154) {
    float t_val = 0.5400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_155) {
    float t_val = 0.5500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_156) {
    float t_val = 0.5600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_157) {
    float t_val = 0.5700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_158) {
    float t_val = 0.5800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_159) {
    float t_val = 0.5900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_160) {
    float t_val = 0.6000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_161) {
    float t_val = 0.6100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_162) {
    float t_val = 0.6200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_163) {
    float t_val = 0.6300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_164) {
    float t_val = 0.6400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_165) {
    float t_val = 0.6500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_166) {
    float t_val = 0.6600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_167) {
    float t_val = 0.6700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_168) {
    float t_val = 0.6800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_169) {
    float t_val = 0.6900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_170) {
    float t_val = 0.7000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_171) {
    float t_val = 0.7100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_172) {
    float t_val = 0.7200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_173) {
    float t_val = 0.7300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_174) {
    float t_val = 0.7400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_175) {
    float t_val = 0.7500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_176) {
    float t_val = 0.7600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_177) {
    float t_val = 0.7700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_178) {
    float t_val = 0.7800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_179) {
    float t_val = 0.7900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_180) {
    float t_val = 0.8000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_181) {
    float t_val = 0.8100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_182) {
    float t_val = 0.8200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_183) {
    float t_val = 0.8300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_184) {
    float t_val = 0.8400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_185) {
    float t_val = 0.8500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_186) {
    float t_val = 0.8600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_187) {
    float t_val = 0.8700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_188) {
    float t_val = 0.8800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_189) {
    float t_val = 0.8900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outQuad");
    float val = easeFunc(t_val);
    float expected = Easing::OutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_190) {
    float t_val = 0.9000f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InOutQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_191) {
    float t_val = 0.9100f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_192) {
    float t_val = 0.9200f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outCubic");
    float val = easeFunc(t_val);
    float expected = Easing::OutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_193) {
    float t_val = 0.9300f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inOutCubic");
    float val = easeFunc(t_val);
    float expected = Easing::InOutCubic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_194) {
    float t_val = 0.9400f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inExpo");
    float val = easeFunc(t_val);
    float expected = Easing::InExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_195) {
    float t_val = 0.9500f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outExpo");
    float val = easeFunc(t_val);
    float expected = Easing::OutExpo(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_196) {
    float t_val = 0.9600f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outElastic");
    float val = easeFunc(t_val);
    float expected = Easing::OutElastic(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_197) {
    float t_val = 0.9700f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("outBounce");
    float val = easeFunc(t_val);
    float expected = Easing::OutBounce(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_198) {
    float t_val = 0.9800f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("Linear");
    float val = easeFunc(t_val);
    float expected = Easing::Linear(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}
TEST(TweenStressTest, EasingCurves_199) {
    float t_val = 0.9900f;
    EXPECT_NEAR(Easing::Linear(t_val), t_val, 0.001f);
    
    auto easeFunc = Easing::GetEaseFunc("inQuad");
    float val = easeFunc(t_val);
    float expected = Easing::InQuad(t_val);
    EXPECT_NEAR(val, expected, 0.001f);
}

// --- 4. INPUT SYSTEM KEY MAPPINGS (200 tests) ---
TEST(InputStressTest, KeyMappings_0) {
    std::string name = "A";
    pal::KeyCode expected_code = pal::KeyCode::A;
    int expected_scancode = 4;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_1) {
    std::string name = "B";
    pal::KeyCode expected_code = pal::KeyCode::B;
    int expected_scancode = 5;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_2) {
    std::string name = "C";
    pal::KeyCode expected_code = pal::KeyCode::C;
    int expected_scancode = 6;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_3) {
    std::string name = "D";
    pal::KeyCode expected_code = pal::KeyCode::D;
    int expected_scancode = 7;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_4) {
    std::string name = "E";
    pal::KeyCode expected_code = pal::KeyCode::E;
    int expected_scancode = 8;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_5) {
    std::string name = "F";
    pal::KeyCode expected_code = pal::KeyCode::F;
    int expected_scancode = 9;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_6) {
    std::string name = "G";
    pal::KeyCode expected_code = pal::KeyCode::G;
    int expected_scancode = 10;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_7) {
    std::string name = "H";
    pal::KeyCode expected_code = pal::KeyCode::H;
    int expected_scancode = 11;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_8) {
    std::string name = "I";
    pal::KeyCode expected_code = pal::KeyCode::I;
    int expected_scancode = 12;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_9) {
    std::string name = "J";
    pal::KeyCode expected_code = pal::KeyCode::J;
    int expected_scancode = 13;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_10) {
    std::string name = "K";
    pal::KeyCode expected_code = pal::KeyCode::K;
    int expected_scancode = 14;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_11) {
    std::string name = "L";
    pal::KeyCode expected_code = pal::KeyCode::L;
    int expected_scancode = 15;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_12) {
    std::string name = "M";
    pal::KeyCode expected_code = pal::KeyCode::M;
    int expected_scancode = 16;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_13) {
    std::string name = "N";
    pal::KeyCode expected_code = pal::KeyCode::N;
    int expected_scancode = 17;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_14) {
    std::string name = "O";
    pal::KeyCode expected_code = pal::KeyCode::O;
    int expected_scancode = 18;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_15) {
    std::string name = "P";
    pal::KeyCode expected_code = pal::KeyCode::P;
    int expected_scancode = 19;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_16) {
    std::string name = "Q";
    pal::KeyCode expected_code = pal::KeyCode::Q;
    int expected_scancode = 20;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_17) {
    std::string name = "R";
    pal::KeyCode expected_code = pal::KeyCode::R;
    int expected_scancode = 21;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_18) {
    std::string name = "S";
    pal::KeyCode expected_code = pal::KeyCode::S;
    int expected_scancode = 22;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_19) {
    std::string name = "T";
    pal::KeyCode expected_code = pal::KeyCode::T;
    int expected_scancode = 23;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_20) {
    std::string name = "U";
    pal::KeyCode expected_code = pal::KeyCode::U;
    int expected_scancode = 24;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_21) {
    std::string name = "V";
    pal::KeyCode expected_code = pal::KeyCode::V;
    int expected_scancode = 25;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_22) {
    std::string name = "W";
    pal::KeyCode expected_code = pal::KeyCode::W;
    int expected_scancode = 26;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_23) {
    std::string name = "X";
    pal::KeyCode expected_code = pal::KeyCode::X;
    int expected_scancode = 27;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_24) {
    std::string name = "Y";
    pal::KeyCode expected_code = pal::KeyCode::Y;
    int expected_scancode = 28;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_25) {
    std::string name = "Z";
    pal::KeyCode expected_code = pal::KeyCode::Z;
    int expected_scancode = 29;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_26) {
    std::string name = "1";
    pal::KeyCode expected_code = pal::KeyCode::Num1;
    int expected_scancode = 30;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_27) {
    std::string name = "2";
    pal::KeyCode expected_code = pal::KeyCode::Num2;
    int expected_scancode = 31;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_28) {
    std::string name = "3";
    pal::KeyCode expected_code = pal::KeyCode::Num3;
    int expected_scancode = 32;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_29) {
    std::string name = "4";
    pal::KeyCode expected_code = pal::KeyCode::Num4;
    int expected_scancode = 33;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_30) {
    std::string name = "5";
    pal::KeyCode expected_code = pal::KeyCode::Num5;
    int expected_scancode = 34;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_31) {
    std::string name = "6";
    pal::KeyCode expected_code = pal::KeyCode::Num6;
    int expected_scancode = 35;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_32) {
    std::string name = "7";
    pal::KeyCode expected_code = pal::KeyCode::Num7;
    int expected_scancode = 36;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_33) {
    std::string name = "8";
    pal::KeyCode expected_code = pal::KeyCode::Num8;
    int expected_scancode = 37;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_34) {
    std::string name = "9";
    pal::KeyCode expected_code = pal::KeyCode::Num9;
    int expected_scancode = 38;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_35) {
    std::string name = "0";
    pal::KeyCode expected_code = pal::KeyCode::Num0;
    int expected_scancode = 39;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_36) {
    std::string name = "Return";
    pal::KeyCode expected_code = pal::KeyCode::Return;
    int expected_scancode = 40;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_37) {
    std::string name = "Escape";
    pal::KeyCode expected_code = pal::KeyCode::Escape;
    int expected_scancode = 41;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_38) {
    std::string name = "Backspace";
    pal::KeyCode expected_code = pal::KeyCode::Backspace;
    int expected_scancode = 42;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_39) {
    std::string name = "Tab";
    pal::KeyCode expected_code = pal::KeyCode::Tab;
    int expected_scancode = 43;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_40) {
    std::string name = "Space";
    pal::KeyCode expected_code = pal::KeyCode::Space;
    int expected_scancode = 44;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_41) {
    std::string name = "F1";
    pal::KeyCode expected_code = pal::KeyCode::F1;
    int expected_scancode = 58;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_42) {
    std::string name = "F2";
    pal::KeyCode expected_code = pal::KeyCode::F2;
    int expected_scancode = 59;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_43) {
    std::string name = "F3";
    pal::KeyCode expected_code = pal::KeyCode::F3;
    int expected_scancode = 60;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_44) {
    std::string name = "F4";
    pal::KeyCode expected_code = pal::KeyCode::F4;
    int expected_scancode = 61;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_45) {
    std::string name = "F5";
    pal::KeyCode expected_code = pal::KeyCode::F5;
    int expected_scancode = 62;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_46) {
    std::string name = "F6";
    pal::KeyCode expected_code = pal::KeyCode::F6;
    int expected_scancode = 63;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_47) {
    std::string name = "F7";
    pal::KeyCode expected_code = pal::KeyCode::F7;
    int expected_scancode = 64;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_48) {
    std::string name = "F8";
    pal::KeyCode expected_code = pal::KeyCode::F8;
    int expected_scancode = 65;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_49) {
    std::string name = "F9";
    pal::KeyCode expected_code = pal::KeyCode::F9;
    int expected_scancode = 66;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_50) {
    std::string name = "F10";
    pal::KeyCode expected_code = pal::KeyCode::F10;
    int expected_scancode = 67;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_51) {
    std::string name = "F11";
    pal::KeyCode expected_code = pal::KeyCode::F11;
    int expected_scancode = 68;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_52) {
    std::string name = "F12";
    pal::KeyCode expected_code = pal::KeyCode::F12;
    int expected_scancode = 69;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_53) {
    std::string name = "PrintScreen";
    pal::KeyCode expected_code = pal::KeyCode::PrintScreen;
    int expected_scancode = 70;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_54) {
    std::string name = "ScrollLock";
    pal::KeyCode expected_code = pal::KeyCode::ScrollLock;
    int expected_scancode = 71;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_55) {
    std::string name = "Pause";
    pal::KeyCode expected_code = pal::KeyCode::Pause;
    int expected_scancode = 72;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_56) {
    std::string name = "Insert";
    pal::KeyCode expected_code = pal::KeyCode::Insert;
    int expected_scancode = 73;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_57) {
    std::string name = "Home";
    pal::KeyCode expected_code = pal::KeyCode::Home;
    int expected_scancode = 74;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_58) {
    std::string name = "PageUp";
    pal::KeyCode expected_code = pal::KeyCode::PageUp;
    int expected_scancode = 75;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_59) {
    std::string name = "Delete";
    pal::KeyCode expected_code = pal::KeyCode::Delete;
    int expected_scancode = 76;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_60) {
    std::string name = "End";
    pal::KeyCode expected_code = pal::KeyCode::End;
    int expected_scancode = 77;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_61) {
    std::string name = "PageDown";
    pal::KeyCode expected_code = pal::KeyCode::PageDown;
    int expected_scancode = 78;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_62) {
    std::string name = "Right";
    pal::KeyCode expected_code = pal::KeyCode::Right;
    int expected_scancode = 79;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_63) {
    std::string name = "Left";
    pal::KeyCode expected_code = pal::KeyCode::Left;
    int expected_scancode = 80;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_64) {
    std::string name = "Down";
    pal::KeyCode expected_code = pal::KeyCode::Down;
    int expected_scancode = 81;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_65) {
    std::string name = "Up";
    pal::KeyCode expected_code = pal::KeyCode::Up;
    int expected_scancode = 82;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_66) {
    std::string name = "LShift";
    pal::KeyCode expected_code = pal::KeyCode::LShift;
    int expected_scancode = 225;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_67) {
    std::string name = "LCtrl";
    pal::KeyCode expected_code = pal::KeyCode::LCtrl;
    int expected_scancode = 224;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_68) {
    std::string name = "LAlt";
    pal::KeyCode expected_code = pal::KeyCode::LAlt;
    int expected_scancode = 226;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_69) {
    std::string name = "RShift";
    pal::KeyCode expected_code = pal::KeyCode::RShift;
    int expected_scancode = 229;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_70) {
    std::string name = "RCtrl";
    pal::KeyCode expected_code = pal::KeyCode::RCtrl;
    int expected_scancode = 228;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_71) {
    std::string name = "RAlt";
    pal::KeyCode expected_code = pal::KeyCode::RAlt;
    int expected_scancode = 230;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_72) {
    std::string name = "A";
    pal::KeyCode expected_code = pal::KeyCode::A;
    int expected_scancode = 4;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_73) {
    std::string name = "B";
    pal::KeyCode expected_code = pal::KeyCode::B;
    int expected_scancode = 5;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_74) {
    std::string name = "C";
    pal::KeyCode expected_code = pal::KeyCode::C;
    int expected_scancode = 6;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_75) {
    std::string name = "D";
    pal::KeyCode expected_code = pal::KeyCode::D;
    int expected_scancode = 7;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_76) {
    std::string name = "E";
    pal::KeyCode expected_code = pal::KeyCode::E;
    int expected_scancode = 8;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_77) {
    std::string name = "F";
    pal::KeyCode expected_code = pal::KeyCode::F;
    int expected_scancode = 9;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_78) {
    std::string name = "G";
    pal::KeyCode expected_code = pal::KeyCode::G;
    int expected_scancode = 10;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_79) {
    std::string name = "H";
    pal::KeyCode expected_code = pal::KeyCode::H;
    int expected_scancode = 11;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_80) {
    std::string name = "I";
    pal::KeyCode expected_code = pal::KeyCode::I;
    int expected_scancode = 12;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_81) {
    std::string name = "J";
    pal::KeyCode expected_code = pal::KeyCode::J;
    int expected_scancode = 13;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_82) {
    std::string name = "K";
    pal::KeyCode expected_code = pal::KeyCode::K;
    int expected_scancode = 14;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_83) {
    std::string name = "L";
    pal::KeyCode expected_code = pal::KeyCode::L;
    int expected_scancode = 15;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_84) {
    std::string name = "M";
    pal::KeyCode expected_code = pal::KeyCode::M;
    int expected_scancode = 16;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_85) {
    std::string name = "N";
    pal::KeyCode expected_code = pal::KeyCode::N;
    int expected_scancode = 17;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_86) {
    std::string name = "O";
    pal::KeyCode expected_code = pal::KeyCode::O;
    int expected_scancode = 18;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_87) {
    std::string name = "P";
    pal::KeyCode expected_code = pal::KeyCode::P;
    int expected_scancode = 19;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_88) {
    std::string name = "Q";
    pal::KeyCode expected_code = pal::KeyCode::Q;
    int expected_scancode = 20;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_89) {
    std::string name = "R";
    pal::KeyCode expected_code = pal::KeyCode::R;
    int expected_scancode = 21;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_90) {
    std::string name = "S";
    pal::KeyCode expected_code = pal::KeyCode::S;
    int expected_scancode = 22;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_91) {
    std::string name = "T";
    pal::KeyCode expected_code = pal::KeyCode::T;
    int expected_scancode = 23;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_92) {
    std::string name = "U";
    pal::KeyCode expected_code = pal::KeyCode::U;
    int expected_scancode = 24;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_93) {
    std::string name = "V";
    pal::KeyCode expected_code = pal::KeyCode::V;
    int expected_scancode = 25;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_94) {
    std::string name = "W";
    pal::KeyCode expected_code = pal::KeyCode::W;
    int expected_scancode = 26;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_95) {
    std::string name = "X";
    pal::KeyCode expected_code = pal::KeyCode::X;
    int expected_scancode = 27;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_96) {
    std::string name = "Y";
    pal::KeyCode expected_code = pal::KeyCode::Y;
    int expected_scancode = 28;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_97) {
    std::string name = "Z";
    pal::KeyCode expected_code = pal::KeyCode::Z;
    int expected_scancode = 29;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_98) {
    std::string name = "1";
    pal::KeyCode expected_code = pal::KeyCode::Num1;
    int expected_scancode = 30;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_99) {
    std::string name = "2";
    pal::KeyCode expected_code = pal::KeyCode::Num2;
    int expected_scancode = 31;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_100) {
    std::string name = "3";
    pal::KeyCode expected_code = pal::KeyCode::Num3;
    int expected_scancode = 32;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_101) {
    std::string name = "4";
    pal::KeyCode expected_code = pal::KeyCode::Num4;
    int expected_scancode = 33;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_102) {
    std::string name = "5";
    pal::KeyCode expected_code = pal::KeyCode::Num5;
    int expected_scancode = 34;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_103) {
    std::string name = "6";
    pal::KeyCode expected_code = pal::KeyCode::Num6;
    int expected_scancode = 35;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_104) {
    std::string name = "7";
    pal::KeyCode expected_code = pal::KeyCode::Num7;
    int expected_scancode = 36;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_105) {
    std::string name = "8";
    pal::KeyCode expected_code = pal::KeyCode::Num8;
    int expected_scancode = 37;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_106) {
    std::string name = "9";
    pal::KeyCode expected_code = pal::KeyCode::Num9;
    int expected_scancode = 38;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_107) {
    std::string name = "0";
    pal::KeyCode expected_code = pal::KeyCode::Num0;
    int expected_scancode = 39;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_108) {
    std::string name = "Return";
    pal::KeyCode expected_code = pal::KeyCode::Return;
    int expected_scancode = 40;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_109) {
    std::string name = "Escape";
    pal::KeyCode expected_code = pal::KeyCode::Escape;
    int expected_scancode = 41;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_110) {
    std::string name = "Backspace";
    pal::KeyCode expected_code = pal::KeyCode::Backspace;
    int expected_scancode = 42;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_111) {
    std::string name = "Tab";
    pal::KeyCode expected_code = pal::KeyCode::Tab;
    int expected_scancode = 43;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_112) {
    std::string name = "Space";
    pal::KeyCode expected_code = pal::KeyCode::Space;
    int expected_scancode = 44;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_113) {
    std::string name = "F1";
    pal::KeyCode expected_code = pal::KeyCode::F1;
    int expected_scancode = 58;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_114) {
    std::string name = "F2";
    pal::KeyCode expected_code = pal::KeyCode::F2;
    int expected_scancode = 59;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_115) {
    std::string name = "F3";
    pal::KeyCode expected_code = pal::KeyCode::F3;
    int expected_scancode = 60;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_116) {
    std::string name = "F4";
    pal::KeyCode expected_code = pal::KeyCode::F4;
    int expected_scancode = 61;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_117) {
    std::string name = "F5";
    pal::KeyCode expected_code = pal::KeyCode::F5;
    int expected_scancode = 62;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_118) {
    std::string name = "F6";
    pal::KeyCode expected_code = pal::KeyCode::F6;
    int expected_scancode = 63;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_119) {
    std::string name = "F7";
    pal::KeyCode expected_code = pal::KeyCode::F7;
    int expected_scancode = 64;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_120) {
    std::string name = "F8";
    pal::KeyCode expected_code = pal::KeyCode::F8;
    int expected_scancode = 65;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_121) {
    std::string name = "F9";
    pal::KeyCode expected_code = pal::KeyCode::F9;
    int expected_scancode = 66;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_122) {
    std::string name = "F10";
    pal::KeyCode expected_code = pal::KeyCode::F10;
    int expected_scancode = 67;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_123) {
    std::string name = "F11";
    pal::KeyCode expected_code = pal::KeyCode::F11;
    int expected_scancode = 68;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_124) {
    std::string name = "F12";
    pal::KeyCode expected_code = pal::KeyCode::F12;
    int expected_scancode = 69;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_125) {
    std::string name = "PrintScreen";
    pal::KeyCode expected_code = pal::KeyCode::PrintScreen;
    int expected_scancode = 70;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_126) {
    std::string name = "ScrollLock";
    pal::KeyCode expected_code = pal::KeyCode::ScrollLock;
    int expected_scancode = 71;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_127) {
    std::string name = "Pause";
    pal::KeyCode expected_code = pal::KeyCode::Pause;
    int expected_scancode = 72;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_128) {
    std::string name = "Insert";
    pal::KeyCode expected_code = pal::KeyCode::Insert;
    int expected_scancode = 73;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_129) {
    std::string name = "Home";
    pal::KeyCode expected_code = pal::KeyCode::Home;
    int expected_scancode = 74;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_130) {
    std::string name = "PageUp";
    pal::KeyCode expected_code = pal::KeyCode::PageUp;
    int expected_scancode = 75;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_131) {
    std::string name = "Delete";
    pal::KeyCode expected_code = pal::KeyCode::Delete;
    int expected_scancode = 76;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_132) {
    std::string name = "End";
    pal::KeyCode expected_code = pal::KeyCode::End;
    int expected_scancode = 77;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_133) {
    std::string name = "PageDown";
    pal::KeyCode expected_code = pal::KeyCode::PageDown;
    int expected_scancode = 78;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_134) {
    std::string name = "Right";
    pal::KeyCode expected_code = pal::KeyCode::Right;
    int expected_scancode = 79;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_135) {
    std::string name = "Left";
    pal::KeyCode expected_code = pal::KeyCode::Left;
    int expected_scancode = 80;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_136) {
    std::string name = "Down";
    pal::KeyCode expected_code = pal::KeyCode::Down;
    int expected_scancode = 81;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_137) {
    std::string name = "Up";
    pal::KeyCode expected_code = pal::KeyCode::Up;
    int expected_scancode = 82;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_138) {
    std::string name = "LShift";
    pal::KeyCode expected_code = pal::KeyCode::LShift;
    int expected_scancode = 225;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_139) {
    std::string name = "LCtrl";
    pal::KeyCode expected_code = pal::KeyCode::LCtrl;
    int expected_scancode = 224;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_140) {
    std::string name = "LAlt";
    pal::KeyCode expected_code = pal::KeyCode::LAlt;
    int expected_scancode = 226;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_141) {
    std::string name = "RShift";
    pal::KeyCode expected_code = pal::KeyCode::RShift;
    int expected_scancode = 229;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_142) {
    std::string name = "RCtrl";
    pal::KeyCode expected_code = pal::KeyCode::RCtrl;
    int expected_scancode = 228;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_143) {
    std::string name = "RAlt";
    pal::KeyCode expected_code = pal::KeyCode::RAlt;
    int expected_scancode = 230;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_144) {
    std::string name = "A";
    pal::KeyCode expected_code = pal::KeyCode::A;
    int expected_scancode = 4;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_145) {
    std::string name = "B";
    pal::KeyCode expected_code = pal::KeyCode::B;
    int expected_scancode = 5;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_146) {
    std::string name = "C";
    pal::KeyCode expected_code = pal::KeyCode::C;
    int expected_scancode = 6;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_147) {
    std::string name = "D";
    pal::KeyCode expected_code = pal::KeyCode::D;
    int expected_scancode = 7;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_148) {
    std::string name = "E";
    pal::KeyCode expected_code = pal::KeyCode::E;
    int expected_scancode = 8;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_149) {
    std::string name = "F";
    pal::KeyCode expected_code = pal::KeyCode::F;
    int expected_scancode = 9;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_150) {
    std::string name = "G";
    pal::KeyCode expected_code = pal::KeyCode::G;
    int expected_scancode = 10;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_151) {
    std::string name = "H";
    pal::KeyCode expected_code = pal::KeyCode::H;
    int expected_scancode = 11;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_152) {
    std::string name = "I";
    pal::KeyCode expected_code = pal::KeyCode::I;
    int expected_scancode = 12;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_153) {
    std::string name = "J";
    pal::KeyCode expected_code = pal::KeyCode::J;
    int expected_scancode = 13;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_154) {
    std::string name = "K";
    pal::KeyCode expected_code = pal::KeyCode::K;
    int expected_scancode = 14;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_155) {
    std::string name = "L";
    pal::KeyCode expected_code = pal::KeyCode::L;
    int expected_scancode = 15;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_156) {
    std::string name = "M";
    pal::KeyCode expected_code = pal::KeyCode::M;
    int expected_scancode = 16;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_157) {
    std::string name = "N";
    pal::KeyCode expected_code = pal::KeyCode::N;
    int expected_scancode = 17;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_158) {
    std::string name = "O";
    pal::KeyCode expected_code = pal::KeyCode::O;
    int expected_scancode = 18;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_159) {
    std::string name = "P";
    pal::KeyCode expected_code = pal::KeyCode::P;
    int expected_scancode = 19;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_160) {
    std::string name = "Q";
    pal::KeyCode expected_code = pal::KeyCode::Q;
    int expected_scancode = 20;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_161) {
    std::string name = "R";
    pal::KeyCode expected_code = pal::KeyCode::R;
    int expected_scancode = 21;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_162) {
    std::string name = "S";
    pal::KeyCode expected_code = pal::KeyCode::S;
    int expected_scancode = 22;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_163) {
    std::string name = "T";
    pal::KeyCode expected_code = pal::KeyCode::T;
    int expected_scancode = 23;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_164) {
    std::string name = "U";
    pal::KeyCode expected_code = pal::KeyCode::U;
    int expected_scancode = 24;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_165) {
    std::string name = "V";
    pal::KeyCode expected_code = pal::KeyCode::V;
    int expected_scancode = 25;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_166) {
    std::string name = "W";
    pal::KeyCode expected_code = pal::KeyCode::W;
    int expected_scancode = 26;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_167) {
    std::string name = "X";
    pal::KeyCode expected_code = pal::KeyCode::X;
    int expected_scancode = 27;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_168) {
    std::string name = "Y";
    pal::KeyCode expected_code = pal::KeyCode::Y;
    int expected_scancode = 28;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_169) {
    std::string name = "Z";
    pal::KeyCode expected_code = pal::KeyCode::Z;
    int expected_scancode = 29;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_170) {
    std::string name = "1";
    pal::KeyCode expected_code = pal::KeyCode::Num1;
    int expected_scancode = 30;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_171) {
    std::string name = "2";
    pal::KeyCode expected_code = pal::KeyCode::Num2;
    int expected_scancode = 31;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_172) {
    std::string name = "3";
    pal::KeyCode expected_code = pal::KeyCode::Num3;
    int expected_scancode = 32;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_173) {
    std::string name = "4";
    pal::KeyCode expected_code = pal::KeyCode::Num4;
    int expected_scancode = 33;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_174) {
    std::string name = "5";
    pal::KeyCode expected_code = pal::KeyCode::Num5;
    int expected_scancode = 34;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_175) {
    std::string name = "6";
    pal::KeyCode expected_code = pal::KeyCode::Num6;
    int expected_scancode = 35;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_176) {
    std::string name = "7";
    pal::KeyCode expected_code = pal::KeyCode::Num7;
    int expected_scancode = 36;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_177) {
    std::string name = "8";
    pal::KeyCode expected_code = pal::KeyCode::Num8;
    int expected_scancode = 37;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_178) {
    std::string name = "9";
    pal::KeyCode expected_code = pal::KeyCode::Num9;
    int expected_scancode = 38;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_179) {
    std::string name = "0";
    pal::KeyCode expected_code = pal::KeyCode::Num0;
    int expected_scancode = 39;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_180) {
    std::string name = "Return";
    pal::KeyCode expected_code = pal::KeyCode::Return;
    int expected_scancode = 40;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_181) {
    std::string name = "Escape";
    pal::KeyCode expected_code = pal::KeyCode::Escape;
    int expected_scancode = 41;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_182) {
    std::string name = "Backspace";
    pal::KeyCode expected_code = pal::KeyCode::Backspace;
    int expected_scancode = 42;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_183) {
    std::string name = "Tab";
    pal::KeyCode expected_code = pal::KeyCode::Tab;
    int expected_scancode = 43;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_184) {
    std::string name = "Space";
    pal::KeyCode expected_code = pal::KeyCode::Space;
    int expected_scancode = 44;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_185) {
    std::string name = "F1";
    pal::KeyCode expected_code = pal::KeyCode::F1;
    int expected_scancode = 58;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_186) {
    std::string name = "F2";
    pal::KeyCode expected_code = pal::KeyCode::F2;
    int expected_scancode = 59;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_187) {
    std::string name = "F3";
    pal::KeyCode expected_code = pal::KeyCode::F3;
    int expected_scancode = 60;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_188) {
    std::string name = "F4";
    pal::KeyCode expected_code = pal::KeyCode::F4;
    int expected_scancode = 61;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_189) {
    std::string name = "F5";
    pal::KeyCode expected_code = pal::KeyCode::F5;
    int expected_scancode = 62;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_190) {
    std::string name = "F6";
    pal::KeyCode expected_code = pal::KeyCode::F6;
    int expected_scancode = 63;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_191) {
    std::string name = "F7";
    pal::KeyCode expected_code = pal::KeyCode::F7;
    int expected_scancode = 64;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_192) {
    std::string name = "F8";
    pal::KeyCode expected_code = pal::KeyCode::F8;
    int expected_scancode = 65;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_193) {
    std::string name = "F9";
    pal::KeyCode expected_code = pal::KeyCode::F9;
    int expected_scancode = 66;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_194) {
    std::string name = "F10";
    pal::KeyCode expected_code = pal::KeyCode::F10;
    int expected_scancode = 67;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_195) {
    std::string name = "F11";
    pal::KeyCode expected_code = pal::KeyCode::F11;
    int expected_scancode = 68;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_196) {
    std::string name = "F12";
    pal::KeyCode expected_code = pal::KeyCode::F12;
    int expected_scancode = 69;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_197) {
    std::string name = "PrintScreen";
    pal::KeyCode expected_code = pal::KeyCode::PrintScreen;
    int expected_scancode = 70;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_198) {
    std::string name = "ScrollLock";
    pal::KeyCode expected_code = pal::KeyCode::ScrollLock;
    int expected_scancode = 71;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}
TEST(InputStressTest, KeyMappings_199) {
    std::string name = "Pause";
    pal::KeyCode expected_code = pal::KeyCode::Pause;
    int expected_scancode = 72;
    
    EXPECT_EQ(InputSystem::KeyCodeFromString(name), expected_code);
    EXPECT_EQ(InputSystem::StringFromKeyCode(expected_code), name);
    EXPECT_EQ(InputSystem::KeyCodeToScancodeValue(expected_code), expected_scancode);
}

// --- 5. A* PATHFINDING STRESS TESTS (200 tests) ---
TEST(NavigationStressTest, PathfindingBlocked_0) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 0, 0 });
    glm::vec3 end = nav.GridToWorld({ 17, 0 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 0);
    EXPECT_EQ(startGrid.z, 0);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_1) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 1, 3 });
    glm::vec3 end = nav.GridToWorld({ 6, 7 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 1);
    EXPECT_EQ(startGrid.z, 3);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_2) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 2, 6 });
    glm::vec3 end = nav.GridToWorld({ 7, 14 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 2);
    EXPECT_EQ(startGrid.z, 6);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_3) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 3, 9 });
    glm::vec3 end = nav.GridToWorld({ 8, 21 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 3);
    EXPECT_EQ(startGrid.z, 9);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_4) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 4, 12 });
    glm::vec3 end = nav.GridToWorld({ 21, 28 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 4);
    EXPECT_EQ(startGrid.z, 12);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_5) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 5, 15 });
    glm::vec3 end = nav.GridToWorld({ 10, 3 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 5);
    EXPECT_EQ(startGrid.z, 15);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_6) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 6, 18 });
    glm::vec3 end = nav.GridToWorld({ 11, 10 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 6);
    EXPECT_EQ(startGrid.z, 18);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_7) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 7, 21 });
    glm::vec3 end = nav.GridToWorld({ 12, 17 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 7);
    EXPECT_EQ(startGrid.z, 21);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_8) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 8, 24 });
    glm::vec3 end = nav.GridToWorld({ 25, 24 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 8);
    EXPECT_EQ(startGrid.z, 24);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_9) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 9, 27 });
    glm::vec3 end = nav.GridToWorld({ 14, 31 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 9);
    EXPECT_EQ(startGrid.z, 27);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_10) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 10, 30 });
    glm::vec3 end = nav.GridToWorld({ 15, 6 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 10);
    EXPECT_EQ(startGrid.z, 30);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_11) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 11, 1 });
    glm::vec3 end = nav.GridToWorld({ 16, 13 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 11);
    EXPECT_EQ(startGrid.z, 1);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_12) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 12, 4 });
    glm::vec3 end = nav.GridToWorld({ 29, 20 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 12);
    EXPECT_EQ(startGrid.z, 4);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_13) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 13, 7 });
    glm::vec3 end = nav.GridToWorld({ 18, 27 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 13);
    EXPECT_EQ(startGrid.z, 7);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_14) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 14, 10 });
    glm::vec3 end = nav.GridToWorld({ 19, 2 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 14);
    EXPECT_EQ(startGrid.z, 10);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_15) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 15, 13 });
    glm::vec3 end = nav.GridToWorld({ 20, 9 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 15);
    EXPECT_EQ(startGrid.z, 13);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_16) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 1, 16 });
    glm::vec3 end = nav.GridToWorld({ 18, 16 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 1);
    EXPECT_EQ(startGrid.z, 16);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_17) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 17, 19 });
    glm::vec3 end = nav.GridToWorld({ 22, 23 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 17);
    EXPECT_EQ(startGrid.z, 19);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_18) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 18, 22 });
    glm::vec3 end = nav.GridToWorld({ 23, 30 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 18);
    EXPECT_EQ(startGrid.z, 22);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_19) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 19, 25 });
    glm::vec3 end = nav.GridToWorld({ 24, 5 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 19);
    EXPECT_EQ(startGrid.z, 25);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_20) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 5, 28 });
    glm::vec3 end = nav.GridToWorld({ 22, 12 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 5);
    EXPECT_EQ(startGrid.z, 28);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_21) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 21, 31 });
    glm::vec3 end = nav.GridToWorld({ 26, 19 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 21);
    EXPECT_EQ(startGrid.z, 31);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_22) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 22, 2 });
    glm::vec3 end = nav.GridToWorld({ 27, 26 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 22);
    EXPECT_EQ(startGrid.z, 2);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_23) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 23, 5 });
    glm::vec3 end = nav.GridToWorld({ 28, 1 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 23);
    EXPECT_EQ(startGrid.z, 5);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_24) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 9, 8 });
    glm::vec3 end = nav.GridToWorld({ 26, 8 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 9);
    EXPECT_EQ(startGrid.z, 8);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_25) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 25, 11 });
    glm::vec3 end = nav.GridToWorld({ 30, 15 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 25);
    EXPECT_EQ(startGrid.z, 11);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_26) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 26, 14 });
    glm::vec3 end = nav.GridToWorld({ 31, 22 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 26);
    EXPECT_EQ(startGrid.z, 14);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_27) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 27, 17 });
    glm::vec3 end = nav.GridToWorld({ 0, 29 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 27);
    EXPECT_EQ(startGrid.z, 17);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_28) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 13, 20 });
    glm::vec3 end = nav.GridToWorld({ 30, 4 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 13);
    EXPECT_EQ(startGrid.z, 20);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_29) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 29, 23 });
    glm::vec3 end = nav.GridToWorld({ 2, 11 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 29);
    EXPECT_EQ(startGrid.z, 23);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_30) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 30, 26 });
    glm::vec3 end = nav.GridToWorld({ 3, 18 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 30);
    EXPECT_EQ(startGrid.z, 26);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_31) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 31, 29 });
    glm::vec3 end = nav.GridToWorld({ 4, 25 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 31);
    EXPECT_EQ(startGrid.z, 29);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_32) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 2, 0 });
    glm::vec3 end = nav.GridToWorld({ 19, 0 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 2);
    EXPECT_EQ(startGrid.z, 0);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_33) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 1, 3 });
    glm::vec3 end = nav.GridToWorld({ 6, 7 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 1);
    EXPECT_EQ(startGrid.z, 3);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_34) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 2, 6 });
    glm::vec3 end = nav.GridToWorld({ 7, 14 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 2);
    EXPECT_EQ(startGrid.z, 6);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_35) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 3, 9 });
    glm::vec3 end = nav.GridToWorld({ 8, 21 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 3);
    EXPECT_EQ(startGrid.z, 9);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_36) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 6, 12 });
    glm::vec3 end = nav.GridToWorld({ 23, 28 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 6);
    EXPECT_EQ(startGrid.z, 12);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_37) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 5, 15 });
    glm::vec3 end = nav.GridToWorld({ 10, 3 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 5);
    EXPECT_EQ(startGrid.z, 15);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_38) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 6, 18 });
    glm::vec3 end = nav.GridToWorld({ 11, 10 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 6);
    EXPECT_EQ(startGrid.z, 18);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_39) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 7, 21 });
    glm::vec3 end = nav.GridToWorld({ 12, 17 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 7);
    EXPECT_EQ(startGrid.z, 21);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_40) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 10, 24 });
    glm::vec3 end = nav.GridToWorld({ 27, 24 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 10);
    EXPECT_EQ(startGrid.z, 24);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_41) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 9, 27 });
    glm::vec3 end = nav.GridToWorld({ 14, 31 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 9);
    EXPECT_EQ(startGrid.z, 27);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_42) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 10, 30 });
    glm::vec3 end = nav.GridToWorld({ 15, 6 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 10);
    EXPECT_EQ(startGrid.z, 30);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_43) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 11, 1 });
    glm::vec3 end = nav.GridToWorld({ 16, 13 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 11);
    EXPECT_EQ(startGrid.z, 1);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_44) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 14, 4 });
    glm::vec3 end = nav.GridToWorld({ 31, 20 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 14);
    EXPECT_EQ(startGrid.z, 4);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_45) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 13, 7 });
    glm::vec3 end = nav.GridToWorld({ 18, 27 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 13);
    EXPECT_EQ(startGrid.z, 7);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_46) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 14, 10 });
    glm::vec3 end = nav.GridToWorld({ 19, 2 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 14);
    EXPECT_EQ(startGrid.z, 10);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_47) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 15, 13 });
    glm::vec3 end = nav.GridToWorld({ 20, 9 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 15);
    EXPECT_EQ(startGrid.z, 13);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_48) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 3, 16 });
    glm::vec3 end = nav.GridToWorld({ 20, 16 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 3);
    EXPECT_EQ(startGrid.z, 16);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_49) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 17, 19 });
    glm::vec3 end = nav.GridToWorld({ 22, 23 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 17);
    EXPECT_EQ(startGrid.z, 19);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_50) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 18, 22 });
    glm::vec3 end = nav.GridToWorld({ 23, 30 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 18);
    EXPECT_EQ(startGrid.z, 22);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_51) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 19, 25 });
    glm::vec3 end = nav.GridToWorld({ 24, 5 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 19);
    EXPECT_EQ(startGrid.z, 25);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_52) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 7, 28 });
    glm::vec3 end = nav.GridToWorld({ 24, 12 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 7);
    EXPECT_EQ(startGrid.z, 28);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_53) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 21, 31 });
    glm::vec3 end = nav.GridToWorld({ 26, 19 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 21);
    EXPECT_EQ(startGrid.z, 31);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_54) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 22, 2 });
    glm::vec3 end = nav.GridToWorld({ 27, 26 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 22);
    EXPECT_EQ(startGrid.z, 2);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_55) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 23, 5 });
    glm::vec3 end = nav.GridToWorld({ 28, 1 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 23);
    EXPECT_EQ(startGrid.z, 5);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_56) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 11, 8 });
    glm::vec3 end = nav.GridToWorld({ 28, 8 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 11);
    EXPECT_EQ(startGrid.z, 8);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_57) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 25, 11 });
    glm::vec3 end = nav.GridToWorld({ 30, 15 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 25);
    EXPECT_EQ(startGrid.z, 11);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_58) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 26, 14 });
    glm::vec3 end = nav.GridToWorld({ 31, 22 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 26);
    EXPECT_EQ(startGrid.z, 14);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_59) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 27, 17 });
    glm::vec3 end = nav.GridToWorld({ 0, 29 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 27);
    EXPECT_EQ(startGrid.z, 17);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_60) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 0, 20 });
    glm::vec3 end = nav.GridToWorld({ 17, 4 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 0);
    EXPECT_EQ(startGrid.z, 20);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_61) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 29, 23 });
    glm::vec3 end = nav.GridToWorld({ 2, 11 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 29);
    EXPECT_EQ(startGrid.z, 23);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_62) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 30, 26 });
    glm::vec3 end = nav.GridToWorld({ 3, 18 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 30);
    EXPECT_EQ(startGrid.z, 26);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_63) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 31, 29 });
    glm::vec3 end = nav.GridToWorld({ 4, 25 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 31);
    EXPECT_EQ(startGrid.z, 29);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_64) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 4, 0 });
    glm::vec3 end = nav.GridToWorld({ 21, 0 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 4);
    EXPECT_EQ(startGrid.z, 0);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_65) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 1, 3 });
    glm::vec3 end = nav.GridToWorld({ 6, 7 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 1);
    EXPECT_EQ(startGrid.z, 3);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_66) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 2, 6 });
    glm::vec3 end = nav.GridToWorld({ 7, 14 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 2);
    EXPECT_EQ(startGrid.z, 6);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_67) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 3, 9 });
    glm::vec3 end = nav.GridToWorld({ 8, 21 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 3);
    EXPECT_EQ(startGrid.z, 9);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_68) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 8, 12 });
    glm::vec3 end = nav.GridToWorld({ 25, 28 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 8);
    EXPECT_EQ(startGrid.z, 12);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_69) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 5, 15 });
    glm::vec3 end = nav.GridToWorld({ 10, 3 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 5);
    EXPECT_EQ(startGrid.z, 15);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_70) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 6, 18 });
    glm::vec3 end = nav.GridToWorld({ 11, 10 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 6);
    EXPECT_EQ(startGrid.z, 18);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_71) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 7, 21 });
    glm::vec3 end = nav.GridToWorld({ 12, 17 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 7);
    EXPECT_EQ(startGrid.z, 21);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_72) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 12, 24 });
    glm::vec3 end = nav.GridToWorld({ 29, 24 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 12);
    EXPECT_EQ(startGrid.z, 24);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_73) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 9, 27 });
    glm::vec3 end = nav.GridToWorld({ 14, 31 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 9);
    EXPECT_EQ(startGrid.z, 27);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_74) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 10, 30 });
    glm::vec3 end = nav.GridToWorld({ 15, 6 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 10);
    EXPECT_EQ(startGrid.z, 30);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_75) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 11, 1 });
    glm::vec3 end = nav.GridToWorld({ 16, 13 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 11);
    EXPECT_EQ(startGrid.z, 1);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_76) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 1, 4 });
    glm::vec3 end = nav.GridToWorld({ 18, 20 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 1);
    EXPECT_EQ(startGrid.z, 4);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_77) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 13, 7 });
    glm::vec3 end = nav.GridToWorld({ 18, 27 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 13);
    EXPECT_EQ(startGrid.z, 7);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_78) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 14, 10 });
    glm::vec3 end = nav.GridToWorld({ 19, 2 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 14);
    EXPECT_EQ(startGrid.z, 10);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_79) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 15, 13 });
    glm::vec3 end = nav.GridToWorld({ 20, 9 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 15);
    EXPECT_EQ(startGrid.z, 13);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_80) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 5, 16 });
    glm::vec3 end = nav.GridToWorld({ 22, 16 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 5);
    EXPECT_EQ(startGrid.z, 16);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_81) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 17, 19 });
    glm::vec3 end = nav.GridToWorld({ 22, 23 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 17);
    EXPECT_EQ(startGrid.z, 19);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_82) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 18, 22 });
    glm::vec3 end = nav.GridToWorld({ 23, 30 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 18);
    EXPECT_EQ(startGrid.z, 22);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_83) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 19, 25 });
    glm::vec3 end = nav.GridToWorld({ 24, 5 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 19);
    EXPECT_EQ(startGrid.z, 25);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_84) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 9, 28 });
    glm::vec3 end = nav.GridToWorld({ 26, 12 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 9);
    EXPECT_EQ(startGrid.z, 28);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_85) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 21, 31 });
    glm::vec3 end = nav.GridToWorld({ 26, 19 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 21);
    EXPECT_EQ(startGrid.z, 31);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_86) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 22, 2 });
    glm::vec3 end = nav.GridToWorld({ 27, 26 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 22);
    EXPECT_EQ(startGrid.z, 2);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_87) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 23, 5 });
    glm::vec3 end = nav.GridToWorld({ 28, 1 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 23);
    EXPECT_EQ(startGrid.z, 5);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_88) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 13, 8 });
    glm::vec3 end = nav.GridToWorld({ 30, 8 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 13);
    EXPECT_EQ(startGrid.z, 8);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_89) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 25, 11 });
    glm::vec3 end = nav.GridToWorld({ 30, 15 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 25);
    EXPECT_EQ(startGrid.z, 11);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_90) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 26, 14 });
    glm::vec3 end = nav.GridToWorld({ 31, 22 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 26);
    EXPECT_EQ(startGrid.z, 14);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_91) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 27, 17 });
    glm::vec3 end = nav.GridToWorld({ 0, 29 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 27);
    EXPECT_EQ(startGrid.z, 17);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_92) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 2, 20 });
    glm::vec3 end = nav.GridToWorld({ 19, 4 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 2);
    EXPECT_EQ(startGrid.z, 20);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_93) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 29, 23 });
    glm::vec3 end = nav.GridToWorld({ 2, 11 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 29);
    EXPECT_EQ(startGrid.z, 23);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_94) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 30, 26 });
    glm::vec3 end = nav.GridToWorld({ 3, 18 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 30);
    EXPECT_EQ(startGrid.z, 26);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_95) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 31, 29 });
    glm::vec3 end = nav.GridToWorld({ 4, 25 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 31);
    EXPECT_EQ(startGrid.z, 29);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_96) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 6, 0 });
    glm::vec3 end = nav.GridToWorld({ 23, 0 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 6);
    EXPECT_EQ(startGrid.z, 0);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_97) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 1, 3 });
    glm::vec3 end = nav.GridToWorld({ 6, 7 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 1);
    EXPECT_EQ(startGrid.z, 3);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_98) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 2, 6 });
    glm::vec3 end = nav.GridToWorld({ 7, 14 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 2);
    EXPECT_EQ(startGrid.z, 6);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_99) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 3, 9 });
    glm::vec3 end = nav.GridToWorld({ 8, 21 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 3);
    EXPECT_EQ(startGrid.z, 9);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_100) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 10, 12 });
    glm::vec3 end = nav.GridToWorld({ 27, 28 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 10);
    EXPECT_EQ(startGrid.z, 12);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_101) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 5, 15 });
    glm::vec3 end = nav.GridToWorld({ 10, 3 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 5);
    EXPECT_EQ(startGrid.z, 15);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_102) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 6, 18 });
    glm::vec3 end = nav.GridToWorld({ 11, 10 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 6);
    EXPECT_EQ(startGrid.z, 18);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_103) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 7, 21 });
    glm::vec3 end = nav.GridToWorld({ 12, 17 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 7);
    EXPECT_EQ(startGrid.z, 21);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_104) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 14, 24 });
    glm::vec3 end = nav.GridToWorld({ 31, 24 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 14);
    EXPECT_EQ(startGrid.z, 24);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_105) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 9, 27 });
    glm::vec3 end = nav.GridToWorld({ 14, 31 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 9);
    EXPECT_EQ(startGrid.z, 27);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_106) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 10, 30 });
    glm::vec3 end = nav.GridToWorld({ 15, 6 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 10);
    EXPECT_EQ(startGrid.z, 30);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_107) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 11, 1 });
    glm::vec3 end = nav.GridToWorld({ 16, 13 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 11);
    EXPECT_EQ(startGrid.z, 1);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_108) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 3, 4 });
    glm::vec3 end = nav.GridToWorld({ 20, 20 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 3);
    EXPECT_EQ(startGrid.z, 4);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_109) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 13, 7 });
    glm::vec3 end = nav.GridToWorld({ 18, 27 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 13);
    EXPECT_EQ(startGrid.z, 7);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_110) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 14, 10 });
    glm::vec3 end = nav.GridToWorld({ 19, 2 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 14);
    EXPECT_EQ(startGrid.z, 10);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_111) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 15, 13 });
    glm::vec3 end = nav.GridToWorld({ 20, 9 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 15);
    EXPECT_EQ(startGrid.z, 13);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_112) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 7, 16 });
    glm::vec3 end = nav.GridToWorld({ 24, 16 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 7);
    EXPECT_EQ(startGrid.z, 16);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_113) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 17, 19 });
    glm::vec3 end = nav.GridToWorld({ 22, 23 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 17);
    EXPECT_EQ(startGrid.z, 19);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_114) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 18, 22 });
    glm::vec3 end = nav.GridToWorld({ 23, 30 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 18);
    EXPECT_EQ(startGrid.z, 22);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_115) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 19, 25 });
    glm::vec3 end = nav.GridToWorld({ 24, 5 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 19);
    EXPECT_EQ(startGrid.z, 25);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_116) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 11, 28 });
    glm::vec3 end = nav.GridToWorld({ 28, 12 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 11);
    EXPECT_EQ(startGrid.z, 28);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_117) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 21, 31 });
    glm::vec3 end = nav.GridToWorld({ 26, 19 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 21);
    EXPECT_EQ(startGrid.z, 31);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_118) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 22, 2 });
    glm::vec3 end = nav.GridToWorld({ 27, 26 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 22);
    EXPECT_EQ(startGrid.z, 2);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_119) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 23, 5 });
    glm::vec3 end = nav.GridToWorld({ 28, 1 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 23);
    EXPECT_EQ(startGrid.z, 5);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_120) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 0, 8 });
    glm::vec3 end = nav.GridToWorld({ 17, 8 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 0);
    EXPECT_EQ(startGrid.z, 8);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_121) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 25, 11 });
    glm::vec3 end = nav.GridToWorld({ 30, 15 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 25);
    EXPECT_EQ(startGrid.z, 11);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_122) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 26, 14 });
    glm::vec3 end = nav.GridToWorld({ 31, 22 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 26);
    EXPECT_EQ(startGrid.z, 14);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_123) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 27, 17 });
    glm::vec3 end = nav.GridToWorld({ 0, 29 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 27);
    EXPECT_EQ(startGrid.z, 17);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_124) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 4, 20 });
    glm::vec3 end = nav.GridToWorld({ 21, 4 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 4);
    EXPECT_EQ(startGrid.z, 20);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_125) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 29, 23 });
    glm::vec3 end = nav.GridToWorld({ 2, 11 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 29);
    EXPECT_EQ(startGrid.z, 23);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_126) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 30, 26 });
    glm::vec3 end = nav.GridToWorld({ 3, 18 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 30);
    EXPECT_EQ(startGrid.z, 26);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_127) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 31, 29 });
    glm::vec3 end = nav.GridToWorld({ 4, 25 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 31);
    EXPECT_EQ(startGrid.z, 29);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_128) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 8, 0 });
    glm::vec3 end = nav.GridToWorld({ 25, 0 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 8);
    EXPECT_EQ(startGrid.z, 0);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_129) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 1, 3 });
    glm::vec3 end = nav.GridToWorld({ 6, 7 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 1);
    EXPECT_EQ(startGrid.z, 3);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_130) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 2, 6 });
    glm::vec3 end = nav.GridToWorld({ 7, 14 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 2);
    EXPECT_EQ(startGrid.z, 6);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_131) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 3, 9 });
    glm::vec3 end = nav.GridToWorld({ 8, 21 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 3);
    EXPECT_EQ(startGrid.z, 9);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_132) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 12, 12 });
    glm::vec3 end = nav.GridToWorld({ 29, 28 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 12);
    EXPECT_EQ(startGrid.z, 12);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_133) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 5, 15 });
    glm::vec3 end = nav.GridToWorld({ 10, 3 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 5);
    EXPECT_EQ(startGrid.z, 15);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_134) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 6, 18 });
    glm::vec3 end = nav.GridToWorld({ 11, 10 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 6);
    EXPECT_EQ(startGrid.z, 18);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_135) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 7, 21 });
    glm::vec3 end = nav.GridToWorld({ 12, 17 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 7);
    EXPECT_EQ(startGrid.z, 21);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_136) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 1, 24 });
    glm::vec3 end = nav.GridToWorld({ 18, 24 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 1);
    EXPECT_EQ(startGrid.z, 24);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_137) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 9, 27 });
    glm::vec3 end = nav.GridToWorld({ 14, 31 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 9);
    EXPECT_EQ(startGrid.z, 27);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_138) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 10, 30 });
    glm::vec3 end = nav.GridToWorld({ 15, 6 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 10);
    EXPECT_EQ(startGrid.z, 30);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_139) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 11, 1 });
    glm::vec3 end = nav.GridToWorld({ 16, 13 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 11);
    EXPECT_EQ(startGrid.z, 1);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_140) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 5, 4 });
    glm::vec3 end = nav.GridToWorld({ 22, 20 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 5);
    EXPECT_EQ(startGrid.z, 4);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_141) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 13, 7 });
    glm::vec3 end = nav.GridToWorld({ 18, 27 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 13);
    EXPECT_EQ(startGrid.z, 7);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_142) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 14, 10 });
    glm::vec3 end = nav.GridToWorld({ 19, 2 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 14);
    EXPECT_EQ(startGrid.z, 10);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_143) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 15, 13 });
    glm::vec3 end = nav.GridToWorld({ 20, 9 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 15);
    EXPECT_EQ(startGrid.z, 13);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_144) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 9, 16 });
    glm::vec3 end = nav.GridToWorld({ 26, 16 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 9);
    EXPECT_EQ(startGrid.z, 16);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_145) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 17, 19 });
    glm::vec3 end = nav.GridToWorld({ 22, 23 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 17);
    EXPECT_EQ(startGrid.z, 19);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_146) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 18, 22 });
    glm::vec3 end = nav.GridToWorld({ 23, 30 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 18);
    EXPECT_EQ(startGrid.z, 22);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_147) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 19, 25 });
    glm::vec3 end = nav.GridToWorld({ 24, 5 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 19);
    EXPECT_EQ(startGrid.z, 25);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_148) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 13, 28 });
    glm::vec3 end = nav.GridToWorld({ 30, 12 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 13);
    EXPECT_EQ(startGrid.z, 28);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_149) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 21, 31 });
    glm::vec3 end = nav.GridToWorld({ 26, 19 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 21);
    EXPECT_EQ(startGrid.z, 31);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_150) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 22, 2 });
    glm::vec3 end = nav.GridToWorld({ 27, 26 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 22);
    EXPECT_EQ(startGrid.z, 2);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_151) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 23, 5 });
    glm::vec3 end = nav.GridToWorld({ 28, 1 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 23);
    EXPECT_EQ(startGrid.z, 5);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_152) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 2, 8 });
    glm::vec3 end = nav.GridToWorld({ 19, 8 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 2);
    EXPECT_EQ(startGrid.z, 8);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_153) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 25, 11 });
    glm::vec3 end = nav.GridToWorld({ 30, 15 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 25);
    EXPECT_EQ(startGrid.z, 11);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_154) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 26, 14 });
    glm::vec3 end = nav.GridToWorld({ 31, 22 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 26);
    EXPECT_EQ(startGrid.z, 14);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_155) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 27, 17 });
    glm::vec3 end = nav.GridToWorld({ 0, 29 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 27);
    EXPECT_EQ(startGrid.z, 17);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_156) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 6, 20 });
    glm::vec3 end = nav.GridToWorld({ 23, 4 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 6);
    EXPECT_EQ(startGrid.z, 20);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_157) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 29, 23 });
    glm::vec3 end = nav.GridToWorld({ 2, 11 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 29);
    EXPECT_EQ(startGrid.z, 23);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_158) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 30, 26 });
    glm::vec3 end = nav.GridToWorld({ 3, 18 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 30);
    EXPECT_EQ(startGrid.z, 26);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_159) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 31, 29 });
    glm::vec3 end = nav.GridToWorld({ 4, 25 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 31);
    EXPECT_EQ(startGrid.z, 29);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_160) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 10, 0 });
    glm::vec3 end = nav.GridToWorld({ 27, 0 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 10);
    EXPECT_EQ(startGrid.z, 0);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_161) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 1, 3 });
    glm::vec3 end = nav.GridToWorld({ 6, 7 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 1);
    EXPECT_EQ(startGrid.z, 3);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_162) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 2, 6 });
    glm::vec3 end = nav.GridToWorld({ 7, 14 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 2);
    EXPECT_EQ(startGrid.z, 6);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_163) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 3, 9 });
    glm::vec3 end = nav.GridToWorld({ 8, 21 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 3);
    EXPECT_EQ(startGrid.z, 9);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_164) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 14, 12 });
    glm::vec3 end = nav.GridToWorld({ 31, 28 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 14);
    EXPECT_EQ(startGrid.z, 12);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_165) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 5, 15 });
    glm::vec3 end = nav.GridToWorld({ 10, 3 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 5);
    EXPECT_EQ(startGrid.z, 15);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_166) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 6, 18 });
    glm::vec3 end = nav.GridToWorld({ 11, 10 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 6);
    EXPECT_EQ(startGrid.z, 18);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_167) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 7, 21 });
    glm::vec3 end = nav.GridToWorld({ 12, 17 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 7);
    EXPECT_EQ(startGrid.z, 21);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_168) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 3, 24 });
    glm::vec3 end = nav.GridToWorld({ 20, 24 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 3);
    EXPECT_EQ(startGrid.z, 24);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_169) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 9, 27 });
    glm::vec3 end = nav.GridToWorld({ 14, 31 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 9);
    EXPECT_EQ(startGrid.z, 27);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_170) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 10, 30 });
    glm::vec3 end = nav.GridToWorld({ 15, 6 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 10);
    EXPECT_EQ(startGrid.z, 30);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_171) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 11, 1 });
    glm::vec3 end = nav.GridToWorld({ 16, 13 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 11);
    EXPECT_EQ(startGrid.z, 1);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_172) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 7, 4 });
    glm::vec3 end = nav.GridToWorld({ 24, 20 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 7);
    EXPECT_EQ(startGrid.z, 4);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_173) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 13, 7 });
    glm::vec3 end = nav.GridToWorld({ 18, 27 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 13);
    EXPECT_EQ(startGrid.z, 7);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_174) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 14, 10 });
    glm::vec3 end = nav.GridToWorld({ 19, 2 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 14);
    EXPECT_EQ(startGrid.z, 10);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_175) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 15, 13 });
    glm::vec3 end = nav.GridToWorld({ 20, 9 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 15);
    EXPECT_EQ(startGrid.z, 13);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_176) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 11, 16 });
    glm::vec3 end = nav.GridToWorld({ 28, 16 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 11);
    EXPECT_EQ(startGrid.z, 16);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_177) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 17, 19 });
    glm::vec3 end = nav.GridToWorld({ 22, 23 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 17);
    EXPECT_EQ(startGrid.z, 19);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_178) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 18, 22 });
    glm::vec3 end = nav.GridToWorld({ 23, 30 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 18);
    EXPECT_EQ(startGrid.z, 22);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_179) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 19, 25 });
    glm::vec3 end = nav.GridToWorld({ 24, 5 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 19);
    EXPECT_EQ(startGrid.z, 25);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_180) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 0, 28 });
    glm::vec3 end = nav.GridToWorld({ 17, 12 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 0);
    EXPECT_EQ(startGrid.z, 28);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_181) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 21, 31 });
    glm::vec3 end = nav.GridToWorld({ 26, 19 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 21);
    EXPECT_EQ(startGrid.z, 31);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_182) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 22, 2 });
    glm::vec3 end = nav.GridToWorld({ 27, 26 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 22);
    EXPECT_EQ(startGrid.z, 2);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_183) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 23, 5 });
    glm::vec3 end = nav.GridToWorld({ 28, 1 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 23);
    EXPECT_EQ(startGrid.z, 5);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_184) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 4, 8 });
    glm::vec3 end = nav.GridToWorld({ 21, 8 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 4);
    EXPECT_EQ(startGrid.z, 8);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_185) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 25, 11 });
    glm::vec3 end = nav.GridToWorld({ 30, 15 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 25);
    EXPECT_EQ(startGrid.z, 11);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_186) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 26, 14 });
    glm::vec3 end = nav.GridToWorld({ 31, 22 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 26);
    EXPECT_EQ(startGrid.z, 14);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_187) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 27, 17 });
    glm::vec3 end = nav.GridToWorld({ 0, 29 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 27);
    EXPECT_EQ(startGrid.z, 17);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_188) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 8, 20 });
    glm::vec3 end = nav.GridToWorld({ 25, 4 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 8);
    EXPECT_EQ(startGrid.z, 20);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_189) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 29, 23 });
    glm::vec3 end = nav.GridToWorld({ 2, 11 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 29);
    EXPECT_EQ(startGrid.z, 23);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_190) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 30, 26 });
    glm::vec3 end = nav.GridToWorld({ 3, 18 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 30);
    EXPECT_EQ(startGrid.z, 26);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_191) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 31, 29 });
    glm::vec3 end = nav.GridToWorld({ 4, 25 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 31);
    EXPECT_EQ(startGrid.z, 29);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_192) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 12, 0 });
    glm::vec3 end = nav.GridToWorld({ 29, 0 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 12);
    EXPECT_EQ(startGrid.z, 0);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_193) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 1, 3 });
    glm::vec3 end = nav.GridToWorld({ 6, 7 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 1);
    EXPECT_EQ(startGrid.z, 3);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_194) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 2, 6 });
    glm::vec3 end = nav.GridToWorld({ 7, 14 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 2);
    EXPECT_EQ(startGrid.z, 6);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_195) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 3, 9 });
    glm::vec3 end = nav.GridToWorld({ 8, 21 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 3);
    EXPECT_EQ(startGrid.z, 9);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, PathfindingBlocked_196) {
    NavigationSystem nav(32, 64.0f);
    
    // Create an impassable wall at x = 16
    for (int z = 0; z < 32; ++z) {
        nav.SetObstacle(16, z, true);
    }
    
    glm::vec3 start = nav.GridToWorld({ 1, 12 });
    glm::vec3 end = nav.GridToWorld({ 18, 28 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 1);
    EXPECT_EQ(startGrid.z, 12);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_FALSE(found);
    EXPECT_TRUE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_197) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 5, 15 });
    glm::vec3 end = nav.GridToWorld({ 10, 3 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 5);
    EXPECT_EQ(startGrid.z, 15);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_198) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 6, 18 });
    glm::vec3 end = nav.GridToWorld({ 11, 10 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 6);
    EXPECT_EQ(startGrid.z, 18);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
TEST(NavigationStressTest, Pathfinding_199) {
    NavigationSystem nav(32, 64.0f);
    
    glm::vec3 start = nav.GridToWorld({ 7, 21 });
    glm::vec3 end = nav.GridToWorld({ 12, 17 });
    
    NavPos startGrid = nav.WorldToGrid(start);
    EXPECT_EQ(startGrid.x, 7);
    EXPECT_EQ(startGrid.z, 21);
    
    std::vector<glm::vec3> path;
    bool found = nav.FindPath(start, end, path);
    EXPECT_TRUE(found);
    EXPECT_FALSE(path.empty());
}
