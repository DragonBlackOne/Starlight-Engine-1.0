#pragma once
#include <string>
#include <memory>
#include <vector>
#include <entt/entt.hpp>

namespace starlight {

    class Scene {
    public:
        Scene();
        virtual ~Scene() = default;
        virtual void OnEnter() = 0;
        virtual void OnUpdate(float dt) = 0;
        virtual void OnFixedUpdate(float dt) = 0;
        virtual void OnRender() = 0;
        virtual void OnUIRender() {}
        virtual void OnExit() = 0;

        entt::registry& GetRegistry() { return m_registry; }
        const entt::registry& GetRegistry() const { return m_registry; }

    protected:
        entt::registry m_registry;
    };

    class BaseScene : public Scene {
    public:
        void OnEnter() override {}
        void OnUpdate(float dt) override { (void)dt; }
        void OnFixedUpdate(float dt) override { (void)dt; }
        void OnRender() override {}
        void OnExit() override {}
    };



    class SceneStack {
    public:
        void Push(std::shared_ptr<Scene> scene) {
            if (!m_scenes.empty()) {
                m_scenes.back()->OnExit();
            }
            m_scenes.push_back(scene);
            scene->OnEnter();
        }

        void Pop() {
            if (!m_scenes.empty()) {
                m_scenes.back()->OnExit();
                m_scenes.pop_back();
                if (!m_scenes.empty()) {
                    m_scenes.back()->OnEnter();
                }
            }
        }

        std::shared_ptr<Scene> Active() const {
            return m_scenes.empty() ? nullptr : m_scenes.back();
        }

    private:
        std::vector<std::shared_ptr<Scene>> m_scenes;
    };

    struct ColorRGB {
        float r, g, b;
    };

    namespace Colors {
        inline constexpr ColorRGB HotMagenta    { 1.0f, 0.2f, 0.95f };
        inline constexpr ColorRGB CyberCyan     { 0.0f, 0.9f, 0.9f };
        inline constexpr ColorRGB NeonViolet    { 0.55f, 0.12f, 0.75f };
        inline constexpr ColorRGB SunsetOrange  { 1.0f, 0.45f, 0.05f };
        inline constexpr ColorRGB GlowGold      { 1.0f, 0.85f, 0.1f };
        inline constexpr ColorRGB DarkObsidian  { 0.04f, 0.02f, 0.08f };
    }
}
