// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#include "Engine.hpp"
#include "Log.hpp"
#include "Renderer.hpp"
#include "InputSystem.hpp"
#include "ScriptSystem.hpp"
#include "AudioSystem.hpp"
#include "AISystem.hpp"
#include "NetworkSystem.hpp"
#include "NavigationSystem.hpp"
#include "Renderer2D.hpp"
#include "HierarchySystem.hpp"
#include "EventSystem.hpp"
#include "DashboardSystem.hpp"
#include "VehicleSystem.hpp"
#include "ClothSystem.hpp"
#include "SSAO_System.hpp"
#include "FileWatcher.hpp"
#include "CoreMinimal.hpp"
#include "CameraSystem.hpp"
#include "LODSystem.hpp"
#include "Memory.hpp"
#include "JobSystem.hpp"
#include "VFSSystem.hpp"
#include "AssetManager.hpp"
#include "GPUCullingSystem.hpp"
#include "AnimationSystem.hpp"
#include "Tween.hpp"
#include "PhysicsSystem.hpp"

#undef APIENTRY
#include <SDL2/SDL.h>
#include <thread>
#include <chrono>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

namespace starlight {

    Engine* Engine::s_instance = nullptr;

    Engine::Engine() {
        s_instance = this;
        m_window = std::make_unique<Window>();
        
        // Register Core Systems
        m_systems.RegisterSystem<InputSystem>();
        m_systems.RegisterSystem<PhysicsSystem>();
        m_systems.RegisterSystem<Renderer>();
        m_systems.RegisterSystem<AudioSystem>();
        m_systems.RegisterSystem<ScriptSystem>();
        
        // Extended Systems
        m_systems.RegisterSystem<JobSystem>();
        m_systems.RegisterSystem<VFSSystem>();
        m_systems.RegisterSystem<AssetManager>();
        m_systems.RegisterSystem<GPUCullingSystem>();
        m_systems.RegisterSystem<AnimationSystem>();
        m_systems.RegisterSystem<EventSystem>();
        m_systems.RegisterSystem<NetworkSystem>();
        m_systems.RegisterSystem<NavigationSystem>();
        m_systems.RegisterSystem<FileWatcher>();
        m_systems.RegisterSystem<TweenSystem>();
        m_systems.RegisterSystem<DashboardSystem>();
    }

    Engine::~Engine() {
        Shutdown();
    }

    void Engine::Initialize(const WindowConfig& config) {
        Log::Init();
        MemoryManager::Initialize();

        m_window->Initialize(config);
        
        EngineContext context;
        context.window = m_window.get();
        context.engine = this;

        // Initialize all registered systems
        for (auto& system : m_systems.GetSystems()) {
            if (!system->OnInitialize(context)) {
                Log::Error("Failed to initialize system: " + std::string(system->GetName()));
            }
        }

        // Legacy/Static Initializations
        Renderer2D::Initialize();
        SSAO_System::Get().Initialize();
        
        m_sceneStack.Push(std::make_shared<BaseScene>());
        
        auto scripting = GetSystem<ScriptSystem>();
        if (scripting) {
            scripting->ExecuteFile("assets/scripts/starlight_odyssey.lua");
        }

        auto fileWatcher = GetSystem<FileWatcher>();
        if (fileWatcher) {
            fileWatcher->AddWatch("assets/scripts/starlight_odyssey.lua", [this](const std::string& path) {
                auto s = GetSystem<ScriptSystem>();
                if (s) s->ExecuteFile(path);
                Log::Info("Engine: Hot-Reloaded Odyssey script!");
            });
        }
        
        // Initialize Default Projection
        float aspect = (float)m_window->GetWidth() / (float)m_window->GetHeight();
        auto renderer = GetSystem<Renderer>();
        if (renderer) {
            renderer->UpdateProjection(60.0f, aspect, 0.1f, 1000.0f);
        }

        Log::Info("Starlight Engine CORE Reconstruction Complete (Modular Mode).");
    }

