#include <gtest/gtest.h>
#include "Noise.hpp"

using namespace starlight;

TEST(NoiseTest, PerlinReturnsConsistentValues) {
    Noise noise(42);
    float v1 = noise.Perlin(1.5f, 2.3f, 3.7f);
    float v2 = noise.Perlin(1.5f, 2.3f, 3.7f);
    EXPECT_FLOAT_EQ(v1, v2);
}

TEST(NoiseTest, DifferentSeedsProduceDifferentResults) {
    Noise noise1(1);
    Noise noise2(2);
    float v1 = noise1.Perlin(0.5f, 0.5f, 0.5f);
    float v2 = noise2.Perlin(0.5f, 0.5f, 0.5f);
    EXPECT_NE(v1, v2);
}

TEST(NoiseTest, PerlinRange) {
    Noise noise(1234);
    for (int x = -3; x <= 3; x++) {
        for (int y = -3; y <= 3; y++) {
            for (int z = -3; z <= 3; z++) {
                float v = noise.Perlin((float)x, (float)y, (float)z);
                EXPECT_GE(v, -1.0f) << "at (" << x << "," << y << "," << z << ")";
                EXPECT_LE(v, 1.0f) << "at (" << x << "," << y << "," << z << ")";
            }
        }
    }
}

TEST(NoiseTest, PerlinIntegerValuesAreZero) {
    Noise noise(1234);
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            for (int z = -2; z <= 2; z++) {
                float v = noise.Perlin((float)x, (float)y, (float)z);
                EXPECT_FLOAT_EQ(v, 0.0f) << "at (" << x << "," << y << "," << z << ")";
            }
        }
    }
}

TEST(NoiseTest, PerlinIsContinuous) {
    Noise noise(42);
    float v1 = noise.Perlin(1.0f, 2.0f, 3.0f);
    float v2 = noise.Perlin(1.001f, 2.0f, 3.0f);
    float diff = std::abs(v2 - v1);
    EXPECT_LT(diff, 0.01f);
}

TEST(NoiseTest, OctavePerlinReturnsConsistentValues) {
    Noise noise(42);
    float v1 = noise.OctavePerlin(1.5f, 2.3f, 3.7f, 4, 0.5f);
    float v2 = noise.OctavePerlin(1.5f, 2.3f, 3.7f, 4, 0.5f);
    EXPECT_FLOAT_EQ(v1, v2);
}

TEST(NoiseTest, OctavePerlinRange) {
    Noise noise(1234);
    for (int octaves = 1; octaves <= 6; octaves++) {
        float v = noise.OctavePerlin(0.3f, 0.7f, 0.2f, octaves, 0.5f);
        EXPECT_GE(v, -1.0f) << "with " << octaves << " octaves";
        EXPECT_LE(v, 1.0f) << "with " << octaves << " octaves";
    }
}

TEST(NoiseTest, SingleOctaveEqualsPerlin) {
    Noise noise(42);
    float perlin = noise.Perlin(2.5f, 3.5f, 4.5f);
    float octave = noise.OctavePerlin(2.5f, 3.5f, 4.5f, 1, 0.5f);
    EXPECT_FLOAT_EQ(perlin, octave);
}

TEST(NoiseTest, MoreOctavesAddDetail) {
    Noise noise(42);
    float v1 = noise.OctavePerlin(1.5f, 2.3f, 3.7f, 1, 0.5f);
    float v2 = noise.OctavePerlin(1.5f, 2.3f, 3.7f, 4, 0.5f);
    EXPECT_NE(v1, v2);
}

TEST(NoiseTest, PersistenceOfZero) {
    Noise noise(42);
    float v = noise.OctavePerlin(1.5f, 2.3f, 3.7f, 4, 0.0f);
    EXPECT_GE(v, -1.0f);
    EXPECT_LE(v, 1.0f);
}
