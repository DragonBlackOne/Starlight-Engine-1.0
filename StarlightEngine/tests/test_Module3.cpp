#include <gtest/gtest.h>
#include "core/ECS/Entity.hpp"
#include "core/ECS/CoreComponents.hpp"
#include "core/Scene/Scene.hpp"
#include "core/Scene/SceneGraph.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace starlight::ecs;

// ---------------------------------------------------------------------------
// 1. Entity & Component Operations
// ---------------------------------------------------------------------------
TEST(Module3ECSTest, EntityComponentLifecycle) {
    entt::registry registry;
    Entity e(registry.create(), &registry);

    EXPECT_TRUE(e.IsValid());
    EXPECT_FALSE(e.HasComponent<TagComponent>());

    e.AddComponent<TagComponent>("Player");
    EXPECT_TRUE(e.HasComponent<TagComponent>());
    EXPECT_EQ(e.GetComponent<TagComponent>().tag, "Player");

    e.GetComponent<TagComponent>().tag = "Hero";
    EXPECT_EQ(e.GetComponent<TagComponent>().tag, "Hero");

    e.RemoveComponent<TagComponent>();
    EXPECT_FALSE(e.HasComponent<TagComponent>());
}

TEST(Module3ECSTest, EntityHandleComparison) {
    entt::registry registry;
    Entity e1(registry.create(), &registry);
    Entity e2(registry.create(), &registry);
    Entity e1Copy = e1;

    EXPECT_EQ(e1, e1Copy);
    EXPECT_NE(e1, e2);
}

// ---------------------------------------------------------------------------
// 2. Scene Entity Management
// ---------------------------------------------------------------------------
TEST(Module3SceneTest, CreateAndFindEntities) {
    starlight::ecs::Scene scene;
    auto player = scene.CreateEntity("Gene");
    auto enemy = scene.CreateEntity("Punk");

    EXPECT_TRUE(player.IsValid());
    EXPECT_TRUE(enemy.IsValid());

    EXPECT_EQ(player.GetComponent<TagComponent>().tag, "Gene");
    EXPECT_EQ(enemy.GetComponent<TagComponent>().tag, "Punk");

    auto found = scene.FindEntityByName("Gene");
    EXPECT_EQ(found, player);

    uint64_t uuid = player.GetComponent<UUIDComponent>().uuid;
    auto foundByUUID = scene.GetEntityByUUID(uuid);
    EXPECT_EQ(foundByUUID, player);
}

TEST(Module3SceneTest, DestroyEntityAndHierarchy) {
    starlight::ecs::Scene scene;
    auto parent = scene.CreateEntity("Parent");
    auto child = scene.CreateEntity("Child");

    SceneGraph::SetParent(child, parent);
    EXPECT_EQ(child.GetComponent<RelationshipComponent>().parent, parent.GetHandle());
    EXPECT_EQ(parent.GetComponent<RelationshipComponent>().children.size(), 1u);

    scene.DestroyEntity(parent);

    // Both parent and child should be destroyed
    EXPECT_FALSE(parent.IsValid());
    EXPECT_FALSE(child.IsValid());
}

// ---------------------------------------------------------------------------
// 3. Scene Graph & World Transform Hierarchy
// ---------------------------------------------------------------------------
TEST(Module3SceneGraphTest, TransformHierarchyPropagation) {
    starlight::ecs::Scene scene;

    // Grandparent at (10, 0, 0)
    auto grandParent = scene.CreateEntity("GrandParent");
    grandParent.GetComponent<TransformComponent>().SetPosition(glm::vec3(10.0f, 0.0f, 0.0f));

    // Parent at (0, 5, 0) relative to GrandParent
    auto parent = scene.CreateEntity("Parent");
    parent.GetComponent<TransformComponent>().SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    SceneGraph::SetParent(parent, grandParent);

    // Child at (0, 0, 2) relative to Parent
    auto child = scene.CreateEntity("Child");
    child.GetComponent<TransformComponent>().SetPosition(glm::vec3(0.0f, 0.0f, 2.0f));
    SceneGraph::SetParent(child, parent);

    scene.UpdateHierarchy();

    glm::mat4 childWorld = child.GetComponent<TransformComponent>().worldMatrix;
    glm::vec3 childWorldPos = glm::vec3(childWorld[3]);

    EXPECT_FLOAT_EQ(childWorldPos.x, 10.0f);
    EXPECT_FLOAT_EQ(childWorldPos.y, 5.0f);
    EXPECT_FLOAT_EQ(childWorldPos.z, 2.0f);
}

TEST(Module3SceneGraphTest, ScaleHierarchyPropagation) {
    starlight::ecs::Scene scene;

    auto parent = scene.CreateEntity("Parent");
    parent.GetComponent<TransformComponent>().SetScale(glm::vec3(2.0f, 2.0f, 2.0f));

    auto child = scene.CreateEntity("Child");
    child.GetComponent<TransformComponent>().SetPosition(glm::vec3(5.0f, 0.0f, 0.0f));
    SceneGraph::SetParent(child, parent);

    scene.UpdateHierarchy();

    glm::mat4 childWorld = child.GetComponent<TransformComponent>().worldMatrix;
    glm::vec3 childWorldPos = glm::vec3(childWorld[3]);

    // Scaled by 2: (5, 0, 0) -> (10, 0, 0)
    EXPECT_FLOAT_EQ(childWorldPos.x, 10.0f);
    EXPECT_FLOAT_EQ(childWorldPos.y, 0.0f);
    EXPECT_FLOAT_EQ(childWorldPos.z, 0.0f);
}
