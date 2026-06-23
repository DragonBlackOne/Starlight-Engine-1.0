#pragma once
#include <cmath>
#include <functional>
#include <vector>
#include <memory>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "EngineSystem.hpp"

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

        static std::function<float(float)> GetEaseFunc(const std::string& name) {
            if (name == "inQuad") return InQuad;
            if (name == "outQuad") return OutQuad;
            if (name == "inOutQuad") return InOutQuad;
            if (name == "inCubic") return InCubic;
            if (name == "outCubic") return OutCubic;
            if (name == "inOutCubic") return InOutCubic;
            if (name == "inExpo") return InExpo;
            if (name == "outExpo") return OutExpo;
            if (name == "outElastic") return OutElastic;
            if (name == "outBounce") return OutBounce;
            return Linear;
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
            float progress = (duration <= 0.0f) ? 1.0f : (elapsed / duration);
            if (progress >= 1.0f) {
                progress = 1.0f;
                active = false;
            }
            float eased = easeFunc(progress);
            *target = startValue + (endValue - startValue) * eased;
        }
    };

    struct EcsTween {
        enum class Type { Position, Scale };

        entt::entity entity;
        Type type;
        glm::vec3 startValue;
        glm::vec3 endValue;
        float duration;
        float elapsed;
        std::function<float(float)> easeFunc;
        bool active = true;

        void Start(entt::entity _entity, Type _type, const glm::vec3& _from, const glm::vec3& _to, float _duration, std::function<float(float)> _ease = Easing::Linear) {
            entity = _entity;
            type = _type;
            startValue = _from;
            endValue = _to;
            duration = _duration;
            elapsed = 0.0f;
            easeFunc = _ease;
            active = true;
        }
    };

    class TweenSystem : public ISystem {
    public:
        // ISystem
        bool OnInitialize(const EngineContext& context) override { (void)context; return true; }
        void OnShutdown() override {}
        void OnUpdate(float dt) override;
        const char* GetName() const override { return "TweenSystem"; }
        bool IsMainThreadOnly() const override { return true; }

        void Update(float dt);
        void Update(float dt, entt::registry& reg);

        void AddTween(const Tween& tween) {
            m_tweens.push_back(tween);
        }

        void AddEcsTween(const EcsTween& ecsTween) {
            m_ecsTweens.push_back(ecsTween);
        }

    private:
        std::vector<Tween> m_tweens;
        std::vector<EcsTween> m_ecsTweens;
    };
}

// Inline implementation to avoid circular dependencies
#include "Engine.hpp"
#include "Components.hpp"

namespace starlight {
    inline void TweenSystem::OnUpdate(float dt) {
        Update(dt);
    }

    inline void TweenSystem::Update(float dt) {
        // Update standard target pointer tweens
        for (auto it = m_tweens.begin(); it != m_tweens.end();) {
            it->Update(dt);
            if (!it->active) {
                it = m_tweens.erase(it);
            } else {
                ++it;
            }
        }

        // Update ECS registry-safe tweens
        if (Engine::IsInitialized()) {
            auto activeScene = Engine::Get().GetSceneStack().Active();
            if (activeScene) {
                Update(dt, activeScene->GetRegistry());
            }
        }
    }

    inline void TweenSystem::Update(float dt, entt::registry& reg) {
        for (auto it = m_ecsTweens.begin(); it != m_ecsTweens.end();) {
            if (!reg.valid(it->entity) || !reg.all_of<TransformComponent>(it->entity)) {
                it = m_ecsTweens.erase(it);
                continue;
            }

            it->elapsed += dt;
            float progress = (it->duration <= 0.0f) ? 1.0f : (it->elapsed / it->duration);
            if (progress >= 1.0f) {
                progress = 1.0f;
                it->active = false;
            }

            float eased = it->easeFunc(progress);
            glm::vec3 current = it->startValue + (it->endValue - it->startValue) * eased;

            auto& trans = reg.get<TransformComponent>(it->entity);
            if (it->type == EcsTween::Type::Position) {
                trans.position = current;
            } else if (it->type == EcsTween::Type::Scale) {
                trans.scale = current;
            }
            trans.UpdateLocalMatrix();

            if (!it->active) {
                it = m_ecsTweens.erase(it);
            } else {
                ++it;
            }
        }
    }
}
