#include <gtest/gtest.h>
#include "CVarSystem.hpp"
#include "EventBroker.hpp"
#include "ConfigSystem.hpp"
#include "InputSystem.hpp"
#include <string>
#include <cstdio>

using namespace starlight;

// --- CVAR SYSTEM TESTS ---

TEST(CVarSystemTest, RegisterAndAccessInt) {
    CVarSystem cvars;
    
    cvars.RegisterInt("g_testInt", 42, "A test integer variable");
    EXPECT_TRUE(cvars.Exists("g_testInt"));
    EXPECT_EQ(cvars.GetInt("g_testInt"), 42);
    
    cvars.SetInt("g_testInt", 100);
    EXPECT_EQ(cvars.GetInt("g_testInt"), 100);
}

TEST(CVarSystemTest, RegisterAndAccessFloat) {
    CVarSystem cvars;
    
    cvars.RegisterFloat("g_testFloat", 3.14f, "A test float variable");
    EXPECT_TRUE(cvars.Exists("g_testFloat"));
    EXPECT_NEAR(cvars.GetFloat("g_testFloat"), 3.14f, 0.001f);
    
    cvars.SetFloat("g_testFloat", 2.718f);
    EXPECT_NEAR(cvars.GetFloat("g_testFloat"), 2.718f, 0.001f);
}

TEST(CVarSystemTest, RegisterAndAccessBool) {
    CVarSystem cvars;
    
    cvars.RegisterBool("g_testBool", false, "A test bool variable");
    EXPECT_TRUE(cvars.Exists("g_testBool"));
    EXPECT_FALSE(cvars.GetBool("g_testBool"));
    
    cvars.SetBool("g_testBool", true);
    EXPECT_TRUE(cvars.GetBool("g_testBool"));
}

TEST(CVarSystemTest, RegisterAndAccessString) {
    CVarSystem cvars;
    
    cvars.RegisterString("g_testString", "hello", "A test string variable");
    EXPECT_TRUE(cvars.Exists("g_testString"));
    EXPECT_EQ(cvars.GetString("g_testString"), "hello");
    
    cvars.SetString("g_testString", "world");
    EXPECT_EQ(cvars.GetString("g_testString"), "world");
}

TEST(CVarSystemTest, ChangeCallbacks) {
    CVarSystem cvars;
    int intCallbackVal = 0;
    float floatCallbackVal = 0.0f;
    bool boolCallbackVal = false;
    std::string stringCallbackVal = "";
    
    cvars.RegisterInt("g_testInt", 10, "Int", [&](int val) { intCallbackVal = val; });
    cvars.RegisterFloat("g_testFloat", 1.0f, "Float", [&](float val) { floatCallbackVal = val; });
    cvars.RegisterBool("g_testBool", false, "Bool", [&](bool val) { boolCallbackVal = val; });
    cvars.RegisterString("g_testString", "init", "String", [&](const std::string& val) { stringCallbackVal = val; });
    
    // Callbacks should trigger immediately on registration
    EXPECT_EQ(intCallbackVal, 10);
    EXPECT_NEAR(floatCallbackVal, 1.0f, 0.001f);
    EXPECT_FALSE(boolCallbackVal);
    EXPECT_EQ(stringCallbackVal, "init");
    
    // Change values and verify callbacks trigger
    cvars.SetInt("g_testInt", 50);
    EXPECT_EQ(intCallbackVal, 50);
    
    cvars.SetFloat("g_testFloat", 5.5f);
    EXPECT_NEAR(floatCallbackVal, 5.5f, 0.001f);
    
    cvars.SetBool("g_testBool", true);
    EXPECT_TRUE(boolCallbackVal);
    
    cvars.SetString("g_testString", "changed");
    EXPECT_EQ(stringCallbackVal, "changed");
}

TEST(CVarSystemTest, SetFromString) {
    CVarSystem cvars;
    
    cvars.RegisterInt("g_testInt", 0, "Int");
    cvars.RegisterFloat("g_testFloat", 0.0f, "Float");
    cvars.RegisterBool("g_testBool", false, "Bool");
    cvars.RegisterString("g_testString", "init", "String");
    
    EXPECT_TRUE(cvars.SetFromString("g_testInt", "42"));
    EXPECT_EQ(cvars.GetInt("g_testInt"), 42);
    
    EXPECT_TRUE(cvars.SetFromString("g_testFloat", "3.1415"));
    EXPECT_NEAR(cvars.GetFloat("g_testFloat"), 3.1415f, 0.001f);
    
    EXPECT_TRUE(cvars.SetFromString("g_testBool", "true"));
    EXPECT_TRUE(cvars.GetBool("g_testBool"));
    EXPECT_TRUE(cvars.SetFromString("g_testBool", "0"));
    EXPECT_FALSE(cvars.GetBool("g_testBool"));
    EXPECT_TRUE(cvars.SetFromString("g_testBool", "on"));
    EXPECT_TRUE(cvars.GetBool("g_testBool"));
    
    EXPECT_TRUE(cvars.SetFromString("g_testString", "outrun"));
    EXPECT_EQ(cvars.GetString("g_testString"), "outrun");
    
    // Invalid formats should return false and not crash
    EXPECT_FALSE(cvars.SetFromString("g_testInt", "not_an_int"));
    EXPECT_FALSE(cvars.SetFromString("g_testFloat", "not_a_float"));
    EXPECT_FALSE(cvars.SetFromString("g_nonExistent", "val"));
}

// --- EVENT BROKER TESTS ---

