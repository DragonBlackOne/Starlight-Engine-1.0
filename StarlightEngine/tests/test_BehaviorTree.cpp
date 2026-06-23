#include <gtest/gtest.h>
#include "BehaviorTree.hpp"

using namespace starlight;

TEST(BehaviorTreeTest, BTActionReturnsSuccess) {
    auto action = std::make_shared<BTAction>([](float) { return BTStatus::Success; });
    EXPECT_EQ(action->Update(1.0f), BTStatus::Success);
}

TEST(BehaviorTreeTest, BTActionReturnsFailure) {
    auto action = std::make_shared<BTAction>([](float) { return BTStatus::Failure; });
    EXPECT_EQ(action->Update(1.0f), BTStatus::Failure);
}

TEST(BehaviorTreeTest, BTActionReturnsRunning) {
    auto action = std::make_shared<BTAction>([](float) { return BTStatus::Running; });
    EXPECT_EQ(action->Update(1.0f), BTStatus::Running);
}

TEST(BehaviorTreeTest, SelectorReturnsSuccessOnFirstSuccess) {
    auto selector = std::make_shared<BTSelector>();
    selector->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Success; }));
    selector->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Failure; }));

    EXPECT_EQ(selector->Update(1.0f), BTStatus::Success);
}

TEST(BehaviorTreeTest, SelectorShortCircuitsOnSuccess) {
    int secondCalled = 0;
    auto selector = std::make_shared<BTSelector>();
    selector->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Success; }));
    selector->AddChild(std::make_shared<BTAction>([&](float) { secondCalled++; return BTStatus::Success; }));

    selector->Update(1.0f);
    EXPECT_EQ(secondCalled, 0);
}

TEST(BehaviorTreeTest, SelectorReturnsFailureWhenAllFail) {
    auto selector = std::make_shared<BTSelector>();
    selector->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Failure; }));
    selector->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Failure; }));
    selector->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Failure; }));

    EXPECT_EQ(selector->Update(1.0f), BTStatus::Failure);
}

TEST(BehaviorTreeTest, SelectorReturnsRunning) {
    auto selector = std::make_shared<BTSelector>();
    selector->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Failure; }));
    selector->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Running; }));
    selector->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Success; }));

    EXPECT_EQ(selector->Update(1.0f), BTStatus::Running);
}

TEST(BehaviorTreeTest, SequenceReturnsFailureOnFirstFailure) {
    auto sequence = std::make_shared<BTSequence>();
    sequence->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Success; }));
    sequence->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Failure; }));
    sequence->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Success; }));

    EXPECT_EQ(sequence->Update(1.0f), BTStatus::Failure);
}

TEST(BehaviorTreeTest, SequenceShortCircuitsOnFailure) {
    int thirdCalled = 0;
    auto sequence = std::make_shared<BTSequence>();
    sequence->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Success; }));
    sequence->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Failure; }));
    sequence->AddChild(std::make_shared<BTAction>([&](float) { thirdCalled++; return BTStatus::Success; }));

    sequence->Update(1.0f);
    EXPECT_EQ(thirdCalled, 0);
}

TEST(BehaviorTreeTest, SequenceReturnsSuccessWhenAllSucceed) {
    auto sequence = std::make_shared<BTSequence>();
    sequence->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Success; }));
    sequence->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Success; }));

    EXPECT_EQ(sequence->Update(1.0f), BTStatus::Success);
}

TEST(BehaviorTreeTest, SequenceReturnsRunning) {
    auto sequence = std::make_shared<BTSequence>();
    sequence->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Success; }));
    sequence->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Running; }));

    EXPECT_EQ(sequence->Update(1.0f), BTStatus::Running);
}

TEST(BehaviorTreeTest, EmptySelectorReturnsFailure) {
    auto selector = std::make_shared<BTSelector>();
    EXPECT_EQ(selector->Update(1.0f), BTStatus::Failure);
}

TEST(BehaviorTreeTest, EmptySequenceReturnsSuccess) {
    auto sequence = std::make_shared<BTSequence>();
    EXPECT_EQ(sequence->Update(1.0f), BTStatus::Success);
}

TEST(BehaviorTreeTest, NestedSelectorAndSequence) {
    auto root = std::make_shared<BTSelector>();
    auto seq = std::make_shared<BTSequence>();
    seq->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Success; }));
    seq->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Success; }));
    root->AddChild(seq);
    root->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Failure; }));

    EXPECT_EQ(root->Update(1.0f), BTStatus::Success);
}

TEST(BehaviorTreeTest, NestedSequenceFailsOnChildFailure) {
    auto root = std::make_shared<BTSequence>();
    auto sel = std::make_shared<BTSelector>();
    sel->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Failure; }));
    sel->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Failure; }));
    root->AddChild(sel);
    root->AddChild(std::make_shared<BTAction>([](float) { return BTStatus::Success; }));

    EXPECT_EQ(root->Update(1.0f), BTStatus::Failure);
}

TEST(BehaviorTreeTest, DtIsPassedThroughToActions) {
    float capturedDt = 0.0f;
    auto action = std::make_shared<BTAction>([&](float dt) {
        capturedDt = dt;
        return BTStatus::Success;
    });
    action->Update(3.14f);
    EXPECT_FLOAT_EQ(capturedDt, 3.14f);
}

TEST(BehaviorTreeTest, DtPassesThroughComposites) {
    float capturedDt = 0.0f;
    auto seq = std::make_shared<BTSequence>();
    seq->AddChild(std::make_shared<BTAction>([&](float dt) {
        capturedDt = dt;
        return BTStatus::Success;
    }));
    seq->Update(2.718f);
    EXPECT_FLOAT_EQ(capturedDt, 2.718f);
}
