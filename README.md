# Fusion ENGINE Ecosystem

> **Este projeto é feito por IA e só o prompt é feito por um humano.**

Bem-vindo ao repositório raiz da **Fusion ENGINE** (anteriormente derivada da *Starlight* e *Titan* Engines). Este repositório contém um ecossistema completo de desenvolvimento de jogos focado em performance industrial, arquitetura C++ modular e uma API poderosa de Scripting em Lua.

## 🌌 Visão Geral

A Fusion ENGINE foi reconstruída do zero através de uma série contínua de "Phases" focadas na maturação de ferramentas de nível de produção.
O núcleo C++ é 100% agnóstico e independente, lidando primariamente com:

- Renderização Avançada (OpenGL, RenderGraph, Cascaded Shadows, PBR, Isometric Procedural Masking).
- Física Profissional via Jolt Physics.
- Job System Multi-threading para tarefas pesadas (Wicked Engine inspirado).
- Exposição completa via máquina virtual (Lua / Sol2) para permitir que a Lógica de Gameplay seja independente da compilação em C++.

## 🎮 SBA v2.0 — Starlight Bridge API

O framework Lua de alto nível que permite criação rápida de jogos sem tocar em C++:

| Sistema | Descrição |
|---------|-----------|
| **Entity** | Wrapper OO para entidades 3D com métodos encadeáveis (`Entity("Tower", 5,0,3):setColor(0,0.6,1)`) |
| **Light** | Wrapper OO para luzes dinâmicas (`Light(10,20,10, 1,1,1, 1200)`) |
| **Tween** | Animação com 8 funções de easing (`Tween.to(entity, {y=5}, 1.5, "easeOutElastic")`) |
| **Scene** | State machine para cenas (`Scene.register("Game", {...}); Scene.switch("Game")`) |
| **Events** | Pub/Sub para comunicação desacoplada (`Events.on("hit", fn); Events.emit("hit", data)`) |
| **Coroutine** | Sequências assíncronas (`Coroutine.start(fn); Coroutine.wait(2.0)`) |
| **Color** | HSV, pulse, blend (`Color.hsv(0.5, 0.8, 1.0)`) |
| **ScreenShake** | Feedback de câmera 2D (`ScreenShake.trigger(15, 0.3)`) |
| **ValueTween** | Animação de valores arbitrários (`ValueTween.to(obj, "alpha", 0, 1.5)`) |
| **MathX** | 11 funções utilitárias (clamp, lerp, smoothstep, remap, distance3D...) |
| **Physics2D** | Detecção de colisão 2D (AABB, Circle, PointInRect, RayCircle) |
| **Timer** | Temporizadores com repetição e cancelamento |

## 📂 Estrutura do Repositório

| Diretório | Descrição |
|-----------|-----------|
| **[`StarlightEngine/`](StarlightEngine/README.md)** | O núcleo C++ da Fusion Engine. Contém todos os sistemas (Core, Renderer, Physics, Network, ECS, Audio) + framework SBA v2.0. |
| **[`CapitalOdyssey/`](CapitalOdyssey/)** | Simulação cibernética de mercado financeiro 3D. Showcase principal com Entity, Tween, Scene, Events. |
| **[`Snake_Project/`](Snake_Project/README.md)** | Cyber Snake 2D com combo system, ScreenShake, rainbow HSV body. |
| **[`Tetris_Project/`](Tetris_Project/README.md)** | Tetris Arcade com SRS, Ghost Piece, DAS, combo system e Back-to-Back bonus. |
| **[`Pong_Project/`](Pong_Project/README.md)** | Pong Neon com power-ups, rally counter, AI opponent e speed indicator. |

## 🚀 Compilação e Build

### Pré-requisitos

- **Sistema Operacional:** Windows
- **Ferramentas:** CMake (3.20+), MSVC (Visual Studio 2022 / Build Tools)
- **Bibliotecas Base (Thirdparty):** EnTT, ImGui, Jolt Physics, SDL2, glad, Lua/Sol2.

### Build Automatizado

```powershell
# Compila TODOS os projetos + sincroniza o framework SBA
.\build_all.ps1
```

O script automaticamente:
1. Sincroniza `core.lua` e `sba_bridge.lua` para todos os projetos
2. Compila a StarlightEngine (biblioteca estática)
3. Compila todos os jogos (CapitalOdyssey, Pong, Tetris, Snake)

### Criar Novo Projeto

```powershell
cd StarlightEngine
.\create_project.ps1 -ProjectName "MeuJogo"
```

Isso gera um projeto completo com SBA v2.0 pré-instalado, starter script com Scene+Entity+Tween, e CMakeLists.txt configurado.

## 📜 Licença e Propriedade

Este projeto é arquitetado autonomamente por Inteligência Artificial sob as restrições e guias arquiteturais ditados pelos Prompts originais do usuário. Todo o ecossistema segue a filosofia de escalabilidade industrial.

NOTA DO CORNO QUE FEZ O PROJETO

Fiz esse projeto no meu tempo livre no ultimo 1 ano só  uma ideia que eu tive e fui ver se era possivel fazer uma engine de jogos usando só inteligencia articial e minha pouca esperiência com prompt de ia depois de muito errar e recomeçar acho que ja da para fazer algumas coisas com o estado atual do projeto feitos por ia e usando a propria engine tem uma serie de joogos arcade 2d simples executando o GameSuite que está na pasta examples/StarlightShowcase e uma simulação 3d simples tambem na pasta examples vejam ate onde da para ir com os recursos atuais da engine me falta conhecimento para estrair melhores resultados com o projeto  não sei se isso sea útil para alguém, mas se for e você quizer  me deixe uma doação por ser gerado por ia só com minha supervisão e eu escrevendo sentenas  de milhares de linhas de prompt o projeto é open source, não lembro quantas ias usei, mas foram muitas e muitas o pix para doação é 5af3d0ff-4200-4613-9455-ee36a06737bc  obrigado e espero que minha ideia maluca seja útil para alguém