#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>
#include <cmath>
#include <array>

namespace starlight {

struct FrustumPlane {
    glm::vec3 normal = {0.0f, 1.0f, 0.0f};
    float distance = 0.0f;

    float DistanceToPoint(const glm::vec3& point) const {
        return glm::dot(normal, point) + distance;
    }
};

class MathUtils {
public:
    // 1. Ray Intersections
    static bool RayOBBIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                  const glm::vec3& boxCenter, const glm::vec3& boxExtents,
                                  const glm::quat& boxRot, float& outDist);

    static bool RayPlaneIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                    const glm::vec3& planePoint, const glm::vec3& planeNormal,
                                    float& outDist);

    static bool RayTriangleIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                       const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                                       float& outDist, glm::vec2& outUV);

    static bool RaySphereIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                     const glm::vec3& sphereCenter, float sphereRadius,
                                     float& outDist);

    // 2. Frustum Culling Geometry
    static std::array<FrustumPlane, 6> ExtractFrustumPlanes(const glm::mat4& viewProj);
    static bool PointInFrustum(const std::array<FrustumPlane, 6>& planes, const glm::vec3& point);
    static bool SphereInFrustum(const std::array<FrustumPlane, 6>& planes, const glm::vec3& center, float radius);
    static bool AABBInFrustum(const std::array<FrustumPlane, 6>& planes, const glm::vec3& minBounds, const glm::vec3& maxBounds);

    // 3. Quat Slerp & Nlerp
    static glm::quat Slerp(const glm::quat& q1, const glm::quat& q2, float t);
    static glm::quat Nlerp(const glm::quat& q1, const glm::quat& q2, float t);

    // 4. Color Space Converters (RGB <-> HSL <-> HSV)
    static glm::vec3 RGBToHSL(const glm::vec3& rgb);
    static glm::vec3 HSLToRGB(const glm::vec3& hsl);
    static glm::vec3 RGBToHSV(const glm::vec3& rgb);
    static glm::vec3 HSVToRGB(const glm::vec3& hsv);

    // 5. Fast Math & Spring Dampers
    static float FastInvSqrt(float number);
    static float FastSin(float x);
    static float FastCos(float x);
    static float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);

    // 6. Matrix Decomposition
    static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outPos,
                                   glm::quat& outRot, glm::vec3& outScale);

    // Helpers
    static float Clamp(float v, float minV, float maxV) { return std::clamp(v, minV, maxV); }
    static float Lerp(float a, float b, float t) { return a + (b - a) * t; }
    static float Approach(float current, float target, float maxDelta);
};

} // namespace starlight
