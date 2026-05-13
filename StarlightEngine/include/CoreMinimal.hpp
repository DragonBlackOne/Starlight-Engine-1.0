// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <string>
#include <memory>
#include <vector>
#include <entt/entt.hpp>

namespace starlight {

    class Scene {
    public:
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
}
