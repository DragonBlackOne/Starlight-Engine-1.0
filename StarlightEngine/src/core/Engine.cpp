#include "Engine.hpp"
#include "PathResolver.hpp"
#include "SystemScheduler.hpp"
#include "AnimationSystem.hpp"
#include "AssetManager.hpp"
#include "AudioSystem.hpp"
#include "Camera2DSystem.hpp"
#include "CoreMinimal.hpp"
#include "DashboardSystem.hpp"
#include "EditorSystem.hpp"
#include "EngineSystemAdapters.hpp"
#include "EventSystem.hpp"
#include "FileWatcher.hpp"
#include "FightingSystem.hpp"
#include "GPUCullingSystem.hpp"
#include "GameplaySystem.hpp"
#include "InputSystem.hpp"
#include "JobSystem.hpp"
#include "DecalSystem.hpp"
#include "Profiler.hpp"
#include "Log.hpp"
#include "Memory.hpp"
#include "NavigationSystem.hpp"
#include "NetworkSystem.hpp"
#include "PhysicsSystem.hpp"
#include "Renderer.hpp"
#include "Renderer2D.hpp"
#include "SSAO_System.hpp"
#include "ScriptSystem.hpp"
#include "SpriteAnimationSystem.hpp"
#include "SpriteRendererSystem.hpp"
#include "TilemapSystem.hpp"
#include "Tween.hpp"
#include "VFSSystem.hpp"
#include "VFXSystem.hpp"
#include "CVarSystem.hpp"
#include "EventBroker.hpp"
#include "DebugConsole.hpp"
#include "ConfigSystem.hpp"
#include "PluginSystem.hpp"
#include "FootIK.hpp"
#include "RagdollSystem.hpp"
#include "AutomationSystem.hpp"

