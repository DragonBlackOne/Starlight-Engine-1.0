// Starlight Engine: ODYSSEY ENTRY POINT
#include "Engine.hpp"
#include "Log.hpp"
#include "Renderer.hpp"
#include "ScriptSystem.hpp"
#include "CoreMinimal.hpp"

using namespace starlight;

// Odyssey Scene — renders the 3D world created by Lua scripts
class OdysseyScene : public Scene {
private:
    float m_fpsTimer = 0.0f;
    int m_frameCount = 0;
public:
    void OnEnter() override {
        Log::Info("Odyssey: Scene Active — Lua controls gameplay.");
    }

    void OnUpdate(float dt) override {
        m_fpsTimer += dt;
        m_frameCount++;
        if (m_fpsTimer >= 1.0f) {
            m_fpsTimer = 0.0f;
            m_frameCount = 0;
        }
    }

    void OnFixedUpdate(float dt) override { (void)dt; }
    void OnExit() override {}

    void OnRender() override {
        Engine::Get().GetRenderer().RenderRegistry(GetRegistry());
    }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    try {
        WindowConfig config = { "Starlight Engine: Odyssey Unified Tech Demo", 1280, 720 };
        Engine engine;
        if (!engine.Initialize(config)) {
            Log::Error("Engine initialization failed, exiting.");
            return 1;
        }
        
        // Replace the default BaseScene with our OdysseyScene
        // so Lua scripts and rendering share the same registry
        engine.GetSceneStack().Pop();
        engine.GetSceneStack().Push(std::make_shared<OdysseyScene>());
        
        // Re-execute the Lua script so it creates entities in this scene's registry
        auto scripting = engine.GetSystem<ScriptSystem>();
        if (scripting) {
            Log::Info("Main: Executing Odyssey Lua script...");
            scripting->ExecuteFile("assets/scripts/starlight_odyssey.lua");
            Log::Info("Main: Lua script execution finished.");
        }
        
        Log::Info("Main: Starting Engine Run loop...");
        engine.Run();
        Log::Info("Main: Engine Run loop terminated naturally.");
    } catch (const std::exception& e) {
        Log::Error("Main: FATAL EXCEPTION: " + std::string(e.what()));
        return 1;
    } catch (...) {
        Log::Error("Main: UNKNOWN FATAL EXCEPTION!");
        return 1;
    }
    return 0;
}