TEST(EventBrokerTest, PubSubCommunication) {
    EventBroker broker;
    
    // Initialize mock Lua state to satisfy Sol types
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::table);
    
    std::string receivedEvent = "";
    int receivedValue = 0;
    
    broker.Subscribe("on_test_event", [&](const std::string& name, const sol::table& data) {
        receivedEvent = name;
        if (data["val"].valid()) {
            receivedValue = data["val"].get<int>();
        }
    });
    
    sol::table dataTable = lua.create_table();
    dataTable["val"] = 999;
    
    broker.Publish("on_test_event", dataTable);
    
    EXPECT_EQ(receivedEvent, "on_test_event");
    EXPECT_EQ(receivedValue, 999);
}

TEST(EventBrokerTest, ExceptionSafety) {
    EventBroker broker;
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::table);
    
    // Subscriber that throws should not crash the publisher
    broker.Subscribe("on_throw", [](const std::string&, const sol::table&) {
        throw std::runtime_error("Test exception");
    });
    
    bool secondCallbackRan = false;
    broker.Subscribe("on_throw", [&](const std::string&, const sol::table&) {
        secondCallbackRan = true;
    });
    
    sol::table dataTable = lua.create_table();
    
    EXPECT_NO_THROW(broker.Publish("on_throw", dataTable));
    EXPECT_TRUE(secondCallbackRan);
}

// --- CONFIG SYSTEM TESTS ---

TEST(ConfigSystemTest, LoadSaveAndGetSet) {
    ConfigSystem config;
    
    config.SetInt("TestSection", "intValue", 42);
    config.SetFloat("TestSection", "floatValue", 3.14f);
    config.SetBool("TestSection", "boolValue", true);
    config.SetString("TestSection", "stringValue", "retro-synthwave");
    
    EXPECT_EQ(config.GetInt("TestSection", "intValue"), 42);
    EXPECT_NEAR(config.GetFloat("TestSection", "floatValue"), 3.14f, 0.001f);
    EXPECT_TRUE(config.GetBool("TestSection", "boolValue"));
    EXPECT_EQ(config.GetString("TestSection", "stringValue"), "retro-synthwave");
    
    EXPECT_EQ(config.GetInt("TestSection", "nonExistentInt", 99), 99);
    EXPECT_EQ(config.GetString("TestSection", "nonExistentString", "default"), "default");
    
    const std::string tempFile = "temp_test.ini";
    config.Save(tempFile);
    
    ConfigSystem loader;
    EXPECT_TRUE(loader.Load(tempFile));
    EXPECT_EQ(loader.GetInt("TestSection", "intValue"), 42);
    EXPECT_NEAR(loader.GetFloat("TestSection", "floatValue"), 3.14f, 0.001f);
    EXPECT_TRUE(loader.GetBool("TestSection", "boolValue"));
    EXPECT_EQ(loader.GetString("TestSection", "stringValue"), "retro-synthwave");
    
    std::remove(tempFile.c_str());
}

// --- INPUT SYSTEM TESTS ---

TEST(InputSystemTest, KeyCodeTranslation) {
    EXPECT_EQ(InputSystem::KeyCodeFromString("Space"), pal::KeyCode::Space);
    EXPECT_EQ(InputSystem::KeyCodeFromString("W"), pal::KeyCode::W);
    EXPECT_EQ(InputSystem::KeyCodeFromString("LShift"), pal::KeyCode::LShift);
    EXPECT_EQ(InputSystem::KeyCodeFromString("NonExistent"), pal::KeyCode::Unknown);
    
    EXPECT_EQ(InputSystem::StringFromKeyCode(pal::KeyCode::Space), "Space");
    EXPECT_EQ(InputSystem::StringFromKeyCode(pal::KeyCode::W), "W");
    EXPECT_EQ(InputSystem::StringFromKeyCode(pal::KeyCode::LShift), "LShift");
}

TEST(CVarSystemTest, AudioAndFrameRateCVars) {
    CVarSystem cvars;
    float mockVolume = 1.0f;
    float mockMusicVolume = 1.0f;
    float mockEffectsVolume = 1.0f;
    bool mockVSync = false;
    int mockMaxFPS = 60;

    cvars.RegisterFloat("snd_volume", 0.8f, "Master volume", [&](float val) { mockVolume = val; });
    cvars.RegisterFloat("snd_musicVolume", 0.5f, "Music volume", [&](float val) { mockMusicVolume = val; });
    cvars.RegisterFloat("snd_effectsVolume", 0.9f, "Effects volume", [&](float val) { mockEffectsVolume = val; });
    cvars.RegisterBool("r_vsync", true, "VSync", [&](bool val) { mockVSync = val; });
    cvars.RegisterInt("r_maxfps", 120, "Max FPS", [&](int val) { mockMaxFPS = val; });

    EXPECT_NEAR(mockVolume, 0.8f, 0.001f);
    EXPECT_NEAR(mockMusicVolume, 0.5f, 0.001f);
    EXPECT_NEAR(mockEffectsVolume, 0.9f, 0.001f);
    EXPECT_TRUE(mockVSync);
    EXPECT_EQ(mockMaxFPS, 120);

    cvars.SetFloat("snd_volume", 0.2f);
    cvars.SetBool("r_vsync", false);
    cvars.SetInt("r_maxfps", 0);

    EXPECT_NEAR(mockVolume, 0.2f, 0.001f);
    EXPECT_FALSE(mockVSync);
    EXPECT_EQ(mockMaxFPS, 0);
}
