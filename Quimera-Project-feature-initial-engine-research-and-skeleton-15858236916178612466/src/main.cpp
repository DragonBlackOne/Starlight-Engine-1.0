// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "Engine.hpp"
#include <iostream>

int main() {
    std::cout << "[DEBUG] PASSO 1: ANTES DA INSTANCIACAO" << std::endl;
    std::cout.flush();

    Vamos::Engine engine;
    
    std::cout << "[DEBUG] PASSO 2: APOS INSTANCIACAO / ANTES DO INIT" << std::endl;
    std::cout.flush();

    engine.Initialize();
    
    std::cout << "[DEBUG] PASSO 3: APOS INIT" << std::endl;
    std::cout.flush();

    return 0;
}
