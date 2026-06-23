#include "GameplaySystem.hpp"
#include "Engine.hpp"
#include "VehicleSystem.hpp"
#include "ClothSystem.hpp"
#include "AISystem.hpp"
#include "HierarchySystem.hpp"
#include "CameraSystem.hpp"
#include "LODSystem.hpp"
#include "JobSystem.hpp"

namespace starlight {

    bool GameplaySystem::OnInitialize(const EngineContext& context) {
        m_engine = context.engine;
        return true;
    }

    void GameplaySystem::OnShutdown() {
    }

    void GameplaySystem::OnUpdate(float dt) {
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (!activeScene) return;

        auto& reg = activeScene->GetRegistry();

        // Phase 1: Parallel heavy systems (JobSystem v4.0)
        JobContext ctx;
        JobSystem::Execute(ctx, [&reg, dt](uint32_t) { VehicleSystem::Update(reg, dt); });
        JobSystem::Execute(ctx, [&reg, dt](uint32_t) { ClothSystem::Update(reg, dt); });
        JobSystem::Execute(ctx, [&reg, dt](uint32_t) { AISystem::Update(reg, dt); });
        
        // While systems are running, we can do some main-thread work
        using scene_clock = std::chrono::high_resolution_clock;
        auto sStart = scene_clock::now();
        activeScene->OnUpdate(dt); // Lua updates stay on main thread for safety
        auto sEnd = scene_clock::now();
        float elapsed = std::chrono::duration<float, std::milli>(sEnd - sStart).count();
        Engine::Get().AccumulateScriptTime(elapsed);

        // Wait for parallel jobs to finish
        JobSystem::Wait(ctx);
        
        // Phase 2: Sequential dependency-heavy systems
        HierarchySystem::Update(reg);
        
        float aspect = (float)Engine::Get().GetWindow().GetWidth() / (float)Engine::Get().GetWindow().GetHeight();
        CameraSystem::Update(reg, aspect);
        LODSystem::Update(reg);
    }

}
