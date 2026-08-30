#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "core/Math.hpp"
#include "core/Memory/Allocator.hpp"
#include "core/Memory/DoubleBufferAllocator.hpp"
#include "core/Memory/PoolAllocator.hpp"
#include "core/Memory/StackAllocator.hpp"
#include "core/SIMDUtils.hpp"

using namespace starlight;

// ---------------------------------------------------------------------------
// StackAllocator
// ---------------------------------------------------------------------------
TEST(StackAllocatorTest, InitialState) {
    StackAllocator stack(1024);
    EXPECT_EQ(stack.GetUsedMemory(), 0u);
    EXPECT_EQ(stack.GetTotalSize(), 1024u);
}

TEST(StackAllocatorTest, AllocateBasic) {
    StackAllocator stack(1024);
    void* a = stack.Allocate(64);
    void* b = stack.Allocate(64);
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    EXPECT_NE(a, b);
    // Overhead (block headers + padding) on top of the payloads.
    EXPECT_GE(stack.GetUsedMemory(), 128u);
}

TEST(StackAllocatorTest, LIFOFree) {
    StackAllocator stack(1024);
    void* a = stack.Allocate(100);
    size_t usedA = stack.GetUsedMemory();
    void* b = stack.Allocate(50);
    size_t usedB = stack.GetUsedMemory();
    EXPECT_GT(usedB, usedA);

    stack.Deallocate(b);
    EXPECT_EQ(stack.GetUsedMemory(), usedA);
    stack.Deallocate(a);
    EXPECT_EQ(stack.GetUsedMemory(), 0u);
}

TEST(StackAllocatorTest, OutOfOrderFreeIgnored) {
    StackAllocator stack(1024);
    void* a = stack.Allocate(100);
    void* b = stack.Allocate(100);
    size_t used = stack.GetUsedMemory();
    // Freeing "a" (not the top) must be ignored.
    stack.Deallocate(a);
    EXPECT_EQ(stack.GetUsedMemory(), used);
    // Now freeing the top works.
    stack.Deallocate(b);
}

TEST(StackAllocatorTest, Rewind) {
    StackAllocator stack(1024);
    void* a = stack.Allocate(100);
    (void)a;
    StackAllocator::Mark mark = stack.GetMark();
    void* b = stack.Allocate(200);
    (void)b;
    EXPECT_GT(stack.GetUsedMemory(), mark);
    stack.Rewind(mark);
    EXPECT_EQ(stack.GetUsedMemory(), mark);
    // Memory is reusable after rewind.
    void* c = stack.Allocate(200);
    ASSERT_NE(c, nullptr);
}

TEST(StackAllocatorTest, ClearReusesMemory) {
    StackAllocator stack(1024);
    stack.Allocate(500);
    stack.Clear();
    EXPECT_EQ(stack.GetUsedMemory(), 0u);
    void* ptr = stack.Allocate(500);
    ASSERT_NE(ptr, nullptr);
}

TEST(StackAllocatorTest, AllocateOverCapacity) {
    StackAllocator stack(64);
    EXPECT_EQ(stack.Allocate(128), nullptr);
}

TEST(StackAllocatorTest, Alignment) {
    StackAllocator stack(1024);
    void* p = stack.Allocate(8, 32);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(p) % 32, 0u);
}

// ---------------------------------------------------------------------------
// DoubleBufferAllocator
// ---------------------------------------------------------------------------
TEST(DoubleBufferTest, AlternatesBuffers) {
    DoubleBufferAllocator dba(1024);
    EXPECT_EQ(dba.GetActiveUsed(), 0u);

    void* p0 = dba.Allocate(128);
    ASSERT_NE(p0, nullptr);
    EXPECT_EQ(dba.GetActiveUsed(), 128u);

    // Swap keeps buffer 0 data intact and makes buffer 1 the active one.
    dba.Swap();
    EXPECT_EQ(dba.GetActiveUsed(), 0u);
    EXPECT_EQ(dba.GetInactiveUsed(), 128u);

    // Write into the now-active buffer; buffer 0 data must remain readable.
    void* p1 = dba.Allocate(64);
    ASSERT_NE(p1, nullptr);
    EXPECT_EQ(dba.GetActiveUsed(), 64u);
}

