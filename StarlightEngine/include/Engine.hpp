// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#pragma once
#include <memory>
#include <vector>
#include "Window.hpp"

#include "CoreMinimal.hpp"
#include "wiJobSystem.h"

#include "EngineSystem.hpp"

namespace starlight {
    class Window;
    class SceneStack;
    class TweenSystem;
    class FileWatcher;
    class PhysicsSystem;
    class Renderer;
    class ScriptSystem;
    class InputSystem;
    class AudioSystem;
    class NavigationSystem;
    class NetworkSystem;
    class AssetManager;

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
        SystemRegistry& GetRegistry() { return m_systems; }
        SceneStack& GetSceneStack() { return m_sceneStack; }

        Window& GetWindow() { return *m_window; }
        const EngineTime& GetTime() const { return m_time; }

        // Legacy accessors
        PhysicsSystem& GetPhysics();
        Renderer& GetRenderer();
        ScriptSystem& GetScripting();
        InputSystem& GetInput();
        AudioSystem& GetAudio();
        TweenSystem& GetTweenSystem();
        NavigationSystem& GetNav();
        NetworkSystem& GetNetwork();
        FileWatcher& GetFileWatcher();
        AssetManager& GetAssetManager();

        // Helper accessors
        template<typename T> T* GetSystem() { return m_systems.GetSystem<T>(); }

        static Engine& Get() { return *s_instance; }
        bool IsConsoleVisible() const { return m_showConsole; }

    private:
        static Engine* s_instance;

        std::unique_ptr<Window> m_window;
        SystemRegistry m_systems;
        SceneStack m_sceneStack;
        
        EngineTime m_time;
        bool m_running = false;
        bool m_showConsole = false;

        wi::jobsystem::context m_physicsJobCtx;

        void Update(float dt);
        void FixedUpdate(float dt);
        void Render();
    };
}
