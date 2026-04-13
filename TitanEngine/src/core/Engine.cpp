#include "Engine.hpp"
#include "Log.hpp"
#include "Renderer.hpp"
#include "InputSystem.hpp"
#include "ScriptSystem.hpp"
#include "TitanAudio.hpp"
#include "AISystem.hpp"
#include "NetworkSystem.hpp"
#include "NavigationSystem.hpp"
#include "EditorSystem.hpp"
#include "HierarchySystem.hpp"
#include "EventSystem.hpp"
#include "VehicleSystem.hpp"
#include "ClothSystem.hpp"
#include "SSAO_System.hpp"
#include "FileWatcher.hpp"
#include "CoreMinimal.hpp"
#include <SDL2/SDL.h>
#include <thread>

namespace titan {

    Engine* Engine::s_instance = nullptr;

    Engine::Engine() {
        s_instance = this;
        m_window = std::make_unique<Window>();
        m_physics = std::make_unique<PhysicsSystem>();
        m_renderer = std::make_unique<Renderer>();
        m_scripting = std::make_unique<ScriptSystem>();
        m_input = std::make_unique<InputSystem>();
        m_audio = std::make_unique<AudioSystem>();
        m_network = std::make_unique<NetworkSystem>();
        m_nav = std::make_unique<NavigationSystem>();
        m_editor = std::unique_ptr<EditorSystem>(&EditorSystem::Get());
        m_fileWatcher = std::make_unique<FileWatcher>();
    }

    Engine::~Engine() {
        Shutdown();
    }

    void Engine::Initialize(const WindowConfig& config) {
        Log::Init();
        wi::jobsystem::Initialize();

        m_window->Initialize(config);
        m_physics->Initialize();
        m_renderer->Initialize();
        m_scripting->Initialize();
        m_input->Initialize();
        m_audio->Initialize();
        m_network->Initialize();
        SSAO_System::Get().Initialize(); // Omega Final Integration
        
        m_scripting->ExecuteFile("assets/scripts/main.lua");

        Log::Info("TitanEngine CORE Initialized.");
    }

    void Engine::Run() {
        m_running = true;
        Uint32 lastTime = SDL_GetTicks();
        float accumulator = 0.0f;

        while (m_running && !m_window->ShouldClose()) {
            Uint32 currentTime = SDL_GetTicks();
            float deltaTime = (currentTime - lastTime) / 1000.0f;
            lastTime = currentTime;

            if (deltaTime > 0.25f) deltaTime = 0.25f;

            m_time.deltaTime = deltaTime;
            m_time.totalTime += deltaTime;

            accumulator += deltaTime;

            m_window->PollEvents();
            m_input->Update();

            while (accumulator >= m_time.fixedDeltaTime) {
                FixedUpdate(m_time.fixedDeltaTime);
                accumulator -= m_time.fixedDeltaTime;
            }

            Update(deltaTime);
            Render();
        }
    }

    void Engine::Update(float dt) {
        m_scripting->Update(dt);
        EventSystem::Get().Flush(); // Omega Final Integration
        m_audio->UpdateVoices(dt);
        m_tweenSystem.Update(dt);
        m_editor->Update(dt);
        m_fileWatcher->Update();
        
        auto activeScene = m_sceneStack.Active();
        if (activeScene) {
            HierarchySystem::Update(activeScene->GetRegistry()); // Phase 14 Omega
            VehicleSystem::Update(activeScene->GetRegistry(), dt); // Phase 15 Singularity
            ClothSystem::Update(activeScene->GetRegistry(), dt);   // Phase 15 Singularity
            AISystem::Update(activeScene->GetRegistry(), dt);
            activeScene->OnUpdate(dt);
        }

        for (auto& module : m_modules) {
            module->Update(dt);
        }
    }

    void Engine::FixedUpdate(float dt) {
        // Run Physics in a Job
        wi::jobsystem::Execute(m_physicsJobCtx, [this, dt](wi::jobsystem::JobArgs args) {
            m_physics->Update(dt);
        });
        
        // Wait for physics to complete before proceeding
        wi::jobsystem::Wait(m_physicsJobCtx);

        auto activeScene = m_sceneStack.Active();
        if (activeScene) {
            activeScene->OnFixedUpdate(dt);
        }
    }

    void Engine::Render() {
        m_renderer->BeginFrame();
        
        auto activeScene = m_sceneStack.Active();
        if (activeScene) {
            activeScene->OnRender();
            m_renderer->RenderRegistry(activeScene->GetRegistry());
        }

        m_editor->RenderUI();

        for (auto& module : m_modules) {
            module->Render();
        }

        m_renderer->EndFrame();
        m_window->SwapBuffers();
    }

    void Engine::AddModule(std::shared_ptr<EngineModule> module) {
        m_modules.push_back(module);
        module->Initialize();
    }

    void Engine::Shutdown() {
        Log::Info("TitanEngine Shutting Down...");
        for (auto& module : m_modules) {
            module->Shutdown();
        }
        m_physics->Shutdown();
        m_audio->Shutdown();
        wi::jobsystem::ShutDown();
        m_window->Shutdown();
    }
}
