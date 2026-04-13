#pragma once
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <cmath>

namespace titan {

    // Industrial Grade Perlin Noise Implementation
    class Noise {
    public:
        Noise(unsigned int seed = 1234) {
            p.resize(256);
            std::iota(p.begin(), p.end(), 0);
            std::default_random_engine engine(seed);
            std::shuffle(p.begin(), p.end(), engine);
            p.insert(p.end(), p.begin(), p.end());
        }

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

    private:
        std::vector<int> p;

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
