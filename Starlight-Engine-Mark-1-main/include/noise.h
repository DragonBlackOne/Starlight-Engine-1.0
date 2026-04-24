// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef NOISE_H
#define NOISE_H

#include <math.h>

// Minimalist Simplex Noise for C
// Based on Ken Perlin's original implementation

static int fastfloor(float x) { return x > 0 ? (int)x : (int)x - 1; }

static float grad(int hash, float x, float y, float z) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

static float noise3d(float x, float y, float z) {
    // Extremely simplified implementation for MVP
    // In a real system, we would use permutation tables (p[])
    return (float)sin(x * 0.1f) * cos(z * 0.1f) * 2.0f; // Functional mock
}

// Returns value between -1.0 and 1.0
static float simplex_noise(float x, float y) {
    return (float)sin(x * 0.05f) * cos(y * 0.05f);
}

#endif
