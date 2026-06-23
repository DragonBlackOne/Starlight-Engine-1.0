#include <gtest/gtest.h>
#include "Log.hpp"

using namespace starlight;

class LogTest : public ::testing::Test {
protected:
    void SetUp() override {
        Log::Init("test_engine.log");
        Log::SetMinLogLevel(LogLevel::Debug);
        // Clear history by consuming existing entries
        auto h = Log::GetHistory();
        (void)h;
    }
};

TEST_F(LogTest, InfoLogsMessage) {
    Log::Info("test message {}", 42);
    auto history = Log::GetHistory();
    bool found = false;
    for (const auto& msg : history) {
        if (msg.find("test message 42") != std::string::npos) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(LogTest, WarnLogsMessage) {
    Log::Warn("warning: {}", "something");
    auto history = Log::GetHistory();
    bool found = false;
    for (const auto& msg : history) {
        if (msg.find("warning: something") != std::string::npos) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(LogTest, ErrorLogsMessage) {
    Log::Error("error code {}", -1);
    auto history = Log::GetHistory();
    bool found = false;
    for (const auto& msg : history) {
        if (msg.find("error code -1") != std::string::npos) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(LogTest, FatalLogsMessage) {
    EXPECT_DEATH(Log::Fatal("fatal: {}", "crash"), ".*FATAL.*fatal: crash.*");
}

TEST_F(LogTest, LevelFilteringSuppressesLowerLevels) {
    Log::SetMinLogLevel(LogLevel::Error);
    Log::Info("should not appear");
    Log::Warn("should not appear either");
    Log::Error("this should appear");

    auto history = Log::GetHistory();
    bool foundInfo = false, foundWarn = false, foundError = false;
    for (const auto& msg : history) {
        if (msg.find("should not appear") != std::string::npos && msg.find("INFO") != std::string::npos) {
            foundInfo = true;
        }
        if (msg.find("should not appear either") != std::string::npos) {
            foundWarn = true;
        }
        if (msg.find("this should appear") != std::string::npos) {
            foundError = true;
        }
    }
    EXPECT_FALSE(foundInfo);
    EXPECT_FALSE(foundWarn);
    EXPECT_TRUE(foundError);
}

TEST_F(LogTest, TagOverloadAppendsTag) {
    Log::Info(LogTag{"[Test]"}, "message with tag");
    auto history = Log::GetHistory();
    bool found = false;
    for (const auto& msg : history) {
        if (msg.find("[Test]") != std::string::npos && msg.find("message with tag") != std::string::npos) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(LogTest, MultipleMessagesInOrder) {
    Log::Info("first");
    Log::Info("second");
    Log::Info("third");

    auto history = Log::GetHistory();
    size_t firstPos = std::string::npos, secondPos = std::string::npos, thirdPos = std::string::npos;
    for (size_t i = 0; i < history.size(); i++) {
        if (history[i].find("first") != std::string::npos) firstPos = i;
        if (history[i].find("second") != std::string::npos) secondPos = i;
        if (history[i].find("third") != std::string::npos) thirdPos = i;
    }
    EXPECT_LT(firstPos, secondPos);
    EXPECT_LT(secondPos, thirdPos);
}

TEST_F(LogTest, MinLogLevelDefault) {
    LogLevel defaultLevel = Log::GetMinLogLevel();
    EXPECT_EQ(defaultLevel, LogLevel::Debug);
}

TEST_F(LogTest, SetAndGetMinLogLevel) {
    Log::SetMinLogLevel(LogLevel::Warn);
    EXPECT_EQ(Log::GetMinLogLevel(), LogLevel::Warn);
    Log::SetMinLogLevel(LogLevel::Fatal);
    EXPECT_EQ(Log::GetMinLogLevel(), LogLevel::Fatal);
}
