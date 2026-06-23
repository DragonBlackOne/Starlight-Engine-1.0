// Capital Odyssey - Main Entry Point (PURE 2D - NON-IMGUI)
#include <string>
#include <vector>
#include "DashboardSystem.hpp"
#include "Engine.hpp"
#include "Log.hpp"
#include "ScriptSystem.hpp"

using namespace starlight;

class OdysseyScene : public BaseScene {
public:
    void OnEnter() override {
        Log::Info("Capital Odyssey: Terminal Online (PURE 2D).");

        auto& scripting = Engine::Get().GetScripting();
        scripting.ExecuteFile("assets/scripts/odyssey_main.lua");

        sol::protected_function onStart = scripting.GetLua()["OnStart"];
        if (onStart.valid()) {
            auto res = onStart();
            if (!res.valid()) {
                sol::error err = res;
                Log::Error("Capital Odyssey Lua Start Error: {}", err.what());
            }
        }
    }

    void OnExit() override {
        Log::Info("Capital Odyssey: Terminal Offline.");
    }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    try {
        WindowConfig config;
        config.title = "Capital Odyssey // Fusion ENGINE";
        config.width = 1600;
        config.height = 900;
        config.mode2D = true;

        Engine engine;
        if (!engine.Initialize(config)) {
            return 1;
        }

        engine.GetSceneStack().Push(std::make_shared<OdysseyScene>());

        engine.Run();
    } catch (const std::exception& e) {
        Log::Error("CRITICAL ERROR: {}", e.what());
        return 1;
    }
    return 0;
}
