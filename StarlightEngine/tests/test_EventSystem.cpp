#include <gtest/gtest.h>
#include "EventSystem.hpp"

using namespace starlight;

class EventSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_system.OnInitialize({});
    }

    EventSystem m_system;
};

TEST_F(EventSystemTest, SubscribeAndEmit) {
    int count = 0;
    m_system.Subscribe(EventType::EntityCreated, [&](const Event&) { count++; });

    Event ev{EventType::EntityCreated};
    m_system.Emit(ev);
    EXPECT_EQ(count, 1);
}

TEST_F(EventSystemTest, MultipleSubscribers) {
    int c1 = 0, c2 = 0;
    m_system.Subscribe(EventType::Collision, [&](const Event&) { c1++; });
    m_system.Subscribe(EventType::Collision, [&](const Event&) { c2++; });

    Event ev{EventType::Collision};
    m_system.Emit(ev);
    EXPECT_EQ(c1, 1);
    EXPECT_EQ(c2, 1);
}

TEST_F(EventSystemTest, EmitDifferentTypes) {
    int entityCount = 0, collisionCount = 0;
    m_system.Subscribe(EventType::EntityCreated, [&](const Event&) { entityCount++; });
    m_system.Subscribe(EventType::Collision, [&](const Event&) { collisionCount++; });

    m_system.Emit(Event{EventType::EntityCreated});
    m_system.Emit(Event{EventType::Collision});
    m_system.Emit(Event{EventType::EntityCreated});
    EXPECT_EQ(entityCount, 2);
    EXPECT_EQ(collisionCount, 1);
}

TEST_F(EventSystemTest, UnsubscribedTypeNotCalled) {
    int count = 0;
    m_system.Subscribe(EventType::Collision, [&](const Event&) { count++; });

    m_system.Emit(Event{EventType::EntityCreated});
    EXPECT_EQ(count, 0);
}

TEST_F(EventSystemTest, CustomTypeSubscribeAndEmit) {
    int count = 0;
    m_system.Subscribe("my_event", [&](const Event&) { count++; });

    Event ev{EventType::Custom, nullptr, 0, "my_event"};
    m_system.Emit(ev);
    EXPECT_EQ(count, 1);
}

TEST_F(EventSystemTest, CustomTypeDoesNotMatchBuiltin) {
    int builtin = 0;
    m_system.Subscribe(EventType::Collision, [&](const Event&) { builtin++; });

    Event ev{EventType::Custom, nullptr, 0, "Collision"};
    m_system.Emit(ev);
    EXPECT_EQ(builtin, 0);
}

TEST_F(EventSystemTest, DeferredEmitDoesNotFireImmediately) {
    int count = 0;
    m_system.Subscribe(EventType::EntityCreated, [&](const Event&) { count++; });

    m_system.EmitDeferred(Event{EventType::EntityCreated});
    EXPECT_EQ(count, 0);
}

TEST_F(EventSystemTest, FlushProcessesDeferredEvents) {
    int count = 0;
    m_system.Subscribe(EventType::EntityCreated, [&](const Event&) { count++; });

    m_system.EmitDeferred(Event{EventType::EntityCreated});
    m_system.Flush();
    EXPECT_EQ(count, 1);
}

TEST_F(EventSystemTest, DeferredEventsProcessedInOrder) {
    std::vector<int> order;
    m_system.Subscribe(EventType::EntityCreated, [&](const Event&) { order.push_back(1); });
    m_system.Subscribe(EventType::Collision, [&](const Event&) { order.push_back(2); });

    m_system.EmitDeferred(Event{EventType::EntityCreated});
    m_system.EmitDeferred(Event{EventType::Collision});
    m_system.Flush();
    ASSERT_EQ(order.size(), 2u);
    EXPECT_EQ(order[0], 1);
    EXPECT_EQ(order[1], 2);
}

TEST_F(EventSystemTest, OnUpdateCallsFlush) {
    int count = 0;
    m_system.Subscribe(EventType::EntityCreated, [&](const Event&) { count++; });

    m_system.EmitDeferred(Event{EventType::EntityCreated});
    m_system.OnUpdate(0.0f);
    EXPECT_EQ(count, 1);
}

TEST_F(EventSystemTest, MultipleEmitSameEvent) {
    int count = 0;
    m_system.Subscribe(EventType::InputPressed, [&](const Event&) { count++; });

    for (int i = 0; i < 10; i++) {
        m_system.Emit(Event{EventType::InputPressed});
    }
    EXPECT_EQ(count, 10);
}

TEST_F(EventSystemTest, SubscriberCanReceiveEventData) {
    const void* received = nullptr;
    m_system.Subscribe(EventType::SceneChanged, [&](const Event& ev) {
        received = ev.data;
    });

    int data = 42;
    Event ev{EventType::SceneChanged};
    ev.data = &data;
    m_system.Emit(ev);
    EXPECT_EQ(received, &data);
}
