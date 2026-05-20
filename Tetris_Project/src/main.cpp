#include "Engine.hpp"
#include "Log.hpp"
#include "ScriptSystem.hpp"
#include <iostream>

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

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    try {
        WindowConfig config;
        config.title = "Fusion Tetris v5.0 (Radical Rebuild)";
        config.width = 800;
        config.height = 1000;
        config.vsync = true;

        Engine engine;
        engine.Initialize(config);

        engine.GetSceneStack().Push(std::make_shared<TetrisGame>());

        engine.Run();
        engine.Shutdown();

    } catch (const std::exception& e) {
        std::cerr << "CRITICAL ERROR: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

