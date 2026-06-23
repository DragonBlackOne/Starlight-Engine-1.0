#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

// Test hierarchy logic in isolation without including Components.hpp
// Components.hpp requires glad/glad.h (OpenGL) — we test the pure math here.
struct RelComponent {
    entt::entity parent = entt::null;
    std::vector<entt::entity> children;
};

struct XformComponent {
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};
    glm::mat4 localMatrix = glm::mat4(1.0f);
    glm::mat4 worldMatrix = glm::mat4(1.0f);

    void UpdateLocalMatrix() {
        localMatrix = glm::translate(glm::mat4(1.0f), position);
        localMatrix *= glm::mat4_cast(rotation);
        localMatrix = glm::scale(localMatrix, scale);
    }
};

static void UpdateRecursive(entt::registry& registry, entt::entity entity, const glm::mat4& parentMatrix) {
    auto& tc = registry.get<XformComponent>(entity);
    tc.worldMatrix = parentMatrix * tc.localMatrix;

    if (registry.all_of<RelComponent>(entity)) {
        auto& rc = registry.get<RelComponent>(entity);
        for (entt::entity child : rc.children) {
            if (registry.all_of<XformComponent>(child)) {
                UpdateRecursive(registry, child, tc.worldMatrix);
            }
        }
    }
}

static void HierarchyUpdate(entt::registry& registry) {
    auto view = registry.view<XformComponent>();
    for (auto entity : view) {
        auto& tc = view.get<XformComponent>(entity);
        tc.UpdateLocalMatrix();
    }

    auto allView = registry.view<XformComponent>();
    for (auto entity : allView) {
        bool hasParent = false;
        if (registry.all_of<RelComponent>(entity)) {
            auto& rc = registry.get<RelComponent>(entity);
            if (rc.parent != entt::null) {
                hasParent = true;
            }
        }
        if (!hasParent) {
            UpdateRecursive(registry, entity, glm::mat4(1.0f));
        }
    }
}

TEST(HierarchySystemTest, SingleEntityWorldEqualsLocal) {
    entt::registry reg;
    auto e = reg.create();
    auto& tc = reg.emplace<XformComponent>(e);
    tc.position = glm::vec3(10, 20, 30);

    HierarchyUpdate(reg);
    EXPECT_EQ(tc.worldMatrix, tc.localMatrix);
}

TEST(HierarchySystemTest, ChildInheritsParentTranslation) {
    entt::registry reg;
    auto parent = reg.create();
    auto& ptc = reg.emplace<XformComponent>(parent);
    ptc.position = glm::vec3(100, 0, 0);
    auto& prc = reg.emplace<RelComponent>(parent);

    auto child = reg.create();
    auto& ctc = reg.emplace<XformComponent>(child);
    ctc.position = glm::vec3(50, 0, 0);
    auto& crc = reg.emplace<RelComponent>(child);
    crc.parent = parent;
    prc.children.push_back(child);

    HierarchyUpdate(reg);

    glm::vec4 childWorldPos = ctc.worldMatrix * glm::vec4(0, 0, 0, 1);
    EXPECT_NEAR(childWorldPos.x, 150.0f, 0.001f);
    EXPECT_NEAR(childWorldPos.y, 0.0f, 0.001f);
    EXPECT_NEAR(childWorldPos.z, 0.0f, 0.001f);
}

TEST(HierarchySystemTest, GrandchildInheritsHierarchy) {
    entt::registry reg;
    auto gp = reg.create();
    reg.emplace<XformComponent>(gp).position = glm::vec3(100, 0, 0);
    auto& gprc = reg.emplace<RelComponent>(gp);

    auto p = reg.create();
    reg.emplace<XformComponent>(p).position = glm::vec3(50, 0, 0);
    auto& prc = reg.emplace<RelComponent>(p);
    prc.parent = gp;
    gprc.children.push_back(p);

    auto c = reg.create();
    reg.emplace<XformComponent>(c).position = glm::vec3(25, 0, 0);
    auto& crc = reg.emplace<RelComponent>(c);
    crc.parent = p;
    prc.children.push_back(c);

    HierarchyUpdate(reg);

    glm::vec4 cPos = reg.get<XformComponent>(c).worldMatrix * glm::vec4(0, 0, 0, 1);
    EXPECT_NEAR(cPos.x, 175.0f, 0.001f);
}