#undef APIENTRY
#include <SDL2/SDL.h>
#include <chrono>
#include <thread>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

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
    m_systems.RegisterSystem<CVarSystem>();
    m_systems.RegisterSystem<ConfigSystem>();
    m_systems.RegisterSystem<EventBroker>();
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
    m_systems.RegisterSystem<VFXSystem>();
    m_systems.RegisterSystem<DashboardSystem>();
    m_systems.RegisterSystem<GameplaySystem>();
    m_systems.RegisterSystem<FightingSystem>();
    m_systems.RegisterSystem<SpriteAnimationSystem>();
    m_systems.RegisterSystem<SpriteRendererSystem>();
    m_systems.RegisterSystem<Camera2DSystem>();
    m_systems.RegisterSystem<TilemapSystem>();
    m_systems.RegisterSystem<EditorSystem>();
    m_systems.RegisterSystem<PluginSystem>();
    m_systems.RegisterSystem<FootIKSystem>();
    m_systems.RegisterSystem<RagdollSystem>();
    m_systems.RegisterSystem<AutomationSystem>();
    m_systems.RegisterSystem<DecalSystem>();

    // Orphan System Adapters (Fase 2)
    m_systems.RegisterSystem<CameraSystemAdapter>();
    m_systems.RegisterSystem<ClothSystemAdapter>();
    m_systems.RegisterSystem<HierarchySystemAdapter>();
    m_systems.RegisterSystem<LODSystemAdapter>();
    m_systems.RegisterSystem<VehicleSystemAdapter>();
    m_systems.RegisterSystem<MeshDeformSystemAdapter>();
    m_systems.RegisterSystem<AISystemAdapter>();
    m_systems.RegisterSystem<DebugConsole>();
    m_systems.RegisterSystem<OctreeSystemAdapter>();
    m_systems.RegisterSystem<PickerSystemAdapter>();
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Initialize(const WindowConfig& config) {
    Log::Init();
    MemoryManager::Initialize();

    auto configSys = GetSystem<ConfigSystem>();
    WindowConfig activeConfig = config;

    if (configSys) {
        std::string resolvedEngineIni = PathResolver::Resolve("assets/engine.ini");
        if (!configSys->Load(resolvedEngineIni)) {
            configSys->SetInt("Window", "width", config.width);
            configSys->SetInt("Window", "height", config.height);
            configSys->SetBool("Window", "fullscreen", config.fullscreen);
            configSys->SetString("Window", "title", config.title);
            configSys->SetInt("Window", "maxFPS", config.maxFPS);
            configSys->SetBool("Window", "vsync", config.vsync);
            configSys->SetBool("Window", "mode2D", config.mode2D);
            
            configSys->SetFloat("Render", "exposure", 1.0f);
            configSys->SetFloat("Render", "gamma", 2.2f);
            configSys->SetFloat("Render", "bloomThreshold", 0.35f);
            configSys->SetInt("Render", "bloomBlurSteps", 3);
            
            configSys->SetInt("Gameplay", "aiDifficulty", 1);
            configSys->SetInt("Gameplay", "roundsToWin", 2);
            configSys->SetBool("Gameplay", "debugBoxes", false);

            configSys->SetFloat("Audio", "volume", 1.0f);
            configSys->SetFloat("Audio", "musicVolume", 1.0f);
            configSys->SetFloat("Audio", "effectsVolume", 1.0f);
            
            configSys->Save(resolvedEngineIni);
        } else {
            activeConfig.width = configSys->GetInt("Window", "width", config.width);
            activeConfig.height = configSys->GetInt("Window", "height", config.height);
            activeConfig.fullscreen = configSys->GetBool("Window", "fullscreen", config.fullscreen);
            activeConfig.title = configSys->GetString("Window", "title", config.title);
            activeConfig.maxFPS = configSys->GetInt("Window", "maxFPS", config.maxFPS);
            activeConfig.vsync = configSys->GetBool("Window", "vsync", config.vsync);
            // mode2D is an engine-level intent set in code (main.cpp); do NOT let
            // engine.ini override it, otherwise all 2D games silently run in 3D mode.
            activeConfig.mode2D = config.mode2D;
        }
    }

    m_maxFPS = activeConfig.maxFPS;

    if (!m_window->Initialize(activeConfig)) {
        Log::Error("Engine: Window initialization failed, aborting.");
        return false;
    }

    m_window->SetResizeCallback([this](int w, int h) {
        for (auto& system : m_systems.GetSystems()) {
            if (system->IsEnabled()) {
                system->OnResize(w, h);
            }
        }
    });

    EngineContext context;
    context.window = m_window.get();
    context.engine = this;

    // Enable/disable systems based on configuration
    if (configSys) {
        for (auto& system : m_systems.GetSystems()) {
            std::string sysName = system->GetName();
            bool defaultEnabled = true;
            if (sysName == "ConfigSystem" || sysName == "InputSystem" || 
                sysName == "Renderer" || sysName == "EventBroker" || 
                sysName == "ScriptSystem" || sysName == "AssetManager" || 
                sysName == "VFSSystem") {
                defaultEnabled = true;
            } else if (sysName == "EditorSystem") {
                defaultEnabled = configSys->GetBool("Systems", "EditorSystem", false);
            } else {
                defaultEnabled = configSys->GetBool("Systems", sysName, true);
            }
            system->SetEnabled(defaultEnabled);
            if (!defaultEnabled) {
                Log::Info("Engine: System '{}' is disabled by configuration.", sysName);
            }
        }
    }

    m_systems.SortByPriority();

    // Initialize all registered and enabled systems
    bool allSystemsOK = true;
    for (auto& system : m_systems.GetSystems()) {
        if (system->IsEnabled()) {
            if (!system->OnInitialize(context)) {
                Log::Error("Engine: System '{}' failed to initialize.", system->GetName());
                allSystemsOK = false;
            }
        }
    }
    if (!allSystemsOK) {
        Log::Error("Engine: One or more systems failed to initialize, aborting.");
        return false;
    }

    m_scheduler = std::make_unique<SystemScheduler>();
    m_scheduler->Build(m_systems.GetSystems());

    // Legacy/Static Initializations
    Renderer2D::Initialize();
    // SSAO is initialized inside Renderer::OnInitialize — do not init twice
    // SSAO_System::Get().Initialize();

    m_sceneStack.Push(std::make_shared<BaseScene>());

    // Initialize Projection (2D Ortho or 3D Perspective)
    auto renderer = GetSystem<Renderer>();
    if (renderer) {
        if (activeConfig.mode2D) {
            renderer->SetOrthoProjection((float)m_window->GetWidth(), (float)m_window->GetHeight());
        } else {
            float aspect = (float)m_window->GetWidth() / (float)m_window->GetHeight();
            renderer->UpdateProjection(60.0f, aspect, 0.1f, 1000.0f);
        }
    }

    // Register default engine CVars populated from engine.ini config
    auto cvarSys = GetSystem<CVarSystem>();
    if (cvarSys) {
        float initExposure = configSys ? configSys->GetFloat("Render", "exposure", 1.0f) : 1.0f;
        float initGamma = configSys ? configSys->GetFloat("Render", "gamma", 2.2f) : 2.2f;
        bool initDebugBoxes = configSys ? configSys->GetBool("Gameplay", "debugBoxes", false) : false;
        int initAiDiff = configSys ? configSys->GetInt("Gameplay", "aiDifficulty", 1) : 1;
        int initRounds = configSys ? configSys->GetInt("Gameplay", "roundsToWin", 2) : 2;
        int initShowProfiler = configSys ? configSys->GetInt("Render", "showProfiler", 0) : 0;
        bool initCull = configSys ? configSys->GetBool("Render", "cull", true) : true;
        bool initCull2D = configSys ? configSys->GetBool("Render", "cull2d", true) : true;
        float initFogDensity = configSys ? configSys->GetFloat("Render", "fogDensity", 0.0f) : 0.0f;
        
        float initVolume = configSys ? configSys->GetFloat("Audio", "volume", 1.0f) : 1.0f;
        float initMusicVolume = configSys ? configSys->GetFloat("Audio", "musicVolume", 1.0f) : 1.0f;
        float initEffectsVolume = configSys ? configSys->GetFloat("Audio", "effectsVolume", 1.0f) : 1.0f;

        cvarSys->RegisterFloat("r_exposure", initExposure, "HDR renderer exposure factor", [](float val) {
            if (Engine::Get().GetSystem<Renderer>()) {
                Engine::Get().GetRenderer().SetExposure(val);
            }
        });
        cvarSys->RegisterFloat("r_gamma", initGamma, "Gamma correction factor", [](float val) {
            if (Engine::Get().GetSystem<Renderer>()) {
                Engine::Get().GetRenderer().SetGamma(val);
            }
        });
        cvarSys->RegisterBool("g_debugBoxes", initDebugBoxes, "Show ECS physics collision boxes");
        cvarSys->RegisterInt("g_aiDifficulty", initAiDiff, "AI difficulty level (0=Easy, 1=Medium, 2=Hard, 3=Expert)");
        cvarSys->RegisterInt("g_roundsToWin", initRounds, "Rounds required to win a match");
        cvarSys->RegisterInt("g_graphicsPreset", 2, "Graphics settings preset (0=Low, 1=Medium, 2=High)");
        cvarSys->RegisterInt("r_showProfiler", initShowProfiler, "Show performance profiler overlay (0=Off, 1=Compact, 2=Detailed)");
        cvarSys->RegisterBool("r_cull", initCull, "Enable CPU frustum culling for 3D meshes");
        cvarSys->RegisterBool("r_cull2d", initCull2D, "Enable CPU viewport culling for 2D sprites/tilemaps");
        cvarSys->RegisterBool("r_deferred", false, "Enable Deferred Shading Renderer");
        cvarSys->RegisterBool("r_taa", true, "Enable Temporal Anti-Aliasing (TAA)");
        cvarSys->RegisterBool("r_fsr", false, "Enable AMD FSR Quality Upscaling");
        cvarSys->RegisterFloat("r_fsr_sharpness", 0.8f, "AMD FSR Sharpening filter strength");
        cvarSys->RegisterBool("r_crt", false, "Enable Retro CRT Scanline Shader");
        cvarSys->RegisterFloat("r_fog_density", initFogDensity, "Atmospheric distance fog density (0=off)");

        cvarSys->RegisterFloat("snd_volume", initVolume, "Master volume (0.0 to 1.0)", [](float val) {
            auto audio = Engine::Get().GetSystem<AudioSystem>();
            if (audio) audio->SetMasterVolume(val);
        });
        cvarSys->RegisterFloat("snd_musicVolume", initMusicVolume, "Music volume (0.0 to 1.0)", [](float val) {
            auto audio = Engine::Get().GetSystem<AudioSystem>();
            if (audio) audio->SetMusicVolume(val);
        });
        cvarSys->RegisterFloat("snd_effectsVolume", initEffectsVolume, "Sound effects volume (0.0 to 1.0)", [](float val) {
            auto audio = Engine::Get().GetSystem<AudioSystem>();
            if (audio) audio->SetEffectsVolume(val);
        });
        cvarSys->RegisterBool("r_vsync", activeConfig.vsync, "Enable VSync", [](bool val) {
            SDL_GL_SetSwapInterval(val ? 1 : 0);
        });
        cvarSys->RegisterInt("r_maxfps", activeConfig.maxFPS, "Max FPS (0=unlimited)", [](int val) {
            Engine::Get().SetMaxFPS(val);
        });
    }

    Log::Info("Engine initialized successfully.");
    return true;
}

