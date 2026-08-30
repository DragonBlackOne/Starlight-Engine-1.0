#include <string>
#include <memory>
#include "Engine.hpp"
#include "Log.hpp"
#include "ScriptSystem.hpp"
#include "Renderer.hpp"
#include "InputSystem.hpp"
#include "EditorSystem.hpp"

using namespace starlight;

class TechDemoScene : public BaseScene {
public:
    void OnEnter() override {
        Log::Info("TechDemo_Project: Initializing Next-Gen Architectural Showcase...");

        auto& engine = Engine::Get();
        auto& renderer = engine.GetRenderer();
        auto& window = engine.GetWindow();

        float aspect = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
        renderer.UpdateProjection(60.0f, aspect, 0.1f, 2000.0f);
        renderer.SetClearColor({ 0.05f, 0.04f, 0.08f }); // Deep synthwave night sky

        auto& scripting = engine.GetScripting();
        scripting.ExecuteFile("assets/scripts/main.lua");

        sol::protected_function onStart = scripting.GetLua()["OnStart"];
        if (onStart.valid()) {
            onStart.set_error_handler(scripting.GetLua()["debug"]["traceback"]);
            auto result = onStart();
            if (!result.valid()) {
                sol::error err = result;
                Log::Error("Lua TechDemo OnStart Error: {}", err.what());
            }
        }
    }

    void OnUpdate([[maybe_unused]] float dt) override {
        (void)dt;
        // F5 Script Hot-Reloading
        if (InputSystem::IsKeyJustPressed(pal::KeyCode::F5)) {
            Log::Info("TechDemo_Project: F5 pressed. Hot-reloading Lua scripts & assets...");
            GetRegistry().clear();

            auto& scripting = Engine::Get().GetScripting();
            scripting.ResetState();

            auto& renderer = Engine::Get().GetRenderer();
            auto& window = Engine::Get().GetWindow();
            float aspect = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
            renderer.UpdateProjection(60.0f, aspect, 0.1f, 2000.0f);

            scripting.ExecuteFile("assets/scripts/main.lua");
            sol::protected_function onStart = scripting.GetLua()["OnStart"];
            if (onStart.valid()) {
                onStart.set_error_handler(scripting.GetLua()["debug"]["traceback"]);
                auto result = onStart();
                if (!result.valid()) {
                    sol::error err = result;
                    Log::Error("Lua TechDemo Hot-Reload Error: {}", err.what());
                }
            }
        }
    }

    void OnRender() override {
        // Handled automatically by Engine Renderer & RenderGraph
    }

    void OnExit() override {
        Log::Info("TechDemo_Project: Exited showcase scene.");
    }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    (void)argc;
    (void)argv;
    try {
        WindowConfig config;
        config.title = "Fusion ENGINE // Next-Gen Architectural Showcase (7-Module Tech Demo)";
        config.width = 1920;
        config.height = 1080;
        config.mode2D = false; // Full 3D PBR + 2D Batching hybrid pipeline
        config.vsync = true;

        Engine engine;
        if (!engine.Initialize(config)) {
            Log::Error("Failed to initialize Fusion ENGINE for TechDemo_Project");
            return 1;
        }

        engine.GetSceneStack().Push(std::make_shared<TechDemoScene>());
        engine.Run();
    } catch (const std::exception& e) {
        Log::Error("CRITICAL ERROR in TechDemo_Project: {}", e.what());
        return 1;
    }
    return 0;
}
