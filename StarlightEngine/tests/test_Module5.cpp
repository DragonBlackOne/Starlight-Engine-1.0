#include <gtest/gtest.h>
#include "core/Assets/AssetTypes.hpp"
#include "core/Assets/Asset.hpp"
#include "core/Assets/FileWatcher.hpp"
#include "core/Assets/AssetManager.hpp"

using namespace starlight::assets;

// ---------------------------------------------------------------------------
// 1. Asset Registry & Metadata Tests
// ---------------------------------------------------------------------------
TEST(Module5AssetTest, AssetRegistrationAndMetadata) {
    auto& mgr = AssetManager::Get();
    mgr.Clear();

    AssetHandle hTex = mgr.RegisterAsset("@assets/textures/character.png", AssetType::Texture, "textures/character.png");
    EXPECT_NE(hTex, 0u);
    EXPECT_TRUE(mgr.HasAsset(hTex));

    const auto* meta = mgr.GetMetadata(hTex);
    ASSERT_NE(meta, nullptr);
    EXPECT_EQ(meta->type, AssetType::Texture);
    EXPECT_EQ(meta->virtualPath, "@assets/textures/character.png");
    EXPECT_EQ(meta->physicalPath, "textures/character.png");

    AssetHandle found = mgr.GetHandleFromPath("@assets/textures/character.png");
    EXPECT_EQ(found, hTex);
}

TEST(Module5AssetTest, AssetTypeToStringConversions) {
    EXPECT_STREQ(AssetTypeToString(AssetType::Texture), "Texture");
    EXPECT_STREQ(AssetTypeToString(AssetType::Mesh3D), "Mesh3D");
    EXPECT_STREQ(AssetTypeToString(AssetType::Audio), "Audio");
    EXPECT_STREQ(AssetTypeToString(AssetType::Shader), "Shader");
    EXPECT_STREQ(AssetTypeToString(AssetType::Script), "Script");
}

// ---------------------------------------------------------------------------
// 2. Asset Storage and Retrieval
// ---------------------------------------------------------------------------
TEST(Module5AssetTest, StoreAndRetrieveTextAsset) {
    auto& mgr = AssetManager::Get();
    mgr.Clear();

    AssetHandle hScript = mgr.RegisterAsset("@assets/scripts/main.lua", AssetType::Script);
    auto textAsset = std::make_shared<TextAsset>("print('Fusion Engine Active')");

    mgr.StoreAsset(hScript, textAsset);

    auto retrieved = mgr.GetAsset<TextAsset>(hScript);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_TRUE(retrieved->IsReady());
    EXPECT_EQ(retrieved->GetText(), "print('Fusion Engine Active')");

    // Retrieve by path
    auto retrievedByPath = mgr.GetAsset<TextAsset>("@assets/scripts/main.lua");
    EXPECT_EQ(retrievedByPath, retrieved);

    // Unload
    mgr.UnloadAsset(hScript);
    EXPECT_EQ(mgr.GetAsset<TextAsset>(hScript), nullptr);
}

// ---------------------------------------------------------------------------
// 3. Hot-Reloading & File Watcher Trigger
// ---------------------------------------------------------------------------
TEST(Module5AssetTest, HotReloadingCallbackTrigger) {
    auto& mgr = AssetManager::Get();
    mgr.Clear();

    AssetHandle hShader = mgr.RegisterAsset("@assets/shaders/pbr.frag", AssetType::Shader, "shaders/pbr.frag");
    mgr.EnableHotReloading(hShader);

    bool reloaded = false;
    AssetHandle reloadedHandle = 0;

    mgr.SetOnAssetReloadedCallback([&](AssetHandle h) {
        reloaded = true;
        reloadedHandle = h;
    });

    mgr.TriggerFileChangeManually("shaders/pbr.frag", FileAction::Modified);

    EXPECT_TRUE(reloaded);
    EXPECT_EQ(reloadedHandle, hShader);
}