PhysicsSystem& Engine::GetPhysics() {
    return *GetSystem<PhysicsSystem>();
}
Renderer& Engine::GetRenderer() {
    return *GetSystem<Renderer>();
}
ScriptSystem& Engine::GetScripting() {
    return *GetSystem<ScriptSystem>();
}
InputSystem& Engine::GetInput() {
    return *GetSystem<InputSystem>();
}
AudioSystem& Engine::GetAudio() {
    return *GetSystem<AudioSystem>();
}
TweenSystem& Engine::GetTweenSystem() {
    return *GetSystem<TweenSystem>();
}
NavigationSystem& Engine::GetNav() {
    return *GetSystem<NavigationSystem>();
}
NetworkSystem& Engine::GetNetwork() {
    return *GetSystem<NetworkSystem>();
}
FileWatcher& Engine::GetFileWatcher() {
    return *GetSystem<FileWatcher>();
}
AssetManager& Engine::GetAssetManager() {
    return *GetSystem<AssetManager>();
}
ConfigSystem& Engine::GetConfig() {
    return *GetSystem<ConfigSystem>();
}

void Engine::Run() {
    m_running = true;

    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();
    double accumulator = 0.0;

    float frameTimes[60] = {0.0f};
    int frameCount = 0;

    while (m_running && !m_window->ShouldClose()) {
        auto currentTime = clock::now();
        std::chrono::duration<double> diff = currentTime - lastTime;
        double deltaTime = diff.count();

        if (m_maxFPS > 0) {
            double targetFrameTime = 1.0 / m_maxFPS;
            if (deltaTime < targetFrameTime) {
                double sleepTime = targetFrameTime - deltaTime;
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));

                currentTime = clock::now();
                diff = currentTime - lastTime;
                deltaTime = diff.count();
            }
        }

        lastTime = currentTime;

        if (deltaTime > 0.25) {
            Log::Warn("Engine: DeltaTime clamped from {:.3f}s to 0.25s (frame spike detected).", deltaTime);
            deltaTime = 0.25;
        }

        m_time.deltaTime = (float)deltaTime;
        m_time.totalTime += (float)deltaTime;

        frameTimes[frameCount % 60] = (float)deltaTime;
        frameCount++;

        float sum = 0.0f;
        int count = std::min(frameCount, 60);
        for (int i = 0; i < count; ++i) {
            sum += frameTimes[i];
        }
        m_time.avgFrameTime = sum / count;
        m_time.fps = m_time.avgFrameTime > 0.0f ? (1.0f / m_time.avgFrameTime) : 0.0f;

        // Reset CPU frame profiler variables
        m_framePhysicsTime = 0.0f;
        m_frameScriptTime = 0.0f;
        m_frameAudioTime = 0.0f;
        m_frameRenderTime = 0.0f;
        m_frameUpdateTime = 0.0f;

        accumulator += deltaTime;

        m_window->PollEvents();

        auto startPhys = clock::now();
        while (accumulator >= m_time.fixedDeltaTime) {
            FixedUpdate((float)m_time.fixedDeltaTime);
            accumulator -= m_time.fixedDeltaTime;
        }
        auto endPhys = clock::now();
        m_framePhysicsTime += std::chrono::duration<float, std::milli>(endPhys - startPhys).count();

        try {
            auto startUpd = clock::now();
            Update((float)deltaTime);
            auto endUpd = clock::now();
            m_frameUpdateTime += std::chrono::duration<float, std::milli>(endUpd - startUpd).count();

            auto startRend = clock::now();
            Render();
            auto endRend = clock::now();
            m_frameRenderTime += std::chrono::duration<float, std::milli>(endRend - startRend).count();
        } catch (const std::exception& e) {
            Log::Error("[Engine Runtime Exception] {}", e.what());
        } catch (...) {
            Log::Error("[Engine Runtime Exception] Unknown exception occurred!");
        }

        // Apply exponential moving average (smooth lerp 0.1f)
        float lerpFactor = 0.1f;
        m_profilerStats.physicsTime += (m_framePhysicsTime - m_profilerStats.physicsTime) * lerpFactor;
        m_profilerStats.scriptTime += (m_frameScriptTime - m_profilerStats.scriptTime) * lerpFactor;
        m_profilerStats.audioTime += (m_frameAudioTime - m_profilerStats.audioTime) * lerpFactor;
        m_profilerStats.renderTime += (m_frameRenderTime - m_profilerStats.renderTime) * lerpFactor;
        m_profilerStats.updateTime += (m_frameUpdateTime - m_profilerStats.updateTime) * lerpFactor;

        Profiler::Get().EndFrame(static_cast<float>(deltaTime * 1000.0));
        MemoryManager::ClearFrame();

        if (m_window->ShouldClose())
            Log::Info("Engine: Window requested close.");
    }
    Log::Info("Engine: Main loop terminated.");
}

