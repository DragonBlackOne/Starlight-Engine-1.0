#include <gtest/gtest.h>
#include "core/Renderer/RendererCommon.hpp"
#include "core/Renderer/BatchRenderer2D.hpp"
#include "core/Renderer/PBRMaterial.hpp"
#include "core/Renderer/RenderPipeline3D.hpp"

using namespace starlight::renderer;

// ---------------------------------------------------------------------------
// 1. BatchRenderer2D Tests
// ---------------------------------------------------------------------------
TEST(Module4Renderer2DTest, BatchInitializationAndSubmission) {
    Batch2DConfig cfg;
    cfg.maxQuads = 100;
    cfg.maxTextureSlots = 8;
    BatchRenderer2D renderer(cfg);

    renderer.Begin(glm::mat4(1.0f));
    EXPECT_EQ(renderer.GetQuadCount(), 0u);
    EXPECT_EQ(renderer.GetVertexCount(), 0u);

    // Draw 2 quads
    renderer.DrawQuad(glm::vec2(0.0f, 0.0f), glm::vec2(100.0f, 100.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    renderer.DrawQuad(glm::vec2(200.0f, 200.0f), glm::vec2(50.0f, 50.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    EXPECT_EQ(renderer.GetQuadCount(), 2u);
    EXPECT_EQ(renderer.GetVertexCount(), 8u);
    EXPECT_EQ(renderer.GetIndexCount(), 12u);

    renderer.End(); // Triggers Flush()
    EXPECT_EQ(renderer.GetStats().drawCalls, 1u);
    EXPECT_EQ(renderer.GetStats().quadCount, 2u);
    EXPECT_EQ(renderer.GetQuadCount(), 0u);
}

TEST(Module4Renderer2DTest, TextureSlotCaching) {
    Batch2DConfig cfg;
    cfg.maxQuads = 100;
    cfg.maxTextureSlots = 8;
    BatchRenderer2D renderer(cfg);

    renderer.Begin(glm::mat4(1.0f));

    // Draw with texture 1001 twice
    renderer.DrawQuad(glm::vec3(0.0f), glm::vec2(32.0f), 1001);
    renderer.DrawQuad(glm::vec3(50.0f), glm::vec2(32.0f), 1001);

    // Slot 0 is reserved for white texture, texture 1001 should occupy slot 1
    EXPECT_EQ(renderer.GetTextureSlotCount(), 2u);

    // Draw with new texture 1002
    renderer.DrawQuad(glm::vec3(100.0f), glm::vec2(32.0f), 1002);
    EXPECT_EQ(renderer.GetTextureSlotCount(), 3u);

    renderer.End();
}

// ---------------------------------------------------------------------------
// 2. PBR Material & Cook-Torrance BRDF Tests
// ---------------------------------------------------------------------------
TEST(Module4PBRMaterialTest, PBRMaterialDefaults) {
    PBRMaterial mat;
    EXPECT_FLOAT_EQ(mat.metallic, 0.0f);
    EXPECT_FLOAT_EQ(mat.roughness, 0.5f);
    EXPECT_FLOAT_EQ(mat.ao, 1.0f);
    EXPECT_FALSE(mat.isSkin);
    EXPECT_FLOAT_EQ(mat.normalScale, 1.0f);
}

TEST(Module4PBRMaterialTest, CookTorranceBRDFMath) {
    glm::vec3 N(0.0f, 1.0f, 0.0f);
    glm::vec3 V = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));
    glm::vec3 L = glm::normalize(glm::vec3(0.0f, 1.0f, -1.0f));
    glm::vec3 H = glm::normalize(V + L);

    // Normal Distribution GGX should be positive and non-zero
    float NDF = PBRMaterial::DistributionGGX(N, H, 0.5f);
    EXPECT_GT(NDF, 0.0f);

    // Geometry Smith shadowing
    float G = PBRMaterial::GeometrySmith(N, V, L, 0.5f);
    EXPECT_GT(G, 0.0f);
    EXPECT_LE(G, 1.0f);

    // Fresnel Schlick
    glm::vec3 F0(0.04f); // Dielectric base reflectance
    float cosTheta = std::max(glm::dot(H, V), 0.0f);
    glm::vec3 F = PBRMaterial::FresnelSchlick(cosTheta, F0);
    EXPECT_GE(F.x, 0.04f);
    EXPECT_LE(F.x, 1.0f);
}

// ---------------------------------------------------------------------------
// 3. RenderPipeline3D Tests
// ---------------------------------------------------------------------------
TEST(Module4RenderPipeline3DTest, PipelineConfigurationAndLights) {
    RenderPipeline3D pipeline;

    CameraRenderContext cam;
    cam.fov = 45.0f;
    cam.aspectRatio = 16.0f / 9.0f;
    cam.cameraPosition = glm::vec3(0.0f, 10.0f, 20.0f);
    pipeline.SetCamera(cam);

    EXPECT_FLOAT_EQ(pipeline.GetCamera().fov, 45.0f);
    EXPECT_FLOAT_EQ(pipeline.GetCamera().cameraPosition.y, 10.0f);

    Light3D sun;
    sun.type = Light3D::Type::Directional;
    sun.direction = glm::normalize(glm::vec3(-1.0f, -2.0f, -1.0f));
    sun.color = glm::vec3(1.0f, 0.95f, 0.8f);
    sun.intensity = 2.0f;
    pipeline.AddLight(sun);

    Light3D pointLight;
    pointLight.type = Light3D::Type::Point;
    pointLight.position = glm::vec3(0.0f, 2.0f, 0.0f);
    pointLight.radius = 8.0f;
    pipeline.AddLight(pointLight);

    EXPECT_EQ(pipeline.GetLightCount(), 2u);
    EXPECT_EQ(pipeline.GetLights()[0].type, Light3D::Type::Directional);
    EXPECT_EQ(pipeline.GetLights()[1].type, Light3D::Type::Point);
}
