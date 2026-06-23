#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Test TransformComponent in isolation without including Components.hpp
// (Components.hpp includes glad/glad.h which requires GL context)
struct LocalTransformComponent {
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};

    glm::mat4 localMatrix = glm::mat4(1.0f);
    glm::mat4 worldMatrix = glm::mat4(1.0f);

    void UpdateLocalMatrix() {
        localMatrix = glm::translate(glm::mat4(1.0f), position);
        localMatrix *= glm::mat4_cast(rotation);
        localMatrix = glm::scale(localMatrix, scale);
    }

    glm::mat4 GetMatrix() const {
        return worldMatrix;
    }
};

TEST(TransformComponentTest, DefaultMatrixIsIdentity) {
    LocalTransformComponent tc;
    tc.UpdateLocalMatrix();
    EXPECT_EQ(tc.localMatrix, glm::mat4(1.0f));
}

TEST(TransformComponentTest, Translation) {
    LocalTransformComponent tc;
    tc.position = glm::vec3(10.0f, 20.0f, 30.0f);
    tc.UpdateLocalMatrix();

    glm::vec4 origin = tc.localMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_FLOAT_EQ(origin.x, 10.0f);
    EXPECT_FLOAT_EQ(origin.y, 20.0f);
    EXPECT_FLOAT_EQ(origin.z, 30.0f);
}

TEST(TransformComponentTest, Scale) {
    LocalTransformComponent tc;
    tc.scale = glm::vec3(2.0f, 3.0f, 4.0f);
    tc.UpdateLocalMatrix();

    glm::vec4 xAxis = tc.localMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 yAxis = tc.localMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 zAxis = tc.localMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

    EXPECT_NEAR(glm::length(glm::vec3(xAxis)), 2.0f, 0.001f);
    EXPECT_NEAR(glm::length(glm::vec3(yAxis)), 3.0f, 0.001f);
    EXPECT_NEAR(glm::length(glm::vec3(zAxis)), 4.0f, 0.001f);
}

TEST(TransformComponentTest, Rotation90DegreesAroundY) {
    LocalTransformComponent tc;
    tc.rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    tc.UpdateLocalMatrix();

    glm::vec4 forward = tc.localMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    // GLM right-handed: -Z rotated 90° around Y gives -X
    EXPECT_NEAR(forward.x, -1.0f, 0.001f);
    EXPECT_NEAR(forward.z, 0.0f, 0.001f);
}

TEST(TransformComponentTest, RotationAroundX) {
    LocalTransformComponent tc;
    tc.rotation = glm::angleAxis(glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    tc.UpdateLocalMatrix();

    glm::vec4 up = tc.localMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    EXPECT_NEAR(glm::length(glm::vec3(up)), 1.0f, 0.001f);
    EXPECT_NEAR(up.y, glm::cos(glm::radians(45.0f)), 0.001f);
    EXPECT_NEAR(up.z, glm::sin(glm::radians(45.0f)), 0.001f);
}

TEST(TransformComponentTest, TranslateRotateScale) {
    LocalTransformComponent tc;
    tc.position = glm::vec3(5.0f, 10.0f, 15.0f);
    tc.rotation = glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    tc.scale = glm::vec3(2.0f, 2.0f, 2.0f);
    tc.UpdateLocalMatrix();

    glm::vec4 p = tc.localMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_NEAR(p.x, -2.0f + 5.0f, 0.001f);
    EXPECT_NEAR(p.y, 10.0f, 0.001f);
}

TEST(TransformComponentTest, GetMatrix) {
    LocalTransformComponent tc;
    tc.worldMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1, 2, 3));
    EXPECT_EQ(tc.GetMatrix(), tc.worldMatrix);
}

TEST(TransformComponentTest, LocalAndWorldAreIndependent) {
    LocalTransformComponent tc;
    tc.position = glm::vec3(5.0f, 0.0f, 0.0f);
    tc.worldMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(99, 0, 0));
    tc.UpdateLocalMatrix();
    glm::vec4 localPos = tc.localMatrix * glm::vec4(0, 0, 0, 1);
    glm::vec4 worldPos = tc.worldMatrix * glm::vec4(0, 0, 0, 1);
    EXPECT_NEAR(localPos.x, 5.0f, 0.001f);
    EXPECT_NEAR(worldPos.x, 99.0f, 0.001f);
}

TEST(TransformComponentTest, ZeroScale) {
    LocalTransformComponent tc;
    tc.scale = glm::vec3(0.0f);
    tc.UpdateLocalMatrix();
    glm::vec4 p = tc.localMatrix * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    EXPECT_FLOAT_EQ(p.x, 0.0f);
    EXPECT_FLOAT_EQ(p.y, 0.0f);
    EXPECT_FLOAT_EQ(p.z, 0.0f);
    EXPECT_FLOAT_EQ(p.w, 1.0f);
}

TEST(TransformComponentTest, NegativeScale) {
    LocalTransformComponent tc;
    tc.scale = glm::vec3(-1.0f, 1.0f, 1.0f);
    tc.UpdateLocalMatrix();
    glm::vec4 p = tc.localMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_FLOAT_EQ(p.x, -1.0f);
}
