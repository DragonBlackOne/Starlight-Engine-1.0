#pragma once
#include <memory>
#include <vector>
#include "Window.hpp"
#include "Version.hpp"

#include "CoreMinimal.hpp"
#include "wicked/core/wiJobSystem.h"
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
class ConfigSystem;
class SystemScheduler;
class JobSystem;
class DecalSystem;

struct EngineTime {
    float deltaTime = 0.0f;
    float totalTime = 0.0f;
    float timeScale = 1.0f;
    const float fixedDeltaTime = 1.0f / 60.0f;
    float fps = 0.0f;
    float avgFrameTime = 0.0f;
};

struct ProfilerStats {
    float physicsTime = 0.0f; // ms
    float scriptTime = 0.0f;  // ms
    float audioTime = 0.0f;   // ms
    float renderTime = 0.0f;  // ms
    float updateTime = 0.0f;  // ms (total de updates excluindo render e fisica)
};

class Engine {
public:
    Engine();
    ~Engine();

    bool Initialize(const WindowConfig& config);
    void Run();
    void Shutdown();

    // Version API (v10.0.0)
    static constexpr const char* GetEngineName() { return STARLIGHT_ENGINE_NAME; }
    static constexpr const char* GetVersionString() { return STARLIGHT_VERSION_STRING; }
    static constexpr const char* GetVersionCodename() { return STARLIGHT_VERSION_CODENAME; }
    static constexpr int GetVersionMajor() { return STARLIGHT_VERSION_MAJOR; }
    static constexpr int GetVersionMinor() { return STARLIGHT_VERSION_MINOR; }
    static constexpr int GetVersionPatch() { return STARLIGHT_VERSION_PATCH; }
    static constexpr int GetVersionNumber() { return STARLIGHT_VERSION_NUMBER; }

    // Modularity
    SystemRegistry& GetRegistry() {
        return m_systems;
    }
    SceneStack& GetSceneStack() {
        return m_sceneStack;
    }

    Window& GetWindow() {
        return *m_window;
    }
    const EngineTime& GetTime() const {
        return m_time;
    }

    // Profiler API
    const ProfilerStats& GetProfilerStats() const {
        return m_profilerStats;
    }
    void AccumulateScriptTime(float ms) {
        m_frameScriptTime += ms;
    }
    void AccumulateAudioTime(float ms) {
        m_frameAudioTime += ms;
    }

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
    ConfigSystem& GetConfig();

    void SetTimeScale(float scale) {
        m_time.timeScale = scale;
    }

    void RequestQuit() {
        m_running = false;
    }
    bool IsPaused() const {
        return m_paused;
    }
    void SetPaused(bool paused) {
        m_paused = paused;
    }
    void SetMaxFPS(int maxFPS) {
        m_maxFPS = maxFPS;
    }
    int GetMaxFPS() const {
        return m_maxFPS;
    }

    // Helper accessors
    template<typename T>
    T* GetSystem() {
        return m_systems.GetSystem<T>();
    }

    static Engine& Get() {
        return *s_instance;
    }
    static bool IsInitialized() {
        return s_instance != nullptr;
    }
private:
    static Engine* s_instance;

    std::unique_ptr<Window> m_window;
    SystemRegistry m_systems;
    std::unique_ptr<SystemScheduler> m_scheduler;
    SceneStack m_sceneStack;

    EngineTime m_time;
    ProfilerStats m_profilerStats;

    // CPU frame profiler variables
    float m_framePhysicsTime = 0.0f;
    float m_frameScriptTime = 0.0f;
    float m_frameAudioTime = 0.0f;
    float m_frameRenderTime = 0.0f;
    float m_frameUpdateTime = 0.0f;

    bool m_running = false;
    bool m_shutdown = false;
    bool m_paused = false;
    int m_maxFPS = 0;

    wi::jobsystem::context m_physicsJobCtx;

    void Update(float dt);
    void FixedUpdate(float dt);
    void Render();
};
}  // namespace starlight
