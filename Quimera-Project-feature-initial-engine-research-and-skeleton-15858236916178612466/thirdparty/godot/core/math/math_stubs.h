// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <assert.h>

#define _FORCE_INLINE_ inline
#define DEV_ASSERT(cond) assert(cond)
#define ERR_FAIL_COND_V_MSG(cond, ret, msg) if(cond) return ret;
#define likely(x) (x)
#define unlikely(x) (x)

typedef float real_t;

namespace Math {
    _FORCE_INLINE_ float sqrt(float v) { return std::sqrt(v); }
    _FORCE_INLINE_ float abs(float v) { return std::abs(v); }
    _FORCE_INLINE_ float floor(float v) { return std::floor(v); }
    _FORCE_INLINE_ float ceil(float v) { return std::ceil(v); }
    _FORCE_INLINE_ float round(float v) { return std::round(v); }
    _FORCE_INLINE_ float atan2(float y, float x) { return std::atan2(y, x); }
    _FORCE_INLINE_ float fposmod(float x, float y) { return x - y * std::floor(x / y); }
    _FORCE_INLINE_ float lerp(float a, float b, float t) { return a + (b - a) * t; }

    // Simplificações para o momento
    inline bool is_equal_approx(float a, float b, float eps = 0.00001f) { return std::abs(a - b) < eps; }

    // Mais funções podem ser adicionadas conforme necessário
    template<typename T>
    _FORCE_INLINE_ T cubic_interpolate(T p_y0, T p_y1, T p_y2, T p_y3, float p_t) {
        float t2 = p_t * p_t;
        float t3 = t2 * p_t;
        return 0.5f * ((2.0f * p_y1) + (-p_y0 + p_y2) * p_t + (2.0f * p_y0 - 5.0f * p_y1 + 4.0f * p_y2 - p_y3) * t2 + (-p_y0 + 3.0f * p_y1 - 3.0f * p_y2 + p_y3) * t3);
    }

    template<typename T>
    _FORCE_INLINE_ T cubic_interpolate_in_time(T p_y0, T p_y1, T p_y2, T p_y3, float p_t, float p_t1, float p_t0, float p_t2) {
        // Fallback simples
        return cubic_interpolate(p_y0, p_y1, p_y2, p_y3, p_t);
    }

    _FORCE_INLINE_ float bezier_interpolate(float p_start, float p_control_1, float p_control_2, float p_end, float p_t) {
        float t = p_t;
        float t2 = t * t;
        float t3 = t2 * t;
        float mt = 1.0f - t;
        float mt2 = mt * mt;
        float mt3 = mt2 * mt;
        return p_start * mt3 + 3.0f * p_control_1 * mt2 * t + 3.0f * p_control_2 * mt * t2 + p_end * t3;
    }

    _FORCE_INLINE_ float bezier_derivative(float p_start, float p_control_1, float p_control_2, float p_end, float p_t) {
        float t = p_t;
        float mt = 1.0f - t;
        float mt2 = mt * mt;
        float t2 = t * t;
        return -3.0f * p_start * mt2 + 3.0f * p_control_1 * (mt2 - 2.0f * t * mt) + 3.0f * p_control_2 * (2.0f * t * mt - t2) + 3.0f * p_end * t2;
    }
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(m_v, m_min, m_max) (((m_v) < (m_min)) ? (m_min) : (((m_v) > (m_max)) ? (m_max) : (m_v)))
#define SIGN(m_v) ((m_v) < 0 ? -1 : 1)

#define UNIT_EPSILON 0.00001

struct String {
    String() {}
    explicit String(const char* s) {}
};

template<typename T>
struct is_zero_constructible : std::false_type {};

inline uint32_t hash_murmur3_one_real(float p_real, uint32_t p_prev = 0) { return p_prev; }
inline uint32_t hash_fmix32(uint32_t h) { return h; }
