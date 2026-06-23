#include <string>
#include "DashboardSystem.hpp"
#include "Engine.hpp"
#include "Log.hpp"
#include "ScriptSystem.hpp"

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

    void OnExit() override {
        Log::Info("Pong Project: Shutting down.");
    }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    try {
        WindowConfig config;
        config.title = "Pong // Fusion ENGINE";
        config.width = 1280;
        config.height = 720;
        config.mode2D = true;

        Engine engine;
        if (!engine.Initialize(config)) {
            return 1;
        }

        engine.GetSceneStack().Push(std::make_shared<PongGame>());

        engine.Run();
    } catch (const std::exception& e) {
        Log::Error("CRITICAL ERROR: {}", e.what());
        return 1;
    }
    return 0;
}
