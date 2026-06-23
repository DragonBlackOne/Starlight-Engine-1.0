#include <gtest/gtest.h>
#include "TextureAtlas.hpp"

using namespace starlight;

TEST(TextureAtlasTest, LoadGridCreatesCorrectRegions) {
    TextureAtlas atlas;
    atlas.LoadGrid(1, 4, 4);

    EXPECT_EQ(atlas.GetFrameCount(), 16);

    const auto* region = atlas.GetFrame(0);
    ASSERT_NE(region, nullptr);
    EXPECT_FLOAT_EQ(region->u, 0.0f);
    EXPECT_FLOAT_EQ(region->v, 0.0f);
    EXPECT_FLOAT_EQ(region->u2, 0.25f);
    EXPECT_FLOAT_EQ(region->v2, 0.25f);
}

TEST(TextureAtlasTest, LoadGridWithTotalFrames) {
    TextureAtlas atlas;
    atlas.LoadGrid(1, 4, 4, 10);

    EXPECT_EQ(atlas.GetFrameCount(), 10);
}

TEST(TextureAtlasTest, AddRegionByName) {
    TextureAtlas atlas;
    atlas.AddRegion("test", 0.0f, 0.0f, 0.5f, 0.5f);

    const auto* region = atlas.GetRegion("test");
    ASSERT_NE(region, nullptr);
    EXPECT_FLOAT_EQ(region->u, 0.0f);
    EXPECT_FLOAT_EQ(region->v, 0.0f);
    EXPECT_FLOAT_EQ(region->u2, 0.5f);
    EXPECT_FLOAT_EQ(region->v2, 0.5f);
}

TEST(TextureAtlasTest, AddRegionByGrid) {
    TextureAtlas atlas;
    atlas.AddRegion("top_left", 0, 0, 4, 4);
    atlas.AddRegion("bottom_right", 3, 3, 4, 4);

    const auto* r1 = atlas.GetRegion("top_left");
    ASSERT_NE(r1, nullptr);
    EXPECT_FLOAT_EQ(r1->u, 0.0f);
    EXPECT_FLOAT_EQ(r1->v, 0.0f);
    EXPECT_FLOAT_EQ(r1->u2, 0.25f);
    EXPECT_FLOAT_EQ(r1->v2, 0.25f);

    const auto* r2 = atlas.GetRegion("bottom_right");
    ASSERT_NE(r2, nullptr);
    EXPECT_FLOAT_EQ(r2->u, 0.75f);
    EXPECT_FLOAT_EQ(r2->v, 0.75f);
    EXPECT_FLOAT_EQ(r2->u2, 1.0f);
    EXPECT_FLOAT_EQ(r2->v2, 1.0f);
}

TEST(TextureAtlasTest, UnknownRegionReturnsNull) {
    TextureAtlas atlas;
    const auto* region = atlas.GetRegion("nonexistent");
    EXPECT_EQ(region, nullptr);
}

TEST(TextureAtlasTest, OutOfBoundsFrameReturnsNull) {
    TextureAtlas atlas;
    atlas.LoadGrid(1, 2, 2);
    const auto* region = atlas.GetFrame(99);
    EXPECT_EQ(region, nullptr);
}

TEST(TextureAtlasTest, TextureIDRoundtrip) {
    TextureAtlas atlas;
    atlas.SetTextureID(42);
    EXPECT_EQ(atlas.GetTextureID(), 42u);

    atlas.LoadGrid(99, 2, 2);
    EXPECT_EQ(atlas.GetTextureID(), 99u);
}
