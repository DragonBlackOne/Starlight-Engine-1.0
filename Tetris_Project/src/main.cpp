#include "Engine.hpp"
#include "Log.hpp"
#include "ScriptSystem.hpp"

using namespace starlight;

class TetrisGame : public BaseScene {
public:
    void OnEnter() override {
        Log::Info("Tetris Project: Initialized.");

        auto& scripting = Engine::Get().GetScripting();
        scripting.ExecuteFile("assets/scripts/tetris_main.lua");

        sol::protected_function onStart = scripting.GetLua()["OnStart"];
        if (onStart.valid()) {
            auto result = onStart();
            if (!result.valid()) {
                sol::error err = result;
                Log::Error("Lua OnStart Error: {}", err.what());
            }
        }
    }

    void OnExit() override {
        Log::Info("Tetris Project: Shutting down.");
    }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    try {
        WindowConfig config;
        config.title = "Tetris // Fusion ENGINE";
        config.width = 800;
        config.height = 1000;
        config.vsync = true;
        config.mode2D = true;

        Engine engine;
        if (!engine.Initialize(config)) {
            return 1;
        }

        engine.GetSceneStack().Push(std::make_shared<TetrisGame>());

        engine.Run();

    } catch (const std::exception& e) {
        Log::Error("CRITICAL ERROR: {}", e.what());
        return -1;
    }
    return 0;
}
