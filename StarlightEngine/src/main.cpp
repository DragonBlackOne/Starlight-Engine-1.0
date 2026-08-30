#include "Engine.hpp"
#include "Log.hpp"

using namespace starlight;

/**
 * @brief Classe principal do seu jogo comercial.
 * Aqui vocÃƒÆ’Ã‚Âª define a lÃƒÆ’Ã‚Â³gica de entrada, atualizaÃƒÆ’Ã‚Â§ÃƒÆ’Ã‚Â£o e renderizaÃƒÆ’Ã‚Â§ÃƒÆ’Ã‚Â£o do seu projeto.
 */
class StarlightGame : public Scene {
public:
    void OnEnter() override {
        Log::Info("Starlight Project: Scene Initialized.");
        // Carregue seus assets protegidos (.pak) aqui no futuro
    }

    void OnUpdate(float dt) override {
        // LÃƒÆ’Ã‚Â³gica de gameplay principal
    }

    void OnFixedUpdate(float dt) override {
        // FÃƒÆ’Ã‚Â­sica constante
    }

    void OnRender() override {
        // Comandos de renderizaÃƒÆ’Ã‚Â§ÃƒÆ’Ã‚Â£o customizados (opcional)
    }

    void OnExit() override {
        Log::Info("Starlight Project: Scene Exited.");
    }
};

#include <crtdbg.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    // Disable MSVC interactive GUI popups for asserts/errors to fail-fast in GDB/CTest
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    
    // ConfiguraÃƒÆ’Ã‚Â§ÃƒÆ’Ã‚Â§ÃƒÆ’Ã‚Â£o de Janela Profissional
    WindowConfig config;
    config.title = "Starlight Engine Open Source Project v1.0";
    config.width = 1920;
    config.height = 1080;
    config.vsync = true;

    Engine engine;
    engine.Initialize(config);
    
    // Inicia a cena do seu jogo
    engine.GetSceneStack().Push(std::make_shared<StarlightGame>());
    
    engine.Run();
    engine.Shutdown();
    
    return 0;
}