TEST(DoubleBufferTest, AllocateFrameConvenience) {
    DoubleBufferAllocator dba(512);
    void* p = dba.AllocateFrame(256);
    ASSERT_NE(p, nullptr);
    // The convenience helper swaps after allocating.
    EXPECT_EQ(dba.GetActiveUsed(), 0u);
    EXPECT_EQ(dba.GetInactiveUsed(), 256u);
}

TEST(DoubleBufferTest, OverflowReturnsNull) {
    DoubleBufferAllocator dba(128);
    void* p = dba.Allocate(64);
    ASSERT_NE(p, nullptr);
    // Second allocation that does not fit into the remaining 64 bytes + header.
    EXPECT_EQ(dba.Allocate(192), nullptr);
}

// ---------------------------------------------------------------------------
// StlAllocator
// ---------------------------------------------------------------------------
TEST(StlAllocatorTest, VectorWithLinearAllocator) {
    LinearAllocator linear(4096);
    StlAllocator<int> stl(&linear);
    std::vector<int, StlAllocator<int>> vec(stl);

    for (int i = 0; i < 100; ++i) {
        vec.push_back(i);
    }
    EXPECT_EQ(vec.size(), 100u);
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(vec[i], i);
    }
    // 100 ints = 400 bytes, must fit in a 4KB linear arena.
    EXPECT_LE(linear.GetUsedMemory(), 4096u);
}

TEST(StlAllocatorTest, StringWithPoolAllocator) {
    PoolAllocator pool(sizeof(char) * 64, 64);
    StlAllocator<char> stl(&pool);
    std::basic_string<char, std::char_traits<char>, StlAllocator<char>> s(stl);
    s = "hello stl allocator integration";
    EXPECT_EQ(s, "hello stl allocator integration");
}

TEST(StlAllocatorTest, Alignment32) {
    LinearAllocator linear(1024);
    StlAllocator<double> stl(&linear);
    double* ptr = stl.allocate(16);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % alignof(double), 0u);
    stl.deallocate(ptr, 16);
}

// ---------------------------------------------------------------------------
// SIMDUtils / Math
// ---------------------------------------------------------------------------
TEST(CoreMathTest, Mat4MulMatchesGlm) {
    glm::mat4 a = glm::translate(glm::mat4(1.0f), glm::vec3(10, 20, 30));
    glm::mat4 b = glm::rotate(glm::mat4(1.0f), 0.7f, glm::vec3(0, 1, 0));
    glm::mat4 expected = a * b;

    glm::mat4 out = simd::Mat4Mul(a, b);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_NEAR(out[i][j], expected[i][j], 0.001f) << "col " << i << " row " << j;
        }
    }
}

TEST(CoreMathTest, Mat4MulRawMatchesGlm) {
    glm::mat4 a = glm::scale(glm::mat4(1.0f), glm::vec3(2, 3, 4));
    glm::mat4 b = glm::translate(glm::mat4(1.0f), glm::vec3(5, 6, 7));
    glm::mat4 expected = a * b;

    float out[16];
    simd::Mat4Mul(out, glm::value_ptr(a), glm::value_ptr(b));
    for (int i = 0; i < 16; ++i) {
        EXPECT_NEAR(out[i], glm::value_ptr(expected)[i], 0.001f) << "idx " << i;
    }
}

TEST(CoreMathTest, QuatMulMatchesGlm) {
    glm::quat a = glm::quat(0.2f, 0.3f, 0.4f, 0.5f);
    glm::quat b = glm::quat(0.6f, 0.1f, 0.2f, 0.3f);
    glm::quat expected = a * b;
    glm::quat out = simd::QuatMul(a, b);
    EXPECT_NEAR(out.w, expected.w, 0.0001f);
    EXPECT_NEAR(out.x, expected.x, 0.0001f);
    EXPECT_NEAR(out.y, expected.y, 0.0001f);
    EXPECT_NEAR(out.z, expected.z, 0.0001f);
}

