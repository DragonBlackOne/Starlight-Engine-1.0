#include <gtest/gtest.h>
#include "Components.hpp"
#include "PhysicsSystem.hpp"

using namespace starlight;

TEST(PhysicsCharacterVirtualTest, ComponentInitialization) {
    CharacterControllerComponent comp;
    EXPECT_EQ(comp.height, 1.8f);
    EXPECT_EQ(comp.radius, 0.4f);
    EXPECT_EQ(comp.joltCharacter, nullptr);
    EXPECT_FALSE(comp.isGrounded);
}

TEST(PhysicsSystemTest, SafeShutdownRepeatedly) {
    PhysicsSystem physics;
    EngineContext ctx;
    
    physics.OnInitialize(ctx);
    physics.OnShutdown();
    
    EXPECT_NO_THROW(physics.OnShutdown());
}

TEST(PhysicsSystemTest, IndividualBodyDestruction) {
    PhysicsSystem physics;
    EngineContext ctx;
    physics.OnInitialize(ctx);

    entt::entity entity = (entt::entity)10;
    EXPECT_NO_THROW(physics.DestroyBody(entity));
    
    physics.OnShutdown();
}
