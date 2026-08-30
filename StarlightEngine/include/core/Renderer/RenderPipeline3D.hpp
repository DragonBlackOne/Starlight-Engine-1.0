#pragma once
#include "RendererCommon.hpp"
#include "PBRMaterial.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

namespace starlight::renderer {

enum class ToneMappingCurve {
    ACES,
    Reinhard,
    Filmic,
    Uncharted2
};

struct ShadowPassConfig {
    bool enabled = true;
    uint32_t resolution = 2048;
    uint32_t cascadeCount = 4;
    float splitLambda = 0.92f;
    float depthBias = 0.005f;
    float normalBias = 0.02f;
};

struct GBufferConfig {
    uint32_t width = 1920;
    uint32_t height = 1080;
    bool enableSSR = true;
    bool enableSSAO = true;
};

struct PostProcessConfig {
    bool enableBloom = true;
    float bloomThreshold = 1.0f;
    float bloomIntensity = 0.8f;

    bool enableToneMapping = true;
    ToneMappingCurve toneCurve = ToneMappingCurve::ACES;
    float exposure = 1.0f;

    bool enableVignette = true;
    float vignetteStrength = 0.25f;

    bool enableFXAA = true;
};

struct Light3D {
    enum class Type { Directional, Point, Spot };

    Type type = Type::Directional;
    glm::vec3 position{ 0.0f };
    glm::vec3 direction{ 0.0f, -1.0f, 0.0f };
    glm::vec3 color{ 1.0f, 1.0f, 1.0f };
    float intensity = 1.0f;
    float radius = 10.0f;
    float innerCutOff = glm::cos(glm::radians(12.5f));
    float outerCutOff = glm::cos(glm::radians(17.5f));
    bool castsShadows = true;
};

class RenderPipeline3D {
public:
    RenderPipeline3D() = default;

    void SetCamera(const CameraRenderContext& camera) {
        m_camera = camera;
    }

    const CameraRenderContext& GetCamera() const { return m_camera; }

    void SetShadowConfig(const ShadowPassConfig& config) { m_shadowConfig = config; }
    const ShadowPassConfig& GetShadowConfig() const { return m_shadowConfig; }

    void SetGBufferConfig(const GBufferConfig& config) { m_gbufferConfig = config; }
    const GBufferConfig& GetGBufferConfig() const { return m_gbufferConfig; }

    void SetPostProcessConfig(const PostProcessConfig& config) { m_postProcessConfig = config; }
    const PostProcessConfig& GetPostProcessConfig() const { return m_postProcessConfig; }

    void AddLight(const Light3D& light) {
        m_lights.push_back(light);
    }

    void ClearLights() {
        m_lights.clear();
    }

    const std::vector<Light3D>& GetLights() const { return m_lights; }
    size_t GetLightCount() const { return m_lights.size(); }

    const RenderStats& GetStats() const { return m_stats; }
    void ResetStats() { m_stats.Reset(); }

private:
    CameraRenderContext m_camera;
    ShadowPassConfig m_shadowConfig;
    GBufferConfig m_gbufferConfig;
    PostProcessConfig m_postProcessConfig;
    std::vector<Light3D> m_lights;
    RenderStats m_stats;
};

} // namespace starlight::renderer
