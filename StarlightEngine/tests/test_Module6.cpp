#include <gtest/gtest.h>
#include "core/Physics/PhysicsCommon.hpp"
#include "core/Physics/PhysicsWorld.hpp"
#include "core/Scripting/ScriptEngine.hpp"

using namespace starlight::physics;
using namespace starlight::scripting;

// ---------------------------------------------------------------------------
// 1. Physics Engine Tests
// ---------------------------------------------------------------------------
TEST(Module6PhysicsTest, PhysicsBodyCreationAndIntegration) {
    SimulationPhysicsWorld world;
    world.SetGravity(glm::vec3(0.0f, -10.0f, 0.0f));

    RigidBodyDesc desc;
    desc.bodyType = BodyType::Dynamic;
    desc.mass = 2.0f;
    desc.linearDamping = 0.0f;

    uint32_t body = world.CreateBody(glm::vec3(0.0f, 10.0f, 0.0f), desc);
    EXPECT_EQ(body, 1u);

    // Step 1.0s under gravity (-10 m/s^2)
    world.Step(1.0f);

    glm::vec3 vel = world.GetBodyLinearVelocity(body);
    glm::vec3 pos = world.GetBodyPosition(body);

    EXPECT_FLOAT_EQ(vel.y, -10.0f);
    EXPECT_FLOAT_EQ(pos.y, 0.0f); // 10 + (-10 * 1) = 0
}

TEST(Module6PhysicsTest, PhysicsForceAndImpulse) {
    SimulationPhysicsWorld world;
    world.SetGravity(glm::vec3(0.0f)); // Zero gravity

    RigidBodyDesc desc;
    desc.bodyType = BodyType::Dynamic;
    desc.mass = 5.0f;
    desc.linearDamping = 0.0f;

    uint32_t body = world.CreateBody(glm::vec3(0.0f), desc);

    // Apply impulse 10 along X (mass = 5 => velocity = 2)
    world.ApplyImpulse(body, glm::vec3(10.0f, 0.0f, 0.0f));
    EXPECT_FLOAT_EQ(world.GetBodyLinearVelocity(body).x, 2.0f);

    // Step 1s => pos should be (2, 0, 0)
    world.Step(1.0f);
    EXPECT_FLOAT_EQ(world.GetBodyPosition(body).x, 2.0f);
}

TEST(Module6PhysicsTest, PhysicsRaycastHit) {
    SimulationPhysicsWorld world;

    RigidBodyDesc desc;
    desc.bodyType = BodyType::Static;
    desc.radius = 1.0f;

    // Body centered at (0, 0, 10) with radius 1
    world.CreateBody(glm::vec3(0.0f, 0.0f, 10.0f), desc);

    RayCastHit hit;
    bool didHit = world.RayCast(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 20.0f), hit);

    EXPECT_TRUE(didHit);
    EXPECT_TRUE(hit.hasHit);
    EXPECT_FLOAT_EQ(hit.distance, 9.0f); // 10 - radius(1) = 9
    EXPECT_FLOAT_EQ(hit.point.z, 9.0f);
}

// ---------------------------------------------------------------------------
// 2. Scripting Runtime (Lua / Sol2) Tests
// ---------------------------------------------------------------------------
TEST(Module6ScriptingTest, LuaExecutionAndVariables) {
    auto& engine = ScriptEngine::Get();
    engine.Initialize();

    engine.SetGlobal("engine_score", 100);
    EXPECT_EQ(engine.GetGlobal<int>("engine_score"), 100);

    bool ok = engine.ExecuteString("engine_score = engine_score + 50");
    EXPECT_TRUE(ok);
    EXPECT_EQ(engine.GetGlobal<int>("engine_score"), 150);
}

TEST(Module6ScriptingTest, LuaVec3MathBinding) {
    auto& engine = ScriptEngine::Get();
    engine.Initialize();

    bool ok = engine.ExecuteString(R"(
        local v1 = vec3.new(1.0, 2.0, 3.0)
        local v2 = vec3.new(4.0, 5.0, 6.0)
        local v3 = v1 + v2
        res_x = v3.x
        res_y = v3.y
        res_z = v3.z
    )");

    EXPECT_TRUE(ok);
    EXPECT_FLOAT_EQ(engine.GetGlobal<float>("res_x"), 5.0f);
    EXPECT_FLOAT_EQ(engine.GetGlobal<float>("res_y"), 7.0f);
    EXPECT_FLOAT_EQ(engine.GetGlobal<float>("res_z"), 9.0f);
}

TEST(Module6ScriptingTest, LuaErrorHandling) {
    auto& engine = ScriptEngine::Get();
    engine.Initialize();

    bool ok = engine.ExecuteString("invalid syntax +++ == 123");
    EXPECT_FALSE(ok);
    EXPECT_FALSE(engine.GetLastError().empty());
}
