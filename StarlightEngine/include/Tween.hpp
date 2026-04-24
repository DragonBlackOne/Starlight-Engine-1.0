// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <cmath>
#include <functional>
#include <vector>
#include <memory>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace starlight {

    /**
     * @brief Easing functions ported from Starlight Engine (Mark-C)
     */
    class Easing {
    public:
        static float Linear(float t) { return t; }
        
        static float InQuad(float t) { return t * t; }
        static float OutQuad(float t) { return t * (2.0f - t); }
        static float InOutQuad(float t) { return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t; }

        static float InCubic(float t) { return t * t * t; }
        static float OutCubic(float t) { float f = t - 1.0f; return f * f * f + 1.0f; }
        static float InOutCubic(float t) { return t < 0.5f ? 4.0f * t * t * t : (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f; }

        static float InExpo(float t) { return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * (t - 1.0f)); }
        static float OutExpo(float t) { return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t); }

        static float OutElastic(float t) {
            if (t == 0.0f || t == 1.0f) return t;
            return std::pow(2.0f, -10.0f * t) * std::sin((t - 0.075f) * (2.0f * (float)M_PI) / 0.3f) + 1.0f;
        }

        static float OutBounce(float t) {
            if (t < 1.0f / 2.75f) return 7.5625f * t * t;
            else if (t < 2.0f / 2.75f) { t -= 1.5f / 2.75f; return 7.5625f * t * t + 0.75f; }
            else if (t < 2.5f / 2.75f) { t -= 2.25f / 2.75f; return 7.5625f * t * t + 0.9375f; }
            else { t -= 2.625f / 2.75f; return 7.5625f * t * t + 0.984375f; }
        }
    };

    struct Tween {
        float* target;
        float startValue;
        float endValue;
        float duration;
        float elapsed;
        std::function<float(float)> easeFunc;
        bool active = false;

        void Start(float* _target, float _from, float _to, float _duration, std::function<float(float)> _ease = Easing::Linear) {
            target = _target;
            startValue = _from;
            endValue = _to;
            duration = _duration;
            elapsed = 0.0f;
            easeFunc = _ease;
            active = true;
        }

        void Update(float dt) {
            if (!active || !target) return;
            elapsed += dt;
            float progress = elapsed / duration;
            if (progress >= 1.0f) {
                progress = 1.0f;
                active = false;
            }
            float eased = easeFunc(progress);
            *target = startValue + (endValue - startValue) * eased;
        }
    };

    class TweenSystem {
    public:
        void AddTween(const Tween& tween) {
            m_tweens.push_back(tween);
        }

        void Update(float dt) {
            for (auto it = m_tweens.begin(); it != m_tweens.end();) {
                it->Update(dt);
                if (!it->active) {
                    it = m_tweens.erase(it);
                } else {
                    ++it;
                }
            }
        }

    private:
        std::vector<Tween> m_tweens;
    };
}
