#pragma once
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <cmath>
#include "FastNoiseLite.h"

namespace starlight {

    // Industrial Grade Perlin Noise Implementation extended with FastNoiseLite
    class Noise {
    public:
        Noise(unsigned int seed = 1234) {
            // Original Perlin noise seed initialization for backward compatibility
            p.resize(256);
            std::iota(p.begin(), p.end(), 0);
            std::default_random_engine engine(seed);
            std::shuffle(p.begin(), p.end(), engine);
            p.insert(p.end(), p.begin(), p.end());

            // Initialize FastNoiseLite
            m_fnl.SetSeed(static_cast<int>(seed));
            m_fnl.SetFrequency(0.01f);
            m_fnl.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        }

        // Classic Perlin (for backward compatibility and tests)
        float Perlin(float x, float y, float z) const {
            int X = (int)std::floor(x) & 255;
            int Y = (int)std::floor(y) & 255;
            int Z = (int)std::floor(z) & 255;

            x -= std::floor(x);
            y -= std::floor(y);
            z -= std::floor(z);

            float u = Fade(x);
            float v = Fade(y);
            float w = Fade(z);

            int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
            int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

            return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
                Grad(p[BA], x - 1, y, z)),
                Lerp(u, Grad(p[AB], x, y - 1, z),
                    Grad(p[BB], x - 1, y - 1, z))),
                Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1),
                    Grad(p[BA + 1], x - 1, y, z - 1)),
                    Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
                        Grad(p[BB + 1], x - 1, y - 1, z - 1))));
        }

        float OctavePerlin(float x, float y, float z, int octaves, float persistence) const {
            float total = 0;
            float frequency = 1;
            float amplitude = 1;
            float maxValue = 0;
            for (int i = 0; i < octaves; i++) {
                total += Perlin(x * frequency, y * frequency, z * frequency) * amplitude;
                maxValue += amplitude;
                amplitude *= persistence;
                frequency *= 2;
            }
            return total / maxValue;
        }

        // Modern FastNoiseLite API
        float Simplex(float x, float y) const {
            m_fnl.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            return m_fnl.GetNoise(x, y);
        }

        float Simplex(float x, float y, float z) const {
            m_fnl.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            return m_fnl.GetNoise(x, y, z);
        }

        float Cellular(float x, float y) const {
            m_fnl.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
            return m_fnl.GetNoise(x, y);
        }

        float Cellular(float x, float y, float z) const {
            m_fnl.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
            return m_fnl.GetNoise(x, y, z);
        }

        float Value(float x, float y) const {
            m_fnl.SetNoiseType(FastNoiseLite::NoiseType_Value);
            return m_fnl.GetNoise(x, y);
        }

        float Value(float x, float y, float z) const {
            m_fnl.SetNoiseType(FastNoiseLite::NoiseType_Value);
            return m_fnl.GetNoise(x, y, z);
        }

        float GetNoise(float x, float y) const {
            return m_fnl.GetNoise(x, y);
        }

        float GetNoise(float x, float y, float z) const {
            return m_fnl.GetNoise(x, y, z);
        }

        void SetSeed(int seed) { m_fnl.SetSeed(seed); }
        void SetFrequency(float freq) { m_fnl.SetFrequency(freq); }
        
        // 0=None, 1=FBm, 2=Ridged, 3=PingPong
        void SetFractalType(int type) { m_fnl.SetFractalType(static_cast<FastNoiseLite::FractalType>(type)); }
        void SetFractalOctaves(int octaves) { m_fnl.SetFractalOctaves(octaves); }
        void SetFractalGain(float gain) { m_fnl.SetFractalGain(gain); }
        void SetFractalLacunarity(float lacunarity) { m_fnl.SetFractalLacunarity(lacunarity); }

    private:
        std::vector<int> p;
        mutable FastNoiseLite m_fnl;

        static float Fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
        static float Lerp(float t, float a, float b) { return a + t * (b - a); }
        static float Grad(int hash, float x, float y, float z) {
            int h = hash & 15;
            float u = h < 8 ? x : y;
            float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
            return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
        }
    };

}
