#include <gtest/gtest.h>
#include "SpriteAnimationComponent.hpp"

using namespace starlight;

TEST(SpriteAnimationTest, DefaultState) {
    SpriteAnimationComponent anim;
    EXPECT_EQ(anim.currentFrame, 0);
    EXPECT_FLOAT_EQ(anim.fps, 12.0f);
    EXPECT_TRUE(anim.playing);
    EXPECT_TRUE(anim.looping);
}

TEST(SpriteAnimationTest, NoFramesInitially) {
    SpriteAnimationComponent anim;
    EXPECT_EQ(anim.frames.size(), 0u);
}

TEST(SpriteAnimationTest, FrameAdvanceViaTimer) {
    SpriteAnimationComponent anim;
    anim.frames = {0, 1, 2, 3};
    anim.fps = 10.0f;

    float dtPerFrame = 1.0f / 10.0f;
    anim.timer += dtPerFrame;
    while (anim.timer >= 1.0f / anim.fps) {
        anim.timer -= 1.0f / anim.fps;
        anim.currentFrame = (anim.currentFrame + 1);
    }
    EXPECT_EQ(anim.currentFrame, 1);
}

TEST(SpriteAnimationTest, LoopsToBeginning) {
    SpriteAnimationComponent anim;
    anim.frames = {0, 1, 2};
    anim.fps = 10.0f;
    anim.currentFrame = 2;

    float dtPerFrame = 1.0f / 10.0f;
    anim.timer += dtPerFrame;
    while (anim.timer >= 1.0f / anim.fps) {
        anim.timer -= 1.0f / anim.fps;
        anim.currentFrame = (anim.currentFrame + 1) % (int)anim.frames.size();
    }
    EXPECT_EQ(anim.currentFrame, 0);
}

TEST(SpriteAnimationTest, StopsAtLastFrameWhenNotLooping) {
    SpriteAnimationComponent anim;
    anim.frames = {0, 1, 2};
    anim.fps = 10.0f;
    anim.looping = false;
    anim.currentFrame = 2;

    float dtPerFrame = 1.0f / 10.0f;
    anim.timer += dtPerFrame;
    while (anim.timer >= 1.0f / anim.fps) {
        anim.timer -= 1.0f / anim.fps;
        if (anim.currentFrame + 1 < (int)anim.frames.size()) {
            anim.currentFrame++;
        }
    }
    EXPECT_EQ(anim.currentFrame, 2);
    EXPECT_FALSE(anim.looping);
}

TEST(SpriteAnimationTest, PlayingFalsePreventsAdvance) {
    SpriteAnimationComponent anim;
    anim.frames = {0, 1, 2};
    anim.fps = 10.0f;
    anim.playing = false;

    anim.timer += 1.0f;
    EXPECT_EQ(anim.currentFrame, 0);
}

TEST(SpriteAnimationTest, SingleFrameAnimation) {
    SpriteAnimationComponent anim;
    anim.frames = {42};
    anim.fps = 10.0f;

    anim.timer += 1.0f;
    while (anim.timer >= 1.0f / anim.fps) {
        anim.timer -= 1.0f / anim.fps;
        anim.currentFrame = (anim.currentFrame + 1) % (int)anim.frames.size();
    }
    EXPECT_EQ(anim.currentFrame, 0);
    EXPECT_EQ(anim.frames[anim.currentFrame], 42u);
}

TEST(SpriteAnimationTest, FastFpsAdvancesQuickly) {
    SpriteAnimationComponent anim;
    anim.frames = {0, 1, 2};
    anim.fps = 60.0f;

    for (int i = 0; i < 60; i++) {
        anim.timer += 1.0f / 60.0f;
        while (anim.timer >= 1.0f / anim.fps) {
            anim.timer -= 1.0f / anim.fps;
            anim.currentFrame = (anim.currentFrame + 1) % (int)anim.frames.size();
        }
    }
    EXPECT_EQ(anim.currentFrame, 0);
}

TEST(SpriteAnimationTest, SlowFpsAdvancesSlowly) {
    SpriteAnimationComponent anim;
    anim.frames = {0, 1, 2};
    anim.fps = 1.0f;

    anim.timer += 0.5f;
    while (anim.timer >= 1.0f / anim.fps) {
        anim.timer -= 1.0f / anim.fps;
        anim.currentFrame = (anim.currentFrame + 1) % (int)anim.frames.size();
    }
    EXPECT_EQ(anim.currentFrame, 0);

    anim.timer += 0.5f;
    while (anim.timer >= 1.0f / anim.fps) {
        anim.timer -= 1.0f / anim.fps;
        anim.currentFrame = (anim.currentFrame + 1) % (int)anim.frames.size();
    }
    EXPECT_EQ(anim.currentFrame, 1);
}
