#include <gtest/gtest.h>
#include "RenderDevice.hpp"
#include "PrefabSystem.hpp"
#include "VFSSystem.hpp"
#include "Components.hpp"
#include <fstream>
#include <filesystem>
#include <glad/glad.h>

using namespace starlight;

TEST(RHITest, CreateDeviceAndResources) {
    // Se glad não foi inicializado (ponteiros nulos), pulamos o teste de OpenGL
    // que requer contexto gráfico ativo.
    if (glGenBuffers == nullptr) {
        GTEST_SKIP() << "No active OpenGL context. Skipping RHI test.";
        return;
    }

    auto device = rhi::RenderDevice::CreateOpenGLDevice();
    ASSERT_NE(device, nullptr);

    float vertices[] = { 0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f };
    auto buf = device->CreateBuffer(rhi::BufferType::Vertex, sizeof(vertices), vertices);
    ASSERT_NE(buf, nullptr);
    EXPECT_EQ(buf->GetSize(), sizeof(vertices));

    uint8_t pixels[] = { 255, 0, 0, 255 }; // 1 red pixel
    auto tex = device->CreateTexture2D(1, 1, 0, pixels);
    ASSERT_NE(tex, nullptr);
    EXPECT_EQ(tex->GetWidth(), 1);
    EXPECT_EQ(tex->GetHeight(), 1);
}

TEST(PrefabTest, InstantiateFromJson) {
    std::string prefabData = R"({
        "TagComponent": { "tag": "Hero" },
        "TransformComponent": {
            "position": { "x": 10.0, "y": 20.0, "z": 30.0 },
            "scale": { "x": 2.0, "y": 2.0, "z": 2.0 }
        },
        "SpriteComponent": {
            "textureID": 1024,
            "visible": true
        },
        "ReverbZoneComponent": {
            "minDistance": 5.0,
            "maxDistance": 15.0,
            "reverbFactor": 0.8
        }
    })";

    std::string filename = "temp_prefab.json";
    std::ofstream out(filename);
    out << prefabData;
    out.close();

    // Make sure VFS is initialized if not already
    VFSSystem::Get().Initialize();

    entt::registry reg;
    auto ent = PrefabSystem::Instantiate(filename, &reg);
    ASSERT_TRUE(ent != entt::null);

    EXPECT_TRUE(reg.all_of<TagComponent>(ent));
    EXPECT_TRUE(reg.all_of<TransformComponent>(ent));
    EXPECT_TRUE(reg.all_of<SpriteComponent>(ent));
    EXPECT_TRUE(reg.all_of<ReverbZoneComponent>(ent));

    EXPECT_EQ(reg.get<TagComponent>(ent).tag, "Hero");
    EXPECT_FLOAT_EQ(reg.get<TransformComponent>(ent).position.x, 10.0f);
    EXPECT_FLOAT_EQ(reg.get<TransformComponent>(ent).position.y, 20.0f);
    EXPECT_FLOAT_EQ(reg.get<TransformComponent>(ent).position.z, 30.0f);
    EXPECT_FLOAT_EQ(reg.get<TransformComponent>(ent).scale.x, 2.0f);
    EXPECT_EQ(reg.get<SpriteComponent>(ent).textureID, 1024u);
    EXPECT_FLOAT_EQ(reg.get<ReverbZoneComponent>(ent).minDistance, 5.0f);
    EXPECT_FLOAT_EQ(reg.get<ReverbZoneComponent>(ent).maxDistance, 15.0f);
    EXPECT_FLOAT_EQ(reg.get<ReverbZoneComponent>(ent).reverbFactor, 0.8f);

    std::filesystem::remove(filename);
}

TEST(RHITest, CreateVulkanDeviceAndGroundwork) {
    auto device = rhi::RenderDevice::CreateVulkanDevice();
    ASSERT_NE(device, nullptr);

    auto buf = device->CreateBuffer(rhi::BufferType::Vertex, 128, nullptr);
    ASSERT_NE(buf, nullptr);
    EXPECT_EQ(buf->GetSize(), 128);

    auto tex = device->CreateTexture2D(64, 64, 0, nullptr);
    ASSERT_NE(tex, nullptr);
    EXPECT_EQ(tex->GetWidth(), 64);
    EXPECT_EQ(tex->GetHeight(), 64);

    auto pipeline = device->CreatePipeline();
    ASSERT_NE(pipeline, nullptr);
    EXPECT_NO_THROW(pipeline->Bind());
}

#include "SIMDMath.hpp"

TEST(SIMDMathTest, IsAABBInFrustumAVX2) {
    // Define 6 planes. Each plane is nx, ny, nz, d
    // Left, Right, Bottom, Top, Near, Far planes of a simple box frustum from -10 to 10
    float planes[24] = {
         1.0f,  0.0f,  0.0f, 10.0f, // Left:   x > -10 => 1*x + 10 > 0
        -1.0f,  0.0f,  0.0f, 10.0f, // Right:  x <  10 => -1*x + 10 > 0
         0.0f,  1.0f,  0.0f, 10.0f, // Bottom: y > -10 => 1*y + 10 > 0
         0.0f, -1.0f,  0.0f, 10.0f, // Top:    y <  10 => -1*y + 10 > 0
         0.0f,  0.0f,  1.0f, 10.0f, // Near:   z > -10 => 1*z + 10 > 0
         0.0f,  0.0f, -1.0f, 10.0f  // Far:    z <  10 => -1*z + 10 > 0
    };

    // AABB completely inside
    float minInside[3] = { -5.0f, -5.0f, -5.0f };
    float maxInside[3] = {  5.0f,  5.0f,  5.0f };
    EXPECT_TRUE(simd::IsAABBInFrustum(planes, minInside, maxInside));

    // AABB intersecting
    float minInter[3] = { -15.0f, -5.0f, -5.0f };
    float maxInter[3] = {   0.0f,  5.0f,  5.0f };
    EXPECT_TRUE(simd::IsAABBInFrustum(planes, minInter, maxInter));

    // AABB completely outside Left plane (x < -10)
    float minOutsideLeft[3] = { -20.0f, -5.0f, -5.0f };
    float maxOutsideLeft[3] = { -12.0f,  5.0f,  5.0f };
    EXPECT_FALSE(simd::IsAABBInFrustum(planes, minOutsideLeft, maxOutsideLeft));

    // AABB completely outside Right plane (x > 10)
    float minOutsideRight[3] = { 12.0f, -5.0f, -5.0f };
    float maxOutsideRight[3] = { 20.0f,  5.0f,  5.0f };
    EXPECT_FALSE(simd::IsAABBInFrustum(planes, minOutsideRight, maxOutsideRight));
}

#include "FSRSystem.hpp"

TEST(FSRSystemTest, InitializationAndDispatch) {
    FSRSystem fsr;
    FfxFsrContextDescription desc{};
    desc.flags = 0;
    desc.maxRenderSizeWidth = 1280;
    desc.maxRenderSizeHeight = 720;
    desc.displaySizeWidth = 1920;
    desc.displaySizeHeight = 1080;

    EXPECT_TRUE(fsr.Initialize(desc));
    
    FSRSettings settings;
    settings.enabled = true;
    settings.qualityMode = FSRQualityMode::Quality;
    settings.sharpness = 0.5f;
    fsr.SetSettings(settings);

    EXPECT_TRUE(fsr.GetSettings().enabled);
    EXPECT_FLOAT_EQ(fsr.GetSettings().sharpness, 0.5f);

    fsr.Dispatch(0, 0, 0.016f, 0.5f);

    fsr.Shutdown();
}
