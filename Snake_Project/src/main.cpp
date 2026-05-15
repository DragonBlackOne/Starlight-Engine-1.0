#include "Engine.hpp"
#include "Log.hpp"
#include "ScriptSystem.hpp"
#include "DashboardSystem.hpp"
#include <string>

using namespace starlight;

class SnakeGame : public BaseScene {
public:
    void OnEnter() override {
        Log::Info("Snake Project: Initialized.");
        
        auto& scripting = Engine::Get().GetScripting();
        scripting.ExecuteFile("assets/scripts/snake_main.lua");
        
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
    
    void OnUIRender() override {}

    void OnExit() override {
        Log::Info("Snake Project: Shutting down.");
    }
};

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    WindowConfig config;
    config.title = "Cyber Snake - Starlight Edition";
    config.width = 1280;
    config.height = 720;
    
    Engine engine;
    engine.Initialize(config);
    
    engine.GetSceneStack().Push(std::make_shared<SnakeGame>());
    
    engine.Run();
    
    return 0;
}
