// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#include "Engine.hpp"
#include "Log.hpp"

using namespace starlight;

/**
 * @brief Classe principal do seu jogo comercial.
 * Aqui vocÃƒÆ’Ã‚Âª define a lÃƒÆ’Ã‚Â³gica de entrada, atualizaÃƒÆ’Ã‚Â§ÃƒÆ’Ã‚Â£o e renderizaÃƒÆ’Ã‚Â§ÃƒÆ’Ã‚Â£o do seu projeto.
 */
class CommercialGame : public Scene {
public:
    void OnEnter() override {
        Log::Info("Commercial Project: Scene Initialized.");
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
        Log::Info("Commercial Project: Scene Exited.");
    }
};

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    // ConfiguraÃƒÆ’Ã‚Â§ÃƒÆ’Ã‚Â£o de Janela Profissional
    WindowConfig config;
    config.title = "Starlight Engine Commercial Project v1.0";
    config.width = 1920;
    config.height = 1080;
    config.vsync = true;

    Engine engine;
    engine.Initialize(config);
    
    // Inicia a cena do seu jogo
    engine.GetSceneStack().Push(std::make_shared<CommercialGame>());
    
    engine.Run();
    engine.Shutdown();
    
    return 0;
}
