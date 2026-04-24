// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "Engine.hpp"
#include "Log.hpp"

using namespace titan;

/**
 * @brief Classe principal do seu jogo comercial.
 * Aqui você define a lógica de entrada, atualização e renderização do seu projeto.
 */
class CommercialGame : public Scene {
public:
    void OnEnter() override {
        Log::Info("Commercial Project: Scene Initialized.");
        // Carregue seus assets protegidos (.pak) aqui no futuro
    }

    void OnUpdate(float dt) override {
        // Lógica de gameplay principal
    }

    void OnFixedUpdate(float dt) override {
        // Física constante
    }

    void OnRender() override {
        // Comandos de renderização customizados (opcional)
    }

    void OnExit() override {
        Log::Info("Commercial Project: Scene Exited.");
    }
};

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    // Configuração de Janela Profissional
    WindowConfig config;
    config.title = "Genesis_Project_01 v1.0";
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