    PhysicsSystem& Engine::GetPhysics() { return *GetSystem<PhysicsSystem>(); }
    Renderer& Engine::GetRenderer() { return *GetSystem<Renderer>(); }
    ScriptSystem& Engine::GetScripting() { return *GetSystem<ScriptSystem>(); }
    InputSystem& Engine::GetInput() { return *GetSystem<InputSystem>(); }
    AudioSystem& Engine::GetAudio() { return *GetSystem<AudioSystem>(); }
    TweenSystem& Engine::GetTweenSystem() { return *GetSystem<TweenSystem>(); }
    NavigationSystem& Engine::GetNav() { return *GetSystem<NavigationSystem>(); }
    NetworkSystem& Engine::GetNetwork() { return *GetSystem<NetworkSystem>(); }
    FileWatcher& Engine::GetFileWatcher() { return *GetSystem<FileWatcher>(); }
    AssetManager& Engine::GetAssetManager() { return *GetSystem<AssetManager>(); }

    void Engine::Run() {
        m_running = true;
        
        using clock = std::chrono::high_resolution_clock;
        auto lastTime = clock::now();
        double accumulator = 0.0;

        while (m_running && !m_window->ShouldClose()) {
            auto currentTime = clock::now();
            std::chrono::duration<double> diff = currentTime - lastTime;
            double deltaTime = diff.count();
            lastTime = currentTime;

            if (deltaTime > 0.25) deltaTime = 0.25;

            m_time.deltaTime = (float)deltaTime;
            m_time.totalTime += (float)deltaTime;

            accumulator += deltaTime;

            m_window->PollEvents();

            while (accumulator >= m_time.fixedDeltaTime) {
                FixedUpdate((float)m_time.fixedDeltaTime);
                accumulator -= m_time.fixedDeltaTime;
            }

            Update((float)deltaTime);
            Render();
            
            MemoryManager::ClearFrame();
            
            if (m_window->ShouldClose()) Log::Info("Engine: Window requested close.");
        }
        Log::Info("Engine: Main loop terminated.");
    }

    void Engine::Update(float dt) {
        // Update registered systems
        for (auto& system : m_systems.GetSystems()) {
            system->OnUpdate(dt);
        }

        // Tween flush
        // (Assuming TweenSystem is now a registered system or handled inside one)        
        auto activeScene = m_sceneStack.Active();
        if (activeScene) {
            VehicleSystem::Update(activeScene->GetRegistry(), dt); 
            ClothSystem::Update(activeScene->GetRegistry(), dt);   
            AISystem::Update(activeScene->GetRegistry(), dt);
            activeScene->OnUpdate(dt);
            HierarchySystem::Update(activeScene->GetRegistry());
            
            float aspect = (float)m_window->GetWidth() / (float)m_window->GetHeight();
            CameraSystem::Update(activeScene->GetRegistry(), aspect);
            LODSystem::Update(activeScene->GetRegistry());
        }
    }

    void Engine::FixedUpdate(float dt) {
        // Registered systems fixed update
        for (auto& system : m_systems.GetSystems()) {
            system->OnFixedUpdate(dt);
        }

        auto activeScene = m_sceneStack.Active();
        if (activeScene) {
            activeScene->OnFixedUpdate(dt);
        }
    }

    void Engine::Render() {
        // ImGui Frame Setup
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // System Rendering
        for (auto& system : m_systems.GetSystems()) {
            system->OnRender();
        }

        // Active Scene ImGui Rendering
        auto activeScene = m_sceneStack.Active();
        if (activeScene) {
            // activeScene->OnImGuiRender(); // Now handled by UIPass
        }

        // Finalize ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        m_window->SwapBuffers();
    }

    void Engine::Shutdown() {
        Log::Info("Starlight Engine Shutting Down...");
        m_systems.Shutdown();
        m_window->Shutdown();
    }
}
