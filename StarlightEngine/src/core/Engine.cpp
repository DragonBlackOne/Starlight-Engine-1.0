// Este projeto é feito por IA e só o prompt é feito por um humano.
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
#include "CameraSystem.hpp"
#include "LODSystem.hpp"

#undef APIENTRY
#include <SDL2/SDL.h>
#include <thread>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

namespace starlight {

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
        m_editor = &EditorSystem::Get();
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
        m_editor->Initialize();
        SSAO_System::Get().Initialize(); // Omega Final Integration
        
        m_scripting->ExecuteFile("assets/scripts/arcade_master.lua");

        // Force PlayMode for Pure Showcase
        m_editor->SetPlayMode(true);
        
        // Initialize Default Projection
        float aspect = (float)m_window->GetWidth() / (float)m_window->GetHeight();
        m_renderer->UpdateProjection(60.0f, aspect, 0.1f, 1000.0f);

        Log::Info("Starlight Engine CORE Initialized (Showcase Mode).");
    }

    void Engine::Run() {
        m_running = true;
        Uint64 lastTime = SDL_GetPerformanceCounter();
        Uint64 frequency = SDL_GetPerformanceFrequency();
        float accumulator = 0.0f;

        while (m_running && !m_window->ShouldClose()) {
            Uint64 currentTime = SDL_GetPerformanceCounter();
            float deltaTime = static_cast<float>(currentTime - lastTime) / static_cast<float>(frequency);
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
        m_audio->UpdateVoices(dt);
        m_tweenSystem.Update(dt);
        m_editor->Update(dt);
        m_fileWatcher->Update();

        m_scripting->Update(dt);
        EventSystem::Get().Flush(); 
        
        auto activeScene = m_sceneStack.Active();
        if (activeScene) {
            VehicleSystem::Update(activeScene->GetRegistry(), dt); 
            ClothSystem::Update(activeScene->GetRegistry(), dt);   
            AISystem::Update(activeScene->GetRegistry(), dt);
            activeScene->OnUpdate(dt);
        }
        
        // Hierarchy should always update for UI
        if (activeScene) {
            float aspect = (float)m_window->GetWidth() / (float)m_window->GetHeight();
            CameraSystem::Update(activeScene->GetRegistry(), aspect);
            LODSystem::Update(activeScene->GetRegistry());
            HierarchySystem::Update(activeScene->GetRegistry()); // Phase 14 Omega
        }

        for (auto& module : m_modules) {
            module->Update(dt);
        }
    }

    void Engine::FixedUpdate(float dt) {
        // Run Physics in a Job
        wi::jobsystem::Execute(m_physicsJobCtx, [this, dt](wi::jobsystem::JobArgs args) {
            (void)args;
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
        static uint64_t frameCount = 0;
        if (frameCount % 60 == 0) {
            Log::Info("--- ENGINE_FRAME: " + std::to_string(frameCount) + " ---");
        }
        frameCount++;

        m_renderer->BeginFrame();
        
        auto activeScene = m_sceneStack.Active();
        if (activeScene) {
            activeScene->OnRender();
            m_renderer->RenderRegistry(activeScene->GetRegistry());
        }
        
        for (auto& module : m_modules) {
            module->Render();
        }

        // --- ImGui Frame ---
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        m_editor->RenderUI();
        m_scripting->RenderUI();

        for (auto& module : m_modules) {
            module->RenderUI();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        m_renderer->EndFrame();

        m_window->SwapBuffers();
    }

    void Engine::AddModule(std::shared_ptr<EngineModule> module) {
        m_modules.push_back(module);
        module->Initialize();
    }

    void Engine::Shutdown() {
        Log::Info("Starlight Engine Shutting Down...");
        for (auto& module : m_modules) {
            module->Shutdown();
        }
        m_physics->Shutdown();
        m_audio->Shutdown();
        wi::jobsystem::ShutDown();
        m_window->Shutdown();
    }
}
