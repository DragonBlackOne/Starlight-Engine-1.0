#pragma once
#include <string>
#include <memory>
#include <vector>
#include <entt/entt.hpp>

namespace titan {

    class Scene {
    public:
        virtual ~Scene() = default;
        virtual void OnEnter() = 0;
        virtual void OnUpdate(float dt) = 0;
        virtual void OnFixedUpdate(float dt) = 0;
        virtual void OnRender() = 0;
        virtual void OnExit() = 0;

        entt::registry& GetRegistry() { return m_registry; }

    protected:
        entt::registry m_registry;
    };

    class EngineModule {
    public:
        virtual ~EngineModule() = default;
        virtual void Initialize() = 0;
        virtual void Update(float dt) = 0;
        virtual void Render() {}
        virtual void RenderUI() {}  // Called inside ImGui frame context
        virtual void Shutdown() = 0;
        virtual std::string GetName() const = 0;
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
