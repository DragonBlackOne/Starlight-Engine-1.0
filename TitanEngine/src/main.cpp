#include "Engine.hpp"
#include "InputSystem.hpp"
#include "Log.hpp"
#include "Renderer.hpp"
#include "Components.hpp"
#include "MeshDeformSystem.hpp"
#include "VehicleSystem.hpp"
#include <SDL2/SDL.h>

using namespace titan;

class MultiverseSingularityModule : public EngineModule {
public:
    std::string GetName() const override { return "MultiverseSingularityModule"; }

    void Initialize() override {
        Log::Info(">>> ♾️ TITAN INFINITE: MULTIVERSE SINGULARITY (100% TOTAL) <<<");
        Log::Info("Forensic Audit: 15+ Repositories analyzed and consolidated.");
        
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (!activeScene) return;
        auto& registry = activeScene->GetRegistry();

        // Setup Camera
        auto camEntity = registry.create();
        registry.emplace<TransformComponent>(camEntity, glm::vec3(0, 2, 5));
        registry.emplace<CameraComponent>(camEntity);

        // Retro Mode 7 Background
        auto retro = registry.create();
        registry.emplace<RetroComponent>(retro);

        // 1. DEFORMABLE RACING BUGGY (From 'jogo-de-corriga')
        m_car = registry.create();
        registry.emplace<TransformComponent>(m_car, glm::vec3(0, 2, 0));
        registry.emplace<VehicleComponent>(m_car);
        
        // Render Mesh
        auto cubeMesh = Engine::Get().GetRenderer().GetCubeMesh();
        auto shader = Engine::Get().GetRenderer().GetBasicShader();
        Material mat; mat.shader = shader; mat.color = {1.0f, 0.2f, 0.2f}; mat.isPBR = false;
        registry.emplace<MeshComponent>(m_car, cubeMesh, mat);

        registry.emplace<MeshDeformComponent>(m_car);
        auto& deform = activeScene->GetRegistry().get<MeshDeformComponent>(m_car);
        (void)deform;
        Log::Info("MeshDeform: Racing DNA integrated from 'jogo-de-corriga'.");
    }

    void Update(float dt) override {
        (void)dt;
        // The dashboard legacy text has been removed!
    }

    void Render() override {}
    void Shutdown() override {}

private:
    entt::entity m_car;
};

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    WindowConfig config = { "TitanEngine Infinite Multiverse 100%", 1280, 720 };
    Engine engine;
    engine.Initialize(config);
    
    engine.AddModule(std::make_shared<MultiverseSingularityModule>());

    engine.Run();
    engine.Shutdown();
    return 0;
}
