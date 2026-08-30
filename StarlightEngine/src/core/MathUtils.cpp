#include "MathUtils.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include <limits>

namespace starlight {

bool MathUtils::RayOBBIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                  const glm::vec3& boxCenter, const glm::vec3& boxExtents,
                                  const glm::quat& boxRot, float& outDist) {
    glm::mat4 rotMat = glm::mat4_cast(boxRot);
    glm::vec3 delta = boxCenter - rayOrigin;

    float tMin = 0.0f;
    float tMax = 100000.0f;

    for (int i = 0; i < 3; ++i) {
        glm::vec3 axis = glm::vec3(rotMat[i]);
        float e = glm::dot(axis, delta);
        float f = glm::dot(rayDir, axis);

        if (std::abs(f) > 0.0001f) {
            float t1 = (e + boxExtents[i]) / f;
            float t2 = (e - boxExtents[i]) / f;

            if (t1 > t2) std::swap(t1, t2);
            if (t2 < tMax) tMax = t2;
            if (t1 > tMin) tMin = t1;
            if (tMax < tMin) return false;
        } else {
            if (-e - boxExtents[i] > 0.0f || -e + boxExtents[i] < 0.0f) return false;
        }
    }

    outDist = tMin;
    return true;
}

bool MathUtils::RayPlaneIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                    const glm::vec3& planePoint, const glm::vec3& planeNormal,
                                    float& outDist) {
    float denom = glm::dot(planeNormal, rayDir);
    if (std::abs(denom) > 0.0001f) {
        float t = glm::dot(planePoint - rayOrigin, planeNormal) / denom;
        if (t >= 0.0f) {
            outDist = t;
            return true;
        }
    }
    return false;
}

bool MathUtils::RayTriangleIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                       const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                                       float& outDist, glm::vec2& outUV) {
    constexpr float EPSILON = 0.0000001f;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(rayDir, edge2);
    float a = glm::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON) return false; // Ray is parallel to triangle

    float f = 1.0f / a;
    glm::vec3 s = rayOrigin - v0;
    float u = f * glm::dot(s, h);

    if (u < 0.0f || u > 1.0f) return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(rayDir, q);

    if (v < 0.0f || u + v > 1.0f) return false;

    float t = f * glm::dot(edge2, q);
    if (t > EPSILON) {
        outDist = t;
        outUV = glm::vec2(u, v);
        return true;
    }
    return false;
}

bool MathUtils::RaySphereIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                     const glm::vec3& sphereCenter, float sphereRadius,
                                     float& outDist) {
    glm::vec3 L = sphereCenter - rayOrigin;
    float tca = glm::dot(L, rayDir);
    if (tca < 0.0f) return false;

    float d2 = glm::dot(L, L) - tca * tca;
    float radius2 = sphereRadius * sphereRadius;
    if (d2 > radius2) return false;

    float thc = std::sqrt(radius2 - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;

    if (t0 < 0.0f) {
        t0 = t1;
        if (t0 < 0.0f) return false;
    }

    outDist = t0;
    return true;
}

std::array<FrustumPlane, 6> MathUtils::ExtractFrustumPlanes(const glm::mat4& m) {
    std::array<FrustumPlane, 6> planes;

    // Left
    planes[0].normal = glm::vec3(m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0]);
    planes[0].distance = m[3][3] + m[3][0];

    // Right
    planes[1].normal = glm::vec3(m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0]);
    planes[1].distance = m[3][3] - m[3][0];

    // Bottom
    planes[2].normal = glm::vec3(m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1]);
    planes[2].distance = m[3][3] + m[3][1];

    // Top
    planes[3].normal = glm::vec3(m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1]);
    planes[3].distance = m[3][3] - m[3][1];

    // Near
    planes[4].normal = glm::vec3(m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2]);
    planes[4].distance = m[3][3] + m[3][2];

    // Far
    planes[5].normal = glm::vec3(m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2]);
    planes[5].distance = m[3][3] - m[3][2];

    for (auto& p : planes) {
        float length = glm::length(p.normal);
        if (length > 0.000001f) {
            p.normal /= length;
            p.distance /= length;
        }
    }
    return planes;
}

bool MathUtils::PointInFrustum(const std::array<FrustumPlane, 6>& planes, const glm::vec3& point) {
    for (const auto& plane : planes) {
        if (plane.DistanceToPoint(point) < 0.0f) return false;
    }
    return true;
}

bool MathUtils::SphereInFrustum(const std::array<FrustumPlane, 6>& planes, const glm::vec3& center, float radius) {
    for (const auto& plane : planes) {
        if (plane.DistanceToPoint(center) < -radius) return false;
    }
    return true;
}

bool MathUtils::AABBInFrustum(const std::array<FrustumPlane, 6>& planes, const glm::vec3& minBounds, const glm::vec3& maxBounds) {
    for (const auto& plane : planes) {
        glm::vec3 p = minBounds;
        if (plane.normal.x >= 0.0f) p.x = maxBounds.x;
        if (plane.normal.y >= 0.0f) p.y = maxBounds.y;
        if (plane.normal.z >= 0.0f) p.z = maxBounds.z;

        if (plane.DistanceToPoint(p) < 0.0f) return false;
    }
    return true;
}

