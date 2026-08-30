#include <string>
#include <memory>
#include "Engine.hpp"
#include "Log.hpp"
#include "ScriptSystem.hpp"
#include "InputSystem.hpp"
#include "Renderer.hpp"

using namespace starlight;

class PongGameScene : public BaseScene {
public:
    void OnEnter() override {
        Log::Info("Pong Project: Initializing Cyberpunk Neon Arcade Scene...");

        auto& engine = Engine::Get();
        auto& renderer = engine.GetRenderer();
        auto& window = engine.GetWindow();

        renderer.SetOrthoProjection(static_cast<float>(window.GetWidth()), static_cast<float>(window.GetHeight()));
        renderer.SetClearColor({0.04f, 0.03f, 0.08f}); // Deep synthwave void

        auto& scripting = engine.GetScripting();
        scripting.ExecuteFile("assets/scripts/pong_main.lua");

        sol::protected_function onStart = scripting.GetLua()["OnStart"];
        if (onStart.valid()) {
            onStart.set_error_handler(scripting.GetLua()["debug"]["traceback"]);
            auto result = onStart();
            if (!result.valid()) {
                sol::error err = result;
                Log::Error("Lua Pong OnStart Error: {}", err.what());
            }
        }
    }

    void OnUpdate([[maybe_unused]] float dt) override {
        (void)dt;
        // F5 Script Hot-Reloading
        if (InputSystem::IsKeyJustPressed(pal::KeyCode::F5)) {
            Log::Info("Pong Project: F5 pressed. Hot-reloading Lua scripts & assets...");
            GetRegistry().clear();

            auto& scripting = Engine::Get().GetScripting();
            scripting.ResetState();

            scripting.ExecuteFile("assets/scripts/pong_main.lua");
            sol::protected_function onStart = scripting.GetLua()["OnStart"];
            if (onStart.valid()) {
                onStart.set_error_handler(scripting.GetLua()["debug"]["traceback"]);
                auto result = onStart();
                if (!result.valid()) {
                    sol::error err = result;
                    Log::Error("Lua Pong Hot-Reload Error: {}", err.what());
                }
            }
        }
    }

    void OnRender() override {
        // Render 2D Sprites and UI handled by engine pipeline
    }

    void OnExit() override {
        Log::Info("Pong Project: Shutting down arcade scene.");
    }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    (void)argc;
    (void)argv;
    try {
        WindowConfig config;
        config.title = "Fusion Pong // Cyberpunk Neon Turbo (Outrun Arcade Edition)";
        config.width = 1280;
        config.height = 720;
        config.fullscreen = false;
        config.resizable = true;
        config.mode2D = true; // 2D Arcade Sprite Mode
        config.vsync = true;

        Engine engine;
        if (!engine.Initialize(config)) {
            Log::Error("Failed to initialize Fusion ENGINE for Pong_Project");
            return 1;
        }

        engine.GetSceneStack().Push(std::make_shared<PongGameScene>());
        engine.Run();
    } catch (const std::exception& e) {
        Log::Error("CRITICAL ERROR in Pong_Project: {}", e.what());
        return 1;
    }
    return 0;
}
