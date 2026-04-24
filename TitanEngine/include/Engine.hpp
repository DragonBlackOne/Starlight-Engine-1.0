// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <memory>
#include <vector>
#include "Window.hpp"
#include "PhysicsSystem.hpp"
#include "Renderer.hpp"
#include "CoreMinimal.hpp"
#include "Tween.hpp"
#include "wiJobSystem.h"

namespace titan {
    class Window;
    class PhysicsSystem;
    class Renderer;
    class ScriptSystem;
    class InputSystem;
    class AudioSystem;
    class SceneStack;
    class TweenSystem;
    class NetworkSystem;
    class NavigationSystem;
    class EditorSystem;
    class FileWatcher;

    struct EngineTime {
        float deltaTime = 0.0f;
        float totalTime = 0.0f;
        float timeScale = 1.0f;
        const float fixedDeltaTime = 1.0f / 60.0f;
    };

    class Engine {
    public:
        Engine();
        ~Engine();

        void Initialize(const WindowConfig& config);
        void Run();
        void Shutdown();

        // Modularity
        void AddModule(std::shared_ptr<EngineModule> module);
        SceneStack& GetSceneStack() { return m_sceneStack; }

        Window& GetWindow() { return *m_window; }
        PhysicsSystem& GetPhysics() { return *m_physics; }
        Renderer& GetRenderer() { return *m_renderer; }
        ScriptSystem& GetScripting() { return *m_scripting; }
        InputSystem& GetInput() { return *m_input; }
        AudioSystem& GetAudio() { return *m_audio; }
        TweenSystem& GetTweenSystem() { return m_tweenSystem; }
        NetworkSystem& GetNetwork() { return *m_network; }
        NavigationSystem& GetNav() { return *m_nav; }
        EditorSystem& GetEditor() { return *m_editor; }
        FileWatcher& GetFileWatcher() { return *m_fileWatcher; }
        const EngineTime& GetTime() const { return m_time; }

        static Engine& Get() { return *s_instance; }

    private:
        static Engine* s_instance;

        std::unique_ptr<Window> m_window;
        std::unique_ptr<PhysicsSystem> m_physics;
        std::unique_ptr<Renderer> m_renderer;
        std::unique_ptr<ScriptSystem> m_scripting;
        std::unique_ptr<InputSystem> m_input;
        std::unique_ptr<AudioSystem> m_audio;
        std::unique_ptr<NetworkSystem> m_network;
        std::unique_ptr<NavigationSystem> m_nav;
        std::unique_ptr<EditorSystem> m_editor;
        std::unique_ptr<FileWatcher> m_fileWatcher;
        
        SceneStack m_sceneStack;
        std::vector<std::shared_ptr<EngineModule>> m_modules;
        TweenSystem m_tweenSystem;

        EngineTime m_time;
        bool m_running = false;

        wi::jobsystem::context m_physicsJobCtx;
        wi::jobsystem::context m_generalJobCtx;

        void Update(float dt);
        void FixedUpdate(float dt);
        void Render();
    };
}