glm::quat MathUtils::Slerp(const glm::quat& q1, const glm::quat& q2, float t) {
    return glm::slerp(q1, q2, std::clamp(t, 0.0f, 1.0f));
}

glm::quat MathUtils::Nlerp(const glm::quat& q1, const glm::quat& q2, float t) {
    glm::quat res = q1 * (1.0f - t) + q2 * t;
    return glm::normalize(res);
}

glm::vec3 MathUtils::RGBToHSL(const glm::vec3& rgb) {
    float r = rgb.r, g = rgb.g, b = rgb.b;
    float maxVal = std::max({r, g, b});
    float minVal = std::min({r, g, b});
    float h = 0.0f, s = 0.0f, l = (maxVal + minVal) * 0.5f;

    if (maxVal != minVal) {
        float d = maxVal - minVal;
        s = l > 0.5f ? d / (2.0f - maxVal - minVal) : d / (maxVal + minVal);
        if (maxVal == r) h = (g - b) / d + (g < b ? 6.0f : 0.0f);
        else if (maxVal == g) h = (b - r) / d + 2.0f;
        else h = (r - g) / d + 4.0f;
        h /= 6.0f;
    }
    return glm::vec3(h, s, l);
}

static float Hue2RGB(float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

glm::vec3 MathUtils::HSLToRGB(const glm::vec3& hsl) {
    float h = hsl.x, s = hsl.y, l = hsl.z;
    if (s == 0.0f) return glm::vec3(l);

    float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
    float p = 2.0f * l - q;
    return glm::vec3(Hue2RGB(p, q, h + 1.0f / 3.0f),
                     Hue2RGB(p, q, h),
                     Hue2RGB(p, q, h - 1.0f / 3.0f));
}

glm::vec3 MathUtils::RGBToHSV(const glm::vec3& rgb) {
    float r = rgb.r, g = rgb.g, b = rgb.b;
    float maxVal = std::max({r, g, b});
    float minVal = std::min({r, g, b});
    float h = 0.0f, s = 0.0f, v = maxVal;
    float d = maxVal - minVal;

    s = (maxVal == 0.0f) ? 0.0f : d / maxVal;

    if (maxVal != minVal) {
        if (maxVal == r) h = (g - b) / d + (g < b ? 6.0f : 0.0f);
        else if (maxVal == g) h = (b - r) / d + 2.0f;
        else h = (r - g) / d + 4.0f;
        h /= 6.0f;
    }
    return glm::vec3(h, s, v);
}

glm::vec3 MathUtils::HSVToRGB(const glm::vec3& hsv) {
    float h = hsv.x, s = hsv.y, v = hsv.z;
    int i = static_cast<int>(h * 6.0f);
    float f = h * 6.0f - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);

    switch (i % 6) {
        case 0: return glm::vec3(v, t, p);
        case 1: return glm::vec3(q, v, p);
        case 2: return glm::vec3(p, v, t);
        case 3: return glm::vec3(p, q, v);
        case 4: return glm::vec3(t, p, v);
        case 5: return glm::vec3(v, p, q);
    }
    return glm::vec3(v);
}

float MathUtils::FastInvSqrt(float number) {
    long i;
    float x2, y;
    const float threehalfs = 1.5f;

    x2 = number * 0.5f;
    y  = number;
    i  = * ( long * ) &y;
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );
    return y;
}

float MathUtils::FastSin(float x) {
    constexpr float PI = 3.14159265358979323846f;
    constexpr float TWO_PI = 2.0f * PI;
    x = std::fmod(x + PI, TWO_PI);
    if (x < 0.0f) x += TWO_PI;
    x -= PI;
    return (16.0f * x * (PI - std::abs(x))) / (5.0f * PI * PI - 4.0f * std::abs(x) * (PI - std::abs(x)));
}

float MathUtils::FastCos(float x) {
    return FastSin(x + 1.57079632679f);
}

float MathUtils::SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
    smoothTime = std::max(0.0001f, smoothTime);
    float omega = 2.0f / smoothTime;
    float x = omega * deltaTime;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
    float change = current - target;
    float originalTo = target;

    float maxChange = maxSpeed * smoothTime;
    change = std::clamp(change, -maxChange, maxChange);
    target = current - change;

    float temp = (currentVelocity + omega * change) * deltaTime;
    currentVelocity = (currentVelocity - omega * temp) * exp;
    float output = target + (change + temp) * exp;

    if (originalTo - current > 0.0f == output > originalTo) {
        output = originalTo;
        currentVelocity = (output - originalTo) / deltaTime;
    }
    return output;
}

bool MathUtils::DecomposeTransform(const glm::mat4& transform, glm::vec3& outPos,
                                   glm::quat& outRot, glm::vec3& outScale) {
    glm::vec3 skew;
    glm::vec4 perspective;
    return glm::decompose(transform, outScale, outRot, outPos, skew, perspective);
}

float MathUtils::Approach(float current, float target, float maxDelta) {
    if (current < target) {
        return std::min(current + maxDelta, target);
    }
    return std::max(current - maxDelta, target);
}

} // namespace starlight
