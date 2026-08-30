#pragma once
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <map>
#include "EngineSystem.hpp"
#include "PAL.hpp"
#include <sol/sol.hpp>

namespace starlight {

    struct PerformanceMetric {
        float avgFPS = 0.0f;
        float minFPS = 9999.0f;
        float maxFPS = 0.0f;
        float avgRenderTimeMs = 0.0f;
        float avgScriptTimeMs = 0.0f;
        float avgPhysicsTimeMs = 0.0f;
        size_t peakEntities = 0;
        size_t peakMemoryBytes = 0;
    };

    struct LuaTestResult {
        std::string name;
        bool success = false;
        std::string errorMessage;
    };

    class AutomationSystem : public ISystem {
    public:
        AutomationSystem();
        ~AutomationSystem() override = default;

        // ISystem
        bool OnInitialize(const EngineContext& context) override;
        void OnShutdown() override;
        void OnUpdate(float dt) override;
        void OnRender() override;
        void OnUIRender() override;
        const char* GetName() const override { return "AutomationSystem"; }
        bool IsMainThreadOnly() const override { return true; }

        // --- 1. C++ Unit/Functional Tests ---
        void RunCppTests();
        void RegisterCppTest(const std::string& name, std::function<bool()> testFunc);

        // --- 2. Lua Script Integration Tests ---
        bool RunLuaTestFile(const std::string& path);
        void RegisterLuaBindings(sol::state& lua);

        // --- 3. Visual Regression / Rendering Tests ---
        bool SaveScreenshotTGA(const std::string& path);
        bool CompareScreenshot(const std::string& baselinePath, const std::string& failureOutputPath, float thresholdPercent = 1.0f);

        // --- 4. Performance Benchmarking ---
        void StartBenchmark(float durationSeconds);
        bool IsBenchmarkRunning() const { return m_benchmarkRunning; }
        void StopBenchmarkAndReport(const std::string& reportPath);

        // --- 5. Input Simulation ---
        void SimulateKeyPress(pal::KeyCode key, float holdDurationSeconds);

    private:
        sol::state* m_luaState = nullptr;
        bool m_runningTests = false;
        bool m_testsExecuted = false;
        
        // Benchmarking state
        bool m_benchmarkRunning = false;
        float m_benchmarkTimer = 0.0f;
        float m_benchmarkDuration = 0.0f;
        std::vector<float> m_fpsHistory;
        std::vector<float> m_renderTimeHistory;
        std::vector<float> m_scriptTimeHistory;
        std::vector<float> m_physicsTimeHistory;
        std::vector<size_t> m_entityCountHistory;
        std::vector<size_t> m_memoryHistory;

        // Input simulation state
        struct QueuedKeyRelease {
            pal::KeyCode key;
            float timeRemaining;
        };
        std::vector<QueuedKeyRelease> m_queuedReleases;

        // Test registries
        std::map<std::string, std::function<bool()>> m_cppTests;
        std::map<std::string, bool> m_cppTestResults;
        std::vector<LuaTestResult> m_luaTestResults;
    };
}
