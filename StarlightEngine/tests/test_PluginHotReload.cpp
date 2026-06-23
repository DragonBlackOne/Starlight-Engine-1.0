#include <gtest/gtest.h>
#include "PluginSystem.hpp"

using namespace starlight;

TEST(PluginHotReloadTest, SystemInitialization) {
    PluginSystem pluginSys;
    EXPECT_FALSE(pluginSys.IsPluginLoaded());
}
