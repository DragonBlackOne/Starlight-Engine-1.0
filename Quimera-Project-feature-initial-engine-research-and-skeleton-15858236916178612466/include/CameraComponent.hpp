#pragma once
#include <glm/glm.hpp>

namespace Vamos {
    struct CameraComponent {
        float fov;
        float aspectRatio;
        float nearPlane;
        float farPlane;

        CameraComponent(float fov = 45.0f, float aspect = 1.33f, float near = 0.1f, float far = 100.0f)
            : fov(fov), aspectRatio(aspect), nearPlane(near), farPlane(far) {}
    };
}