TEST(HierarchySystemTest, ChildScaleAffectsGrandchild) {
    entt::registry reg;
    auto p = reg.create();
    reg.emplace<XformComponent>(p).scale = glm::vec3(2, 2, 2);
    auto& prc = reg.emplace<RelComponent>(p);

    auto c = reg.create();
    reg.emplace<XformComponent>(c).position = glm::vec3(10, 0, 0);
    auto& crc = reg.emplace<RelComponent>(c);
    crc.parent = p;
    prc.children.push_back(c);

    HierarchyUpdate(reg);

    glm::vec4 cPos = reg.get<XformComponent>(c).worldMatrix * glm::vec4(0, 0, 0, 1);
    EXPECT_NEAR(cPos.x, 20.0f, 0.001f);
}

TEST(HierarchySystemTest, ChildRotationAffectsGrandchild) {
    entt::registry reg;
    auto p = reg.create();
    reg.emplace<XformComponent>(p).rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0));
    auto& prc = reg.emplace<RelComponent>(p);

    auto c = reg.create();
    reg.emplace<XformComponent>(c).position = glm::vec3(10, 0, 0);
    auto& crc = reg.emplace<RelComponent>(c);
    crc.parent = p;
    prc.children.push_back(c);

    HierarchyUpdate(reg);

    glm::vec4 cPos = reg.get<XformComponent>(c).worldMatrix * glm::vec4(0, 0, 0, 1);
    EXPECT_NEAR(cPos.x, 0.0f, 0.001f);
    EXPECT_NEAR(cPos.z, -10.0f, 0.001f);
}

TEST(HierarchySystemTest, OrphanEntityUsesLocalMatrix) {
    entt::registry reg;
    auto e = reg.create();
    reg.emplace<XformComponent>(e).position = glm::vec3(42, 0, 0);
    reg.emplace<RelComponent>(e);

    HierarchyUpdate(reg);

    glm::vec4 pos = reg.get<XformComponent>(e).worldMatrix * glm::vec4(0, 0, 0, 1);
    EXPECT_NEAR(pos.x, 42.0f, 0.001f);
}

TEST(HierarchySystemTest, EmptyRegistryDoesNotCrash) {
    entt::registry reg;
    HierarchyUpdate(reg);
}

TEST(HierarchySystemTest, EntityWithoutRelationshipUsesLocalMatrix) {
    entt::registry reg;
    auto e = reg.create();
    reg.emplace<XformComponent>(e).position = glm::vec3(77, 0, 0);

    HierarchyUpdate(reg);

    glm::vec4 pos = reg.get<XformComponent>(e).worldMatrix * glm::vec4(0, 0, 0, 1);
    EXPECT_NEAR(pos.x, 77.0f, 0.001f);
}

TEST(HierarchySystemTest, MultipleChildren) {
    entt::registry reg;
    auto p = reg.create();
    reg.emplace<XformComponent>(p).position = glm::vec3(10, 0, 0);
    auto& prc = reg.emplace<RelComponent>(p);

    for (int i = 0; i < 5; i++) {
        auto c = reg.create();
        reg.emplace<XformComponent>(c).position = glm::vec3((float)(i * 10), 0, 0);
        auto& crc = reg.emplace<RelComponent>(c);
        crc.parent = p;
        prc.children.push_back(c);

        HierarchyUpdate(reg);

        glm::vec4 cPos = reg.get<XformComponent>(c).worldMatrix * glm::vec4(0, 0, 0, 1);
        EXPECT_NEAR(cPos.x, 10.0f + i * 10.0f, 0.001f);
    }
}
