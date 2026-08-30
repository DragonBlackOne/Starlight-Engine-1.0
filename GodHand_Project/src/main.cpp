#include <string>
#include <memory>
#include "Engine.hpp"
#include "Log.hpp"
#include "ScriptSystem.hpp"
#include "Renderer.hpp"
#include "InputSystem.hpp"
#include "EditorSystem.hpp"

using namespace starlight;

class GodHandGameScene : public BaseScene {
public:
    void OnEnter() override {
        Log::Info("GodHand_Project: Initializing 3D Beat 'Em Up Scene...");

        auto& engine = Engine::Get();
        auto& renderer = engine.GetRenderer();
        auto& window = engine.GetWindow();

        float aspect = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
        renderer.UpdateProjection(65.0f, aspect, 0.1f, 1000.0f);
        renderer.SetClearColor({0.12f, 0.08f, 0.05f}); // Warm dusty sunset background

        auto& scripting = engine.GetScripting();
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

    void OnUpdate([[maybe_unused]] float dt) override {
        (void)dt;
        // F5 Script Hot-Reloading
        if (InputSystem::IsKeyJustPressed(pal::KeyCode::F5)) {
            Log::Info("GodHand_Project: F5 pressed. Hot-reloading scripts...");
            GetRegistry().clear();

            auto& scripting = Engine::Get().GetScripting();
            scripting.ResetState();

            auto& renderer = Engine::Get().GetRenderer();
            auto& window = Engine::Get().GetWindow();
            float aspect = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
            renderer.UpdateProjection(65.0f, aspect, 0.1f, 1000.0f);

            scripting.ExecuteFile("assets/scripts/main.lua");
            sol::protected_function onStart = scripting.GetLua()["OnStart"];
            if (onStart.valid()) {
                onStart.set_error_handler(scripting.GetLua()["debug"]["traceback"]);
                auto result = onStart();
                if (!result.valid()) {
                    sol::error err = result;
                    Log::Error("Lua Hot-Reload OnStart Error: {}", err.what());
                }
            }
        }
    }

    void OnRender() override {
        // Automatically handled by Renderer pipeline and RenderGraph
    }

    void OnExit() override {
        Log::Info("GodHand_Project: Exiting scene.");
    }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    try {
        WindowConfig config;
        config.title = "God Hand // Fusion ENGINE (PS2 Retro 3D Brawler)";
        config.width = 1280;
        config.height = 720;
        config.mode2D = false; // Full 3D Mode with PBR, Shadows & SSAO

        Engine engine;
        if (!engine.Initialize(config)) {
            Log::Error("Failed to initialize Fusion ENGINE for GodHand_Project");
            return 1;
        }

        // Run game cleanly without editor panels
        auto editor = engine.GetSystem<EditorSystem>();
        if (editor) {
            editor->SetEnabled(false);
        }

        engine.GetSceneStack().Push(std::make_shared<GodHandGameScene>());
        engine.Run();
    } catch (const std::exception& e) {
        Log::Error("CRITICAL ERROR in GodHand_Project: {}", e.what());
        return 1;
    }
    return 0;
}