TEST(CoreMathTest, QuatDotMatchesGlm) {
    glm::quat a = glm::normalize(glm::quat(1.0f, 2.0f, 3.0f, 4.0f));
    glm::quat b = glm::normalize(glm::quat(4.0f, 3.0f, 2.0f, 1.0f));
    EXPECT_NEAR(simd::QuatDot(a, b), glm::dot(a, b), 0.0001f);
}

TEST(CoreMathTest, IsaDetection) {
    simd::Isa isa = simd::GetHighestIsa();
    // On x86_64 hosts, SSE2 is effectively guaranteed.
    EXPECT_TRUE(isa >= simd::Isa::SSE);
}

TEST(CoreMathTest, AlignUpHelpers) {
    EXPECT_EQ(simd::AlignUpAddr(100, 16), 112u);
    EXPECT_EQ(simd::AlignUpAddr(112, 16), 112u);
    EXPECT_EQ(starlight::AlignUp(101, 16), 112u);
    EXPECT_EQ(starlight::AlignUp(96, 16), 96u);
}

// ---------------------------------------------------------------------------
// Engine Version & Diagnostics API (v12.0.0)
// ---------------------------------------------------------------------------
#include "Version.hpp"
#include "Engine.hpp"
#include "core/Memory/MemoryManager.hpp"
#include "Renderer2D.hpp"
#include "AssetLoader.hpp"
#include "AudioSystem.hpp"
#include "InputSystem.hpp"
#include "JobSystem.hpp"
#include "DecalSystem.hpp"
#include "Profiler.hpp"
#include "MathUtils.hpp"

TEST(EngineVersionTest, VersionConstants) {
    EXPECT_EQ(Engine::GetVersionMajor(), 15);
    EXPECT_EQ(Engine::GetVersionMinor(), 0);
    EXPECT_EQ(Engine::GetVersionPatch(), 0);
    EXPECT_STREQ(Engine::GetVersionString(), "15.0.0");
    EXPECT_STREQ(Engine::GetVersionCodename(), "Cosmos");
    EXPECT_STREQ(Engine::GetEngineName(), "Starlight Engine");
    EXPECT_EQ(Engine::GetVersionNumber(), 150000);
}

TEST(MemoryManagerTelemetryTest, GlobalAllocationsAndQueries) {
    MemoryManager::Initialize();
    
    EXPECT_GE(MemoryManager::Get().GetAllocatorCount(), 0u);
    void* ptr = MemoryManager::AllocateFrame(256);
    EXPECT_NE(ptr, nullptr);
    EXPECT_GE(MemoryManager::Get().GetTotalUsedMemory(), 256u);
    EXPECT_GE(MemoryManager::Get().GetTotalCapacity(), 4 * 1024 * 1024u);

    MemoryManager::ClearFrame();
    EXPECT_EQ(MemoryManager::Get().GetTotalUsedMemory(), 0u);

    MemoryManager::Shutdown();
}

TEST(Renderer2DStatsTest, ResetAndQueryStats) {
    Renderer2D::ResetStats();
    auto stats = Renderer2D::GetStats();
    EXPECT_EQ(stats.drawCalls, 0u);
    EXPECT_EQ(stats.quadCount, 0u);
}

TEST(ProceduralGeometryTest, TorusAndIcosphereGeneration) {
    auto torus = AssetLoader::CreateTorusMesh(1.0f, 0.3f, 16, 12);
    ASSERT_NE(torus, nullptr);
    EXPECT_GT(torus->GetIndexCount(), 0u);

    auto ico = AssetLoader::CreateIcosphereMesh(1.0f, 1);
    ASSERT_NE(ico, nullptr);
    EXPECT_GT(ico->GetIndexCount(), 0u);
}

