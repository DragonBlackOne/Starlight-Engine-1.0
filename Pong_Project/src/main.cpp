#include "Engine.hpp"
#include "Log.hpp"
#include "ScriptSystem.hpp"
#include "DashboardSystem.hpp"
#include <string>

using namespace starlight;

class PongGame : public BaseScene {
public:
    void OnEnter() override {
        Log::Info("Pong Project: Initialized.");
        
        auto& scripting = Engine::Get().GetScripting();
        scripting.ExecuteFile("assets/scripts/pong_main.lua");
        
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
        // We can call a specific UI render function if needed,
        // but ScriptSystem::OnUIRender() already calls "OnRenderUI" in lua.
    }

    void OnExit() override {
        Log::Info("Pong Project: Shutting down.");
    }
};

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    WindowConfig config;
    config.title = "Starlight Pong - Retro Edition";
    config.width = 1280;
    config.height = 720;
    
    Engine engine;
    engine.Initialize(config);
    
    engine.GetSceneStack().Push(std::make_shared<PongGame>());
    
    engine.Run();
    
    return 0;
}
