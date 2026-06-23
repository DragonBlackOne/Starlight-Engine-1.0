#include <string>
#include "DashboardSystem.hpp"
#include "Engine.hpp"
#include "FightingSystem.hpp"
#include "Log.hpp"
#include "ScriptSystem.hpp"

using namespace starlight;

class FightingGameScene : public BaseScene {
public:
    void OnEnter() override {
        Log::Info("FusionFight: Scene initialized.");

        auto& scripting = Engine::Get().GetScripting();
        scripting.ExecuteFile("assets/scripts/main.lua");

        sol::protected_function onStart = scripting.GetLua()["OnStart"];
        if (onStart.valid()) {
            onStart.set_error_handler(scripting.GetLua()["debug"]["traceback"]);
            auto result = onStart();
            if (!result.valid()) {
                sol::error err = result;
                Log::Error("Lua OnStart Error: {}", err.what());
            }
        }
    }

    void OnExit() override {
        Log::Info("FusionFight: Shutting down.");
    }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    try {
        WindowConfig config;
        config.title = "FusionFight // Fusion ENGINE";
        config.width = 1280;
        config.height = 720;
        config.mode2D = true;

        Engine engine;
        if (!engine.Initialize(config)) {
            return 1;
        }

        engine.GetSceneStack().Push(std::make_shared<FightingGameScene>());

        engine.Run();
    } catch (const std::exception& e) {
        Log::Error("CRITICAL ERROR: {}", e.what());
        return 1;
    }
    return 0;
}
