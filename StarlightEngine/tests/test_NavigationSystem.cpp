#include <gtest/gtest.h>
#include "NavigationSystem.hpp"

using namespace starlight;

TEST(NavigationSystemTest, FindPathOnEmptyGrid) {
    NavigationSystem nav(10, 10.0f);
    glm::vec3 start(-5.0f, 0.0f, -5.0f);
    glm::vec3 end(5.0f, 0.0f, 5.0f);

    std::vector<glm::vec3> path;
    bool result = nav.FindPath(start, end, path);

    EXPECT_TRUE(result);
    EXPECT_GT(path.size(), 0);
}

TEST(NavigationSystemTest, BlockedPathGoesAround) {
    NavigationSystem nav(10, 10.0f);
    glm::vec3 start(-5.0f, 0.0f, -5.0f);
    glm::vec3 end(5.0f, 0.0f, 5.0f);

    // Block middle of the grid (3..6 on row 5), path should go around
    for (int i = 3; i <= 6; i++) {
        nav.SetObstacle(i, 5, true);
    }

    std::vector<glm::vec3> path;
    bool result = nav.FindPath(start, end, path);

    EXPECT_TRUE(result);
}

TEST(NavigationSystemTest, FullRowBlockedReturnsFalse) {
    NavigationSystem nav(10, 10.0f);
    glm::vec3 start(-5.0f, 0.0f, -5.0f);
    glm::vec3 end(5.0f, 0.0f, 5.0f);

    // Blocking the entire middle row creates an impassable wall
    for (int i = 0; i < 10; i++) {
        nav.SetObstacle(i, 5, true);
    }

    std::vector<glm::vec3> path;
    bool result = nav.FindPath(start, end, path);

    EXPECT_FALSE(result);
}

TEST(NavigationSystemTest, FullyBlockedReturnsFalse) {
    NavigationSystem nav(10, 10.0f);
    glm::vec3 start(-5.0f, 0.0f, -5.0f);
    glm::vec3 end(5.0f, 0.0f, 5.0f);

    for (int z = 0; z < 10; z++) {
        for (int x = 0; x < 10; x++) {
            nav.UpdateRect(0, 0, 9, 9, true);
        }
    }

    std::vector<glm::vec3> path;
    bool result = nav.FindPath(start, end, path);

    EXPECT_FALSE(result);
}

TEST(NavigationSystemTest, SameStartAndEndReturnsFalse) {
    NavigationSystem nav(10, 10.0f);
    glm::vec3 pos(0.0f, 0.0f, 0.0f);

    std::vector<glm::vec3> path;
    bool result = nav.FindPath(pos, pos, path);

    EXPECT_FALSE(result);
}

TEST(NavigationSystemTest, WorldToGridRoundtrip) {
    NavigationSystem nav(10, 10.0f);
    glm::vec3 world(-5.0f, 0.0f, -5.0f);

    NavPos grid = nav.WorldToGrid(world);
    glm::vec3 back = nav.GridToWorld(grid);

    EXPECT_NEAR(world.x, back.x, 2.0f);
    EXPECT_NEAR(world.z, back.z, 2.0f);
}

TEST(NavigationSystemTest, GridToWorldRoundtrip) {
    NavigationSystem nav(10, 10.0f);
    NavPos grid(2, 3);

    glm::vec3 world = nav.GridToWorld(grid);
    NavPos back = nav.WorldToGrid(world);

    EXPECT_EQ(grid.x, back.x);
    EXPECT_EQ(grid.z, back.z);
}