TEST(CameraTraumaTest, TraumaClampingAndDecay) {
    Renderer renderer;
    renderer.SetCameraTrauma(0.0f);
    EXPECT_FLOAT_EQ(renderer.GetCameraTrauma(), 0.0f);

    renderer.AddCameraTrauma(0.75f);
    EXPECT_FLOAT_EQ(renderer.GetCameraTrauma(), 0.75f);

    renderer.AddCameraTrauma(0.5f); // Should clamp to 1.0
    EXPECT_FLOAT_EQ(renderer.GetCameraTrauma(), 1.0f);

    renderer.SetCameraTrauma(0.5f);
    EXPECT_FLOAT_EQ(renderer.GetCameraTrauma(), 0.5f);
}

TEST(ColorGradingTest, ParametersApplication) {
    Renderer renderer;
    renderer.SetColorGrading(1.2f, 1.1f, 1.3f, 2.0f, 0.4f);
    EXPECT_FLOAT_EQ(renderer.GetExposure(), 1.2f);
    EXPECT_FLOAT_EQ(renderer.GetContrast(), 1.1f);
    EXPECT_FLOAT_EQ(renderer.GetSaturation(), 1.3f);
    EXPECT_FLOAT_EQ(renderer.GetGamma(), 2.0f);
    EXPECT_FLOAT_EQ(renderer.GetVignetteStrength(), 0.4f);
}

TEST(MaterialAudioImpactTest, MaterialEnumIntegrity) {
    EXPECT_EQ(static_cast<int>(AudioSystem::AudioMaterial::Metal), 0);
    EXPECT_EQ(static_cast<int>(AudioSystem::AudioMaterial::Wood), 1);
    EXPECT_EQ(static_cast<int>(AudioSystem::AudioMaterial::Concrete), 2);
    EXPECT_EQ(static_cast<int>(AudioSystem::AudioMaterial::Flesh), 3);
    EXPECT_EQ(static_cast<int>(AudioSystem::AudioMaterial::CyberShield), 4);
    EXPECT_EQ(static_cast<int>(AudioSystem::AudioMaterial::Glass), 5);
}

TEST(InputActionMappingTest, ActionBindingAndAxisResolution) {
    InputSystem input;
    input.BindAction("Jump", pal::KeyCode::Space);
    input.BindAction("MoveLeft", pal::KeyCode::A);
    input.BindAction("MoveRight", pal::KeyCode::D);

    auto jumpKeys = input.GetActionKeys("Jump");
    ASSERT_EQ(jumpKeys.size(), 1u);
    EXPECT_EQ(jumpKeys[0], pal::KeyCode::Space);

    // Initial state: not pressed
    EXPECT_FLOAT_EQ(input.GetActionAxis("MoveLeft", "MoveRight"), 0.0f);

    input.ClearActionBindings("Jump");
    EXPECT_EQ(input.GetActionKeys("Jump").size(), 0u);
}

TEST(AudioSpatialParametersTest, DopplerAndAttenuationParameters) {
    AudioSystem audio;
    audio.SetDopplerFactor(2.5f);
    EXPECT_FLOAT_EQ(audio.GetDopplerFactor(), 2.5f);

    audio.SetDopplerFactor(10.0f); // Clamped to 5.0
    EXPECT_FLOAT_EQ(audio.GetDopplerFactor(), 5.0f);
}

TEST(JobSystemTest, ParallelExecutionAndDispatch) {
    JobSystem jobs;
    EngineContext ctx;
    jobs.OnInitialize(ctx);

    std::atomic<int> counter{0};
    auto h1 = jobs.Dispatch([&counter]() {
        counter.fetch_add(10, std::memory_order_relaxed);
    }, JobPriority::High);

    jobs.Wait(h1);
    EXPECT_EQ(counter.load(), 10);

    // ParallelFor
    std::vector<int> data(100, 0);
    jobs.ParallelFor(100, [&data](uint32_t i) {
        data[i] = static_cast<int>(i * 2);
    }, 16);

    for (size_t i = 0; i < 100; ++i) {
        EXPECT_EQ(data[i], static_cast<int>(i * 2));
    }

    jobs.OnShutdown();
}

