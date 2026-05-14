# Starlight Pong Project
> **Este projeto é feito por IA e só o prompt é feito por um humano.**

O **Pong Project** atua como o laboratório de testes fundacional (Hello World) da Fusion Engine. Ele foi o primeiro jogo desenvolvido para comprovar a robustez e eficiência da máquina virtual Lua operando sob o framework em C++.

## 🏓 O Projeto

A implementação descarta qualquer física exagerada em favor de puro controle algébrico no plano 2D (`MathX.lua` e `Physics2D.lua`). O ambiente adota uma paleta vaporwave/synthwave estrita, renderizando raquetes em glow neon e calculando colisão AABB frame-a-frame contra uma bola em alta aceleração.

### Destaques Técnicos
- **State Machine Baseada em Lua:** A lógica do pong altera dinamicamente entre as fases de Main Menu, Playing e Pause, exibindo como o Lua gerencia loops sem interferir na Engine C++.
- **UI Responsiva via ImGui:** Os placares e botões interativos utilizam wrappers nativos customizados (`gfx.draw_quad`) unificados com o ponteiro do Mouse capturado via Input System.
- **Audio Hooking:** Sincronia perfeita entre a detecção de colisão AABB da bola e a invocação em C++ (`audio.play_sound`) para emissão sonora fluida.

## 🚀 Como Executar

Se você desejar validar rapidamente a integridade do compilador Starlight e confirmar que o input está respondendo:
```bash
# Na raiz da engine:
cd Pong_Project
cmake --build build --config Release
.\build\Release\Pong_Project.exe
```
