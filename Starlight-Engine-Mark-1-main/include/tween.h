#ifndef TWEEN_H
#define TWEEN_H

#include <math.h>

// --- Framework Overhaul: Tween & Easing Library ---
// Zero-allocation, header-only C11 easing functions.
// Usage: float t = slf_ease_out_bounce(progress); // progress[0..1] -> result[0..1]

#include <math.h>

// Linear (identity)
static inline float slf_ease_linear(float t) { return t; }

// Quadratic
static inline float slf_ease_in_quad(float t) { return t * t; }
static inline float slf_ease_out_quad(float t) { return t * (2.0f - t); }
static inline float slf_ease_in_out_quad(float t) { return t < 0.5f ? 2*t*t : -1+(4-2*t)*t; }

// Cubic
static inline float slf_ease_in_cubic(float t) { return t * t * t; }
static inline float slf_ease_out_cubic(float t) { float f = t - 1.0f; return f*f*f + 1.0f; }
static inline float slf_ease_in_out_cubic(float t) { return t < 0.5f ? 4*t*t*t : (t-1)*(2*t-2)*(2*t-2)+1; }

// Exponential
static inline float slf_ease_in_expo(float t) { return t == 0.0f ? 0.0f : powf(2, 10*(t-1)); }
static inline float slf_ease_out_expo(float t) { return t == 1.0f ? 1.0f : 1.0f - powf(2, -10*t); }

// Elastic
static inline float slf_ease_out_elastic(float t) {
    if (t == 0.0f || t == 1.0f) return t;
    return powf(2, -10*t) * sinf((t - 0.075f) * (2.0f*(float)M_PI) / 0.3f) + 1.0f;
}

// Back (overshoot)
static inline float slf_ease_in_back(float t) { 
    float s = 1.70158f; 
    return t * t * ((s+1)*t - s); 
}
static inline float slf_ease_out_back(float t) {
    float s = 1.70158f; 
    t -= 1.0f; 
    return t*t*((s+1)*t + s) + 1.0f; 
}

// Bounce
static inline float slf_ease_out_bounce(float t) {
    if (t < 1.0f/2.75f) return 7.5625f*t*t;
    else if (t < 2.0f/2.75f) { t -= 1.5f/2.75f; return 7.5625f*t*t + 0.75f; }
    else if (t < 2.5f/2.75f) { t -= 2.25f/2.75f; return 7.5625f*t*t + 0.9375f; }
    else { t -= 2.625f/2.75f; return 7.5625f*t*t + 0.984375f; }
}

// --- Tween Animator (Optional runtime helper) ---
typedef struct {
    float* target;       // Pointer to the value being animated
    float  start_value;
    float  end_value;
    float  duration;
    float  elapsed;
    float (*ease_func)(float t);
    int    active;
} SLF_Tween;

static inline void slf_tween_start(SLF_Tween* tw, float* target, float from, float to, float duration, float(*ease)(float)) {
    tw->target = target;
    tw->start_value = from;
    tw->end_value = to;
    tw->duration = duration;
    tw->elapsed = 0.0f;
    tw->ease_func = ease;
    tw->active = 1;
}

static inline void slf_tween_update(SLF_Tween* tw, float dt) {
    if (!tw->active) return;
    tw->elapsed += dt;
    float progress = tw->elapsed / tw->duration;
    if (progress >= 1.0f) { progress = 1.0f; tw->active = 0; }
    float eased = tw->ease_func(progress);
    *tw->target = tw->start_value + (tw->end_value - tw->start_value) * eased;
}

#endif