TEST(DecalSystemTest, DecalSpawnAndFadeDecay) {
    DecalSystem decals;
    EngineContext ctx;
    decals.OnInitialize(ctx);

    uint32_t id = decals.SpawnDecal(glm::vec3(0, 0, 0), glm::quat(1, 0, 0, 0), glm::vec3(1, 1, 0.2f), "textures/impact.png", 2.0f);
    EXPECT_GT(id, 0u);
    EXPECT_EQ(decals.GetActiveDecalCount(), 1u);

    // Advance time past lifetime
    decals.OnUpdate(2.5f);
    EXPECT_EQ(decals.GetActiveDecalCount(), 0u);

    decals.OnShutdown();
}

TEST(ProfilerTest, FrameTimeHistoryAndOscilloscopeStats) {
    Profiler& profiler = Profiler::Get();
    profiler.Clear();

    profiler.BeginFrame();
    profiler.BeginSample("PhysicsTest");
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    profiler.EndSample("PhysicsTest");
    profiler.EndFrame(16.66f);

    EXPECT_GT(profiler.GetAvgFPS(), 0.0f);
    EXPECT_EQ(profiler.GetFrameTimeHistory().size(), 120u);

    auto samples = profiler.GetActiveSamples();
    EXPECT_GE(samples.size(), 1u);
}

TEST(MathUtilsTest, GeometryAndColorConversions) {
    float dist = 0.0f;
    bool hitPlane = starlight::MathUtils::RayPlaneIntersection(glm::vec3(0, 10, 0), glm::vec3(0, -1, 0),
                                                              glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), dist);
    EXPECT_TRUE(hitPlane);
    EXPECT_FLOAT_EQ(dist, 10.0f);

    // Ray Triangle Intersection (Möller-Trumbore)
    glm::vec2 uv(0.0f);
    bool hitTri = starlight::MathUtils::RayTriangleIntersection(glm::vec3(0, 5, 0), glm::vec3(0, -1, 0),
                                                               glm::vec3(-1, 0, -1), glm::vec3(1, 0, -1), glm::vec3(0, 0, 1),
                                                               dist, uv);
    EXPECT_TRUE(hitTri);
    EXPECT_FLOAT_EQ(dist, 5.0f);

    // Ray Sphere Intersection
    bool hitSphere = starlight::MathUtils::RaySphereIntersection(glm::vec3(0, 0, -10), glm::vec3(0, 0, 1),
                                                                glm::vec3(0, 0, 0), 2.0f, dist);
    EXPECT_TRUE(hitSphere);
    EXPECT_NEAR(dist, 8.0f, 0.01f);

    // SmoothDamp
    float current = 0.0f;
    float velocity = 0.0f;
    float nextVal = starlight::MathUtils::SmoothDamp(current, 10.0f, velocity, 0.1f, 100.0f, 0.016f);
    EXPECT_GT(nextVal, 0.0f);
    EXPECT_LT(nextVal, 10.0f);

    glm::vec3 red(1.0f, 0.0f, 0.0f);
    glm::vec3 hsl = starlight::MathUtils::RGBToHSL(red);
    glm::vec3 rgbRecovered = starlight::MathUtils::HSLToRGB(hsl);
    EXPECT_NEAR(red.r, rgbRecovered.r, 0.01f);
    EXPECT_NEAR(red.g, rgbRecovered.g, 0.01f);
    EXPECT_NEAR(red.b, rgbRecovered.b, 0.01f);

    float fastInv = starlight::MathUtils::FastInvSqrt(4.0f);
    EXPECT_NEAR(fastInv, 0.5f, 0.02f);
}

