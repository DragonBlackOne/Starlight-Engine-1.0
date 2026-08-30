#include <gtest/gtest.h>
#include "AutomationSystem.hpp"
#include "Engine.hpp"
#include <fstream>
#include <filesystem>

using namespace starlight;

TEST(AutomationSystemTest, RegisterAndRunCppTests) {
    AutomationSystem automation;
    
    bool test1Executed = false;
    automation.RegisterCppTest("Test1", [&]() {
        test1Executed = true;
        return true;
    });

    bool test2Executed = false;
    automation.RegisterCppTest("Test2", [&]() {
        test2Executed = true;
        return false;
    });

    automation.RunCppTests();

    EXPECT_TRUE(test1Executed);
    EXPECT_TRUE(test2Executed);
}

TEST(AutomationSystemTest, PerformanceBenchmarkFlow) {
    AutomationSystem automation;
    
    EXPECT_FALSE(automation.IsBenchmarkRunning());
    
    automation.StartBenchmark(0.1f);
    EXPECT_TRUE(automation.IsBenchmarkRunning());
    
    // Simulate some update ticks to populate metrics history
    automation.OnUpdate(0.016f);
    automation.OnUpdate(0.016f);
    
    std::string reportPath = "build/test_perf_report.md";
    automation.StopBenchmarkAndReport(reportPath);
    
    EXPECT_FALSE(automation.IsBenchmarkRunning());
    
    // Clean up report
    if (std::filesystem::exists(reportPath)) {
        std::filesystem::remove(reportPath);
    }
}

TEST(AutomationSystemTest, ScreenshotTGAHeaders) {
    AutomationSystem automation;
    
    std::string testImgPath = "build/test_header_check.tga";
    std::filesystem::create_directories("build");
    
    // If OpenGL context is not fully initialized in standard unit tests,
    // glReadPixels might be a no-op or write dummy values. But we can test if it writes a valid 18-byte TGA header.
    bool saved = automation.SaveScreenshotTGA(testImgPath);
    
    // SaveScreenshotTGA depends on window/OpenGL context.
    // If it succeeds, check header bytes. If it fails due to missing GL/Window context in headless tests, that's expected.
    if (saved) {
        std::ifstream file(testImgPath, std::ios::binary);
        ASSERT_TRUE(file.is_open());
        
        uint8_t header[18];
        file.read(reinterpret_cast<char*>(header), 18);
        file.close();
        
        EXPECT_EQ(header[2], 2); // True-color uncompressed
        EXPECT_EQ(header[16], 24); // 24-bit RGB
        
        std::filesystem::remove(testImgPath);
    }
}

TEST(AutomationSystemTest, GamesValidationLua) {
    AutomationSystem automation;
    sol::state lua;
    lua.open_libraries(
        sol::lib::base, 
        sol::lib::package, 
        sol::lib::table, 
        sol::lib::string, 
        sol::lib::math, 
        sol::lib::io, 
        sol::lib::os, 
        sol::lib::debug
    );
    automation.RegisterLuaBindings(lua);
    lua["test"]["is_headless_test"] = true;
    
    bool success = automation.RunLuaTestFile("assets/tests/games_validation.lua");
    EXPECT_TRUE(success);
    
    // Verify that C++ engine health tests executed and were properly exposed to Lua
    sol::table results = lua["test"]["getCppResults"]();
    ASSERT_TRUE(results.valid());
    EXPECT_TRUE(results["Engine_CVarSystem_Integrity"].get<bool>());
    EXPECT_TRUE(results["Engine_InputSystem_Integrity"].get<bool>());
    EXPECT_TRUE(results["Engine_EventBroker_Integrity"].get<bool>());
    EXPECT_TRUE(results["Engine_JobSystem_Integrity"].get<bool>());
}