void Engine::Update(float dt) {
    if (m_scheduler) {
        m_scheduler->Update(dt);
    }
}

void Engine::FixedUpdate(float dt) {
    if (m_paused)
        return;

    if (m_scheduler) {
        m_scheduler->FixedUpdate(dt);
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

    Renderer2D::ResetStats();

    // System Rendering
    for (auto& system : m_systems.GetSystems()) {
        if (system->IsEnabled()) {
            system->OnRender();
        }
    }

    // System UI Rendering handled by RenderGraph UIPass / OnRender

    // Draw performance profiler overlay if enabled and editor is not active
    auto cvarSys = GetSystem<CVarSystem>();
    auto editor = GetSystem<EditorSystem>();
    bool editorActive = (editor && editor->IsEnabled());
    if (cvarSys && cvarSys->GetInt("r_showProfiler") > 0 && !editorActive) {
        int mode = cvarSys->GetInt("r_showProfiler");
        ImGui::SetNextWindowBgAlpha(0.75f);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | 
                                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | 
                                 ImGuiWindowFlags_NoNav;
        
        float pad = 10.0f;
        const ImGuiViewport* vp = ImGui::GetMainViewport();
        ImVec2 window_pos = ImVec2(vp->WorkPos.x + vp->WorkSize.x - pad, vp->WorkPos.y + pad);
        ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.04f, 0.02f, 0.08f, 0.90f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.6f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));
        
        if (ImGui::Begin("##PerformanceProfilerHUD", nullptr, flags)) {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.9f, 1.0f), "STARLIGHT PERFORMANCE HUD");
            ImGui::Separator();
            ImGui::Text("FPS: %.1f (%.3f ms)", m_time.fps, m_time.avgFrameTime * 1000.0f);
            
            auto stats = Renderer2D::GetStats();
            ImGui::Text("Draw Calls: %d | Quads: %d", stats.drawCalls, stats.quadCount);
            ImGui::Text("2D Objects: %u / %u (Visible/Total)", stats.visible2DObjects, stats.total2DObjects);
            
            auto renderer = GetSystem<Renderer>();
            if (renderer) {
                const auto& rStats = renderer->GetStats();
                ImGui::Text("3D Meshes: %u / %u (Visible/Total)", rStats.visibleMeshes, rStats.totalMeshes);
            }

            auto activeScene = m_sceneStack.Active();
            if (activeScene) {
                size_t entCount = activeScene->GetRegistry().storage<entt::entity>().size();
                ImGui::Text("Entities: %zu", entCount);
            }

            if (mode == 2) {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.6f, 1.0f), "CPU SUBSYSTEMS LATENCY");
                ImGui::Separator();
                
                const auto& prof = m_profilerStats;
                
                auto drawSubsystemTime = [](const char* label, float timeMs, const ImVec4& color) {
                    ImGui::Text("%-8s: %6.2f ms", label, timeMs);
                    ImGui::SameLine();
                    float progress = timeMs / 16.67f;
                    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
                    ImGui::ProgressBar(progress, ImVec2(100.0f, ImGui::GetTextLineHeight() * 0.8f), "");
                    ImGui::PopStyleColor(2);
                };
                
                drawSubsystemTime("Render", prof.renderTime, ImVec4(0.0f, 1.0f, 0.9f, 1.0f));
                drawSubsystemTime("Scripts", prof.scriptTime, ImVec4(1.0f, 0.0f, 0.6f, 1.0f));
                drawSubsystemTime("Physics", prof.physicsTime, ImVec4(1.0f, 0.7f, 0.0f, 1.0f));
                drawSubsystemTime("Audio", prof.audioTime, ImVec4(0.2f, 1.0f, 0.4f, 1.0f));
                drawSubsystemTime("Update", prof.updateTime, ImVec4(0.7f, 0.4f, 1.0f, 1.0f));
                
                static float fpsHistory[100] = {0};
                static int fpsIndex = 0;
                fpsHistory[fpsIndex] = m_time.fps;
                fpsIndex = (fpsIndex + 1) % 100;
                
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.6f, 1.0f), "FPS HISTORY (100 frames)");
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.09f, 0.04f, 0.16f, 0.5f));
                ImGui::PlotLines("##FPSPlot", fpsHistory, 100, fpsIndex, nullptr, 0.0f, 120.0f, ImVec2(220.0f, 40.0f));
                ImGui::PopStyleColor();
            }
        }
        ImGui::End();
        ImGui::PopStyleColor(3);
    }

    // Finalize ImGui (ensure screen framebuffer is bound)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    m_window->SwapBuffers();
}

