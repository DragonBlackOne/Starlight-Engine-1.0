#include <gtest/gtest.h>
#include "CoreMinimal.hpp"

using namespace starlight;

class TestScene : public BaseScene {
public:
    int enterCount = 0;
    int exitCount = 0;
    int updateCount = 0;

    void OnEnter() override { enterCount++; }
    void OnExit() override { exitCount++; }
    void OnUpdate(float dt) override { updateCount++; BaseScene::OnUpdate(dt); }
};

TEST(SceneStackTest, InitiallyEmpty) {
    SceneStack stack;
    EXPECT_EQ(stack.Active(), nullptr);
}

TEST(SceneStackTest, PushMakesSceneActive) {
    SceneStack stack;
    auto scene = std::make_shared<TestScene>();
    stack.Push(scene);
    EXPECT_EQ(stack.Active(), scene);
}

TEST(SceneStackTest, PushCallsOnEnter) {
    SceneStack stack;
    auto scene = std::make_shared<TestScene>();
    stack.Push(scene);
    EXPECT_EQ(scene->enterCount, 1);
}

TEST(SceneStackTest, PopCallsOnExit) {
    SceneStack stack;
    auto scene = std::make_shared<TestScene>();
    stack.Push(scene);
    stack.Pop();
    EXPECT_EQ(scene->exitCount, 1);
}

TEST(SceneStackTest, PopMakesPreviousSceneActive) {
    SceneStack stack;
    auto scene1 = std::make_shared<TestScene>();
    auto scene2 = std::make_shared<TestScene>();
    stack.Push(scene1);
    stack.Push(scene2);
    stack.Pop();
    EXPECT_EQ(stack.Active(), scene1);
}

TEST(SceneStackTest, PopOnEmptyStackDoesNothing) {
    SceneStack stack;
    stack.Pop();
    EXPECT_EQ(stack.Active(), nullptr);
}

TEST(SceneStackTest, PushExitsPreviousAndEntersNew) {
    SceneStack stack;
    auto scene1 = std::make_shared<TestScene>();
    auto scene2 = std::make_shared<TestScene>();
    stack.Push(scene1);
    stack.Push(scene2);
    EXPECT_EQ(scene1->exitCount, 1);
    EXPECT_EQ(scene2->enterCount, 1);
}

TEST(SceneStackTest, PopReEntersPreviousScene) {
    SceneStack stack;
    auto scene1 = std::make_shared<TestScene>();
    auto scene2 = std::make_shared<TestScene>();
    stack.Push(scene1);
    stack.Push(scene2);
    stack.Pop();
    EXPECT_EQ(scene1->enterCount, 2);
}

TEST(SceneStackTest, MultiplePushPopCycles) {
    SceneStack stack;
    auto scene1 = std::make_shared<TestScene>();
    auto scene2 = std::make_shared<TestScene>();
    auto scene3 = std::make_shared<TestScene>();

    stack.Push(scene1);
    stack.Push(scene2);
    stack.Push(scene3);
    EXPECT_EQ(stack.Active(), scene3);

    stack.Pop();
    EXPECT_EQ(stack.Active(), scene2);

    stack.Pop();
    EXPECT_EQ(stack.Active(), scene1);

    stack.Pop();
    EXPECT_EQ(stack.Active(), nullptr);
}

TEST(SceneStackTest, ActiveSceneReceivesEvents) {
    SceneStack stack;
    auto scene = std::make_shared<TestScene>();
    stack.Push(scene);
    scene->OnUpdate(1.0f);
    EXPECT_EQ(scene->updateCount, 1);
}

TEST(SceneStackTest, MultipleScenesGetCorrectEnterExitOrder) {
    SceneStack stack;
    std::vector<int> order;
    auto makeScene = [&](int id) {
        class OrderScene : public BaseScene {
        public:
            int m_id;
            std::vector<int>& m_order;
            OrderScene(int id, std::vector<int>& order) : m_id(id), m_order(order) {}
            void OnEnter() override { m_order.push_back(m_id); }
            void OnExit() override { m_order.push_back(-m_id); }
        };
        return std::make_shared<OrderScene>(id, order);
    };

    auto s1 = makeScene(1);
    auto s2 = makeScene(2);
    auto s3 = makeScene(3);

    stack.Push(s1);
    stack.Push(s2);
    stack.Push(s3);
    stack.Pop();
    stack.Pop();

    std::vector<int> expected = {1, -1, 2, -2, 3, -3, 2, -2, 1};
    EXPECT_EQ(order, expected);
}
