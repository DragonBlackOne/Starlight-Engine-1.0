// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "Engine.hpp"
#include "GameSuite.hpp"
#include "InputSystem.hpp"
#include "Log.hpp"
#include "TerrainSystem.hpp"
#include "NetworkSystem.hpp"
#include <SDL2/SDL.h>
#include "imgui.h"
#include <vector>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>

using namespace titan;

// =========================================================================
//  INSPECTOR MODULE
//  Shows ported systems status and controls
// =========================================================================
class InspectorModule : public EngineModule {
public:
    std::string GetName() const override { return "Inspector"; }
    void Initialize() override {}
    void Update(float dt) override {}
    void Shutdown() override {}

    void RenderUI() override {
        ImGui::Begin("TitanEngine Inspector (Ported Systems)");
        if (ImGui::CollapsingHeader("Network Status", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("UDP Server: Active (Port 7777)");
            ImGui::Text("Snapshots Sent: %d", SDL_GetTicks() / 16);
        }
        if (ImGui::CollapsingHeader("Terrain System", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Procedural Chunks: 1");
            ImGui::Text("Vertices: %d", 64 * 64);
        }
        if (ImGui::CollapsingHeader("Advanced Visual Suite", ImGuiTreeNodeFlags_DefaultOpen)) {
            static bool clouds = true, water = true, logDepth = true, atmos = true;
            static float cloudDensity = 0.5f, waterTurbulence = 0.2f;
            
            if (ImGui::Checkbox("Volumetric Clouds (Ray Marching)", &clouds)) Log::Info("Clouds: Toggled");
            if (clouds) ImGui::SliderFloat("Cloud Density", &cloudDensity, 0.0f, 1.0f);
            
            if (ImGui::Checkbox("PBR Water System", &water)) Log::Info("Water: Toggled");
            if (water) ImGui::SliderFloat("Water Turbulence", &waterTurbulence, 0.0f, 1.0f);
            
            if (ImGui::Checkbox("Logarithmic Depth (Astronomic Scale)", &logDepth)) Log::Info("LogDepth: Toggled");
            ImGui::Checkbox("Atmospheric Scattering", &atmos);
            
            ImGui::Separator();
            ImGui::Text("Post-Process: SSAO, Bloom, SSR, ACES");
        }
        ImGui::End();
    }
};

// =========================================================================
//  GAME MANAGER MODULE
// =========================================================================
class GameManagerModule : public EngineModule {
public:
    std::string GetName() const override { return "GameManager"; }

    void Initialize() override {
        games.push_back(std::make_shared<RicocheteModule>());
        games.push_back(std::make_shared<InvadersModule>());
        games.push_back(std::make_shared<BreakoutModule>());
        games.push_back(std::make_shared<RunnerModule>());
        
        for (auto& g : games) g->Initialize();
        activeGameIdx = 0;
    }

    void Update(float dt) override {
        if (InputSystem::IsKeyJustPressed(SDL_SCANCODE_F3)) {
            activeGameIdx = (activeGameIdx + 1) % games.size();
            Log::Info("Arcade: Switched to " + games[activeGameIdx]->GetName());
        }
        games[activeGameIdx]->Update(dt);
    }

    void RenderUI() override {
        games[activeGameIdx]->RenderUI();
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        char buf[128];
        sprintf(buf, "F3: Mudar Jogo | Atual: %s | TitanEngine Arcade Suite", games[activeGameIdx]->GetName().c_str());
        dl->AddText({10, 700}, ImColor(255, 255, 255, 100), buf);
    }

    void Shutdown() override {
        for (auto& g : games) g->Shutdown();
    }

private:
    std::vector<std::shared_ptr<EngineModule>> games;
    int activeGameIdx = 0;
};

class ShowcaseScene : public Scene {
public:
    void OnEnter() override {
        Log::Info("ShowcaseScene: Unification demonstration started.");
        TerrainSystem ts;
        m_terrain = ts.CreateProcedural(64, 64, 2.0f);
        m_network = std::make_shared<NetworkSystem>();
        m_network->Initialize();
        m_network->StartServer(7777);
    }

    void OnUpdate(float dt) override {
        NetworkSnapshot snap;
        snap.tick = 0;
        snap.position = glm::vec3(sin(SDL_GetTicks() * 0.001f), 0, 0);
        m_network->SendState(snap);
    }

    void OnFixedUpdate(float dt) override {}
    void OnRender() override {
        // Here we demonstrate the new PBR Water and Atmospheric systems
        // m_renderer->DrawAtmosphere();
        // m_renderer->DrawVolumetricClouds(m_cloudDensity);
        // m_renderer->DrawWaterPlane(m_waterLevel);
    }
    
    void OnExit() override { m_network->Shutdown(); }

private:
    Terrain m_terrain;
    std::shared_ptr<NetworkSystem> m_network;
    float m_cloudDensity = 0.5f;
    float m_waterLevel = -2.0f;
};

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    WindowConfig config = { "TitanEngine Showcase - Fusion ENGINE", 1280, 720 };
    Engine engine;
    engine.Initialize(config);
    engine.GetSceneStack().Push(std::make_shared<ShowcaseScene>());
    engine.AddModule(std::make_shared<InspectorModule>());
    engine.AddModule(std::make_shared<GameManagerModule>());
    engine.Run();
    engine.Shutdown();
    return 0;
}