void Engine::Shutdown() {
    if (m_shutdown)
        return;

    auto cvarSys = GetSystem<CVarSystem>();
    auto configSys = GetSystem<ConfigSystem>();
    if (cvarSys && configSys) {
        configSys->SetFloat("Render", "exposure", cvarSys->GetFloat("r_exposure"));
        configSys->SetFloat("Render", "gamma", cvarSys->GetFloat("r_gamma"));
        configSys->SetInt("Render", "showProfiler", cvarSys->GetInt("r_showProfiler"));
        configSys->SetBool("Render", "cull", cvarSys->GetBool("r_cull"));
        configSys->SetBool("Render", "cull2d", cvarSys->GetBool("r_cull2d"));
        configSys->SetBool("Gameplay", "debugBoxes", cvarSys->GetBool("g_debugBoxes"));
        configSys->SetInt("Gameplay", "aiDifficulty", cvarSys->GetInt("g_aiDifficulty"));
        configSys->SetInt("Gameplay", "roundsToWin", cvarSys->GetInt("g_roundsToWin"));
        
        configSys->SetInt("Window", "width", m_window->GetWidth());
        configSys->SetInt("Window", "height", m_window->GetHeight());
        configSys->SetBool("Window", "fullscreen", m_window->IsFullscreen());
        configSys->SetBool("Window", "vsync", cvarSys->GetBool("r_vsync"));
        configSys->SetInt("Window", "maxFPS", cvarSys->GetInt("r_maxfps"));

        configSys->SetFloat("Audio", "volume", cvarSys->GetFloat("snd_volume"));
        configSys->SetFloat("Audio", "musicVolume", cvarSys->GetFloat("snd_musicVolume"));
        configSys->SetFloat("Audio", "effectsVolume", cvarSys->GetFloat("snd_effectsVolume"));
        
        auto inputSys = GetSystem<InputSystem>();
        if (inputSys) {
            inputSys->SaveBindings(*configSys);
        }
        
        configSys->Save(PathResolver::Resolve("assets/engine.ini"));
    }

    m_shutdown = true;
    Log::Info("Starlight Engine Shutting Down...");
    SSAO_System::Get().Shutdown();
    Renderer2D::Shutdown();
    m_systems.Shutdown();
    m_window->Shutdown();
    MemoryManager::Shutdown();
}
}  // namespace starlight
