// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - Procedural Generation (FastNoiseLite)
// Licença: MIT | Origem: Auburn/FastNoiseLite
// ============================================================

#include "FastNoiseLite/Cpp/FastNoiseLite.h"
#include <vector>

namespace Vamos {
    class ProceduralNoise {
    public:
        enum class NoiseType {
            Perlin, Simplex, Cellular, Value, ValueCubic
        };

        ProceduralNoise(int seed = 1337) {
            noise.SetSeed(seed);
            noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
            noise.SetFrequency(0.01f);
        }

        void SetType(NoiseType type) {
            switch (type) {
                case NoiseType::Perlin:    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin); break;
                case NoiseType::Simplex:   noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2); break;
                case NoiseType::Cellular:  noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular); break;
                case NoiseType::Value:     noise.SetNoiseType(FastNoiseLite::NoiseType_Value); break;
                case NoiseType::ValueCubic:noise.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic); break;
            }
        }

        void SetFrequency(float freq) { noise.SetFrequency(freq); }
        void SetOctaves(int octaves) { noise.SetFractalOctaves(octaves); }
        void SetLacunarity(float lac) { noise.SetFractalLacunarity(lac); }
        void SetGain(float gain) { noise.SetFractalGain(gain); }

        void EnableFractal(int octaves = 5) {
            noise.SetFractalType(FastNoiseLite::FractalType_FBm);
            noise.SetFractalOctaves(octaves);
        }

        float Get2D(float x, float y) { return noise.GetNoise(x, y); }
        float Get3D(float x, float y, float z) { return noise.GetNoise(x, y, z); }

        // Gerar heightmap 2D
        std::vector<float> GenerateHeightmap(int width, int height, float offsetX = 0, float offsetY = 0) {
            std::vector<float> map(width * height);
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    map[y * width + x] = noise.GetNoise(x + offsetX, y + offsetY);
                }
            }
            return map;
        }

    private:
        FastNoiseLite noise;
    };
}
