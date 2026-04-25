// Starlight Engine: CELESTIAL NEXUS SHOWCASE
#include "Engine.hpp"
#include "Log.hpp"
#include "CoreMinimal.hpp"
#include "Components.hpp"
#include "Renderer.hpp"
#include "NetworkSystem.hpp"
#include "CameraSystem.hpp"

using namespace starlight;

class ShowcaseScene : public Scene {
public:
    void OnEnter() override {
        Log::Info("ShowcaseScene: Initializing Celestial Nexus...");
        
        // Start non-blocking server
        Engine::Get().GetNetwork().StartServer(7777);
    }

    void OnUpdate(float dt) override {
        // Lua handles the logic, but we can add C++ global logic here
    }

    void OnFixedUpdate(float dt) override {}
    void OnExit() override {}

    void OnRender() override {
        // Clear is handled by Renderer::BeginFrame
        Engine::Get().GetRenderer().RenderRegistry(GetRegistry());
    }
};

int main(int argc, char* argv[]) {
    WindowConfig config = { "Starlight Engine: Celestial Nexus Showcase", 1280, 720 };
    Engine engine;
    engine.Initialize(config);
    
    // Disable ImGui rendering for the pure showcase
    // (Already commented out in Engine::Render)

    auto scene = std::make_shared<ShowcaseScene>();
    engine.GetSceneStack().Push(scene);
    
    engine.Run();
    return 0;
}
