#pragma once
#include <glm/glm.hpp>
#include <cstdint>

namespace starlight::renderer {

struct RenderStats {
    uint32_t drawCalls = 0;
    uint32_t triangleCount = 0;
    uint32_t quadCount = 0;
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    uint32_t textureBinds = 0;
    uint32_t shaderBinds = 0;
    float gpuFrameTimeMs = 0.0f;

    void Reset() {
        drawCalls = 0;
        triangleCount = 0;
        quadCount = 0;
        vertexCount = 0;
        indexCount = 0;
        textureBinds = 0;
        shaderBinds = 0;
        gpuFrameTimeMs = 0.0f;
    }
};

struct CameraRenderContext {
    glm::mat4 viewMatrix{ 1.0f };
    glm::mat4 projectionMatrix{ 1.0f };
    glm::mat4 viewProjection{ 1.0f };
    glm::vec3 cameraPosition{ 0.0f };
    float fov = 60.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;

    void UpdateViewProjection() {
        viewProjection = projectionMatrix * viewMatrix;
    }
};

} // namespace starlight::renderer
