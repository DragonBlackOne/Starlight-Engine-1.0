// Capital Odyssey - Main Entry Point (PURE 2D - NON-IMGUI)
#include "Engine.hpp"
#include "Log.hpp"
#include "ScriptSystem.hpp"
#include "DashboardSystem.hpp"
#include <string>
#include <vector>

using namespace starlight;

class CommercialGame : public BaseScene {
public:
    void OnEnter() override {
        Log::Info("Capital Odyssey: Terminal Online (PURE 2D).");
        
        auto& scripting = Engine::Get().GetScripting();
        scripting.ExecuteFile("assets/scripts/odyssey_main.lua");
        
        sol::function onStart = scripting.GetLua()["OnStart"];
        if (onStart.valid()) {
            onStart();
        }
    }

    void OnUpdate(float dt) override {
        auto& scripting = Engine::Get().GetScripting();
        sol::function onUpdate = scripting.GetLua()["OnUpdate"];
        if (onUpdate.valid()) {
            onUpdate(dt);
        }
    }

    void OnFixedUpdate(float dt) override { (void)dt; }
    void OnRender() override {}
    
    void OnUIRender() override {
        // Handled by RenderGraph UIPass
    }

    void OnExit() override {
        Log::Info("Capital Odyssey: Terminal Offline.");
    }
};

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    WindowConfig config;
    config.title = "Capital Odyssey: Economic Reconstruction";
    config.width = 1600;
    config.height = 900;
    
    Engine engine;
    engine.Initialize(config);
    
    engine.GetSceneStack().Push(std::make_shared<CommercialGame>());
    
    engine.Run();
    
    return 0;
}
