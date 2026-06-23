#include <gtest/gtest.h>
#include "ConsoleSystem.hpp"

using namespace starlight;

class ConsoleSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(ConsoleSystemTest, LogStoresMessage) {
    auto& cs = ConsoleSystem::Get();
    cs.Log("hello world");
    ASSERT_GE(cs.GetLogs().size(), 1u);
    EXPECT_NE(cs.GetLogs().back().find("hello world"), std::string::npos);
}

TEST_F(ConsoleSystemTest, MultipleLogs) {
    auto& cs = ConsoleSystem::Get();
    size_t before = cs.GetLogs().size();
    cs.Log("first");
    cs.Log("second");
    cs.Log("third");
    EXPECT_EQ(cs.GetLogs().size(), before + 3);
}

TEST_F(ConsoleSystemTest, InitiallyHidden) {
    auto& cs = ConsoleSystem::Get();
    EXPECT_FALSE(cs.IsVisible());
}

TEST_F(ConsoleSystemTest, ToggleVisibility) {
    auto& cs = ConsoleSystem::Get();
    cs.Toggle();
    EXPECT_TRUE(cs.IsVisible());
    cs.Toggle();
    EXPECT_FALSE(cs.IsVisible());
}

TEST_F(ConsoleSystemTest, RegisterFloatCVar) {
    auto& cs = ConsoleSystem::Get();
    float value = 42.0f;
    cs.RegisterCVar("test_float", &value);
}

TEST_F(ConsoleSystemTest, RegisterIntCVar) {
    auto& cs = ConsoleSystem::Get();
    int value = 10;
    cs.RegisterCVar("test_int", &value);
}

TEST_F(ConsoleSystemTest, ExecuteSetFloatCVar) {
    auto& cs = ConsoleSystem::Get();
    float value = 0.0f;
    cs.RegisterCVar("gravity", &value);
    cs.Execute("gravity 9.81");
    EXPECT_FLOAT_EQ(value, 9.81f);
}

TEST_F(ConsoleSystemTest, ExecuteSetIntCVar) {
    auto& cs = ConsoleSystem::Get();
    int value = 0;
    cs.RegisterCVar("max_entities", &value);
    cs.Execute("max_entities 100");
    EXPECT_EQ(value, 100);
}

TEST_F(ConsoleSystemTest, ExecuteUnknownCommand) {
    auto& cs = ConsoleSystem::Get();
    cs.Execute("nonexistent_command");
}

TEST_F(ConsoleSystemTest, ExecuteWithoutArgs) {
    auto& cs = ConsoleSystem::Get();
    float value = 1.0f;
    cs.RegisterCVar("test", &value);
    cs.Execute("test");
    EXPECT_FLOAT_EQ(value, 1.0f);
}

TEST_F(ConsoleSystemTest, ExecuteMultipleCVars) {
    auto& cs = ConsoleSystem::Get();
    float fval = 0.0f;
    int ival = 0;
    cs.RegisterCVar("fvar", &fval);
    cs.RegisterCVar("ivar", &ival);

    cs.Execute("fvar 3.14");
    cs.Execute("ivar 42");
    EXPECT_FLOAT_EQ(fval, 3.14f);
    EXPECT_EQ(ival, 42);
}

TEST_F(ConsoleSystemTest, ExecuteInvalidFloat) {
    auto& cs = ConsoleSystem::Get();
    float value = 0.0f;
    cs.RegisterCVar("test", &value);
    EXPECT_THROW(cs.Execute("test not_a_number"), std::invalid_argument);
    EXPECT_FLOAT_EQ(value, 0.0f);
}

TEST_F(ConsoleSystemTest, ExecuteInvalidInt) {
    auto& cs = ConsoleSystem::Get();
    int value = 0;
    cs.RegisterCVar("test", &value);
    EXPECT_THROW(cs.Execute("test not_an_int"), std::invalid_argument);
    EXPECT_EQ(value, 0);
}

TEST_F(ConsoleSystemTest, ExecuteWithTrailingSpaces) {
    auto& cs = ConsoleSystem::Get();
    float value = 0.0f;
    cs.RegisterCVar("test", &value);
    cs.Execute("test   5.0  ");
    EXPECT_FLOAT_EQ(value, 5.0f);
}
