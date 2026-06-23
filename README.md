# Fusion ENGINE Ecosystem

![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)
![C++ Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![Lua Support](https://img.shields.io/badge/Lua-5.4-orange.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)
![Graphics API](https://img.shields.io/badge/Graphics-OpenGL%204.5-red.svg)
![Physics Engine](https://img.shields.io/badge/Physics-Jolt%20Physics-blueviolet.svg)

> **Este projeto é feito por IA e o direcionamento e prompts foram feitos por um humano.**

Bem-vindo ao repositório raiz da **Fusion ENGINE** (anteriormente derivada da *Starlight* e *Titan* Engines). Este repositório contém um ecossistema completo de desenvolvimento de jogos focado em performance industrial, arquitetura C++ modular e uma API poderosa de Scripting em Lua.

---

## 🏗️ Arquitetura do Sistema

O ecossistema é dividido entre o núcleo C++ de alta performance e a ponte de scripting em Lua (SBA):

```mermaid
graph TD
    subgraph StarlightEngine Core (C++20)
        Engine[Engine Loop]
        Window[Window & Input]
        Renderer[OpenGL Renderer / RenderGraph]
        Audio[AudioSystem]
        Physics[Jolt Physics Wrapper]
        ECS[EnTT ECS Registry]
        Scripting[ScriptSystem / Sol2]
    end

    subgraph SBA v2.0 Framework (Lua)
        Bridge[sba_bridge.lua]
        Core[core.lua]
    end

    subgraph Game Projects (Lua + C++ Bootstrap)
        Pong[Pong Project]
        Snake[Snake Project]
        Tetris[Tetris Project]
        Odyssey[Capital Odyssey]
    end

    Engine --> Window
    Engine --> Renderer
    Engine --> ECS
    Engine --> Physics
    Engine --> Audio
    Engine --> Scripting
    
    Scripting <--> Bridge
    Bridge --> Core
    
    Core --> Pong
    Core --> Snake
    Core --> Tetris
    Core --> Odyssey
```

---

## 🌌 Visão Geral

A Fusion ENGINE foi reconstruída do zero através de uma série contínua de "Phases" focadas na maturação de ferramentas de nível de produção.
O núcleo C++ é 100% agnóstico e independente, lidando primariamente com:

- Renderização Avançada (OpenGL, RenderGraph, Cascaded Shadows, PBR, Isometric Procedural Masking).
- Física Profissional via Jolt Physics.
- Job System Multi-threading para tarefas pesadas (Wicked Engine inspirado).
- Exposição completa via máquina virtual (Lua / Sol2) para permitir que a Lógica de Gameplay seja independente da compilação em C++.

---

## 🎮 SBA v2.0 — Starlight Bridge API

O framework Lua de alto nível que permite criação rápida de jogos sem tocar em C++:

| Sistema | Descrição |
| --------- | ----------- |
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

---

## 📂 Estrutura do Repositório

| Diretório | Descrição |
| ----------- | ----------- |
| **[`StarlightEngine/`](StarlightEngine/README.md)** | O núcleo C++ da Fusion Engine. Contém todos os sistemas (Core, Renderer, Physics, Network, ECS, Audio) + framework SBA v2.0. |
| **[`CapitalOdyssey/`](CapitalOdyssey/README.md)** | Simulação cibernética de mercado financeiro 3D com análise gráfica em tempo real e auto-investimento. |
| **[`Snake_Project/`](Snake_Project/README.md)** | Cyber Snake 2D com combo system, múltiplos tipos de comida e placares locais. |
| **[`Tetris_Project/`](Tetris_Project/README.md)** | Tetris Arcade com SRS completo, T-Spin detection, lock delay e efeitos visuais neon. |
| **[`Pong_Project/`](Pong_Project/README.md)** | Pong Neon com rastro dinâmico de partículas, IA com 3 dificuldades (Easy/Medium/Hard) e limite de 11 pontos. |

Documentos úteis na raiz:

- [`CONTRIBUTING.md`](CONTRIBUTING.md) — fluxo de contribuição e convenções.
- [`CHANGELOG.md`](CHANGELOG.md) — registro de mudanças.
- [`AGENTS.md`](AGENTS.md) — convenções de código para IAs e contribuidores.

---

## 🚀 Compilação e Build

### Pré-requisitos

- **Sistema Operacional:** Windows
- **Ferramentas:** CMake (3.20+), MSVC (Visual Studio 2022 / Build Tools)
- **Bibliotecas Base (Thirdparty):** EnTT, ImGui, Jolt Physics, SDL2, glad, Lua/Sol2.

### Build Automatizado

```powershell
# Compila TODOS os projetos + sincroniza o framework SBA
powershell -File build_all.ps1
```

O script automaticamente:

1. Sincroniza `core.lua` e `sba_bridge.lua` para todos os projetos.
2. Compila a StarlightEngine (biblioteca estática).
3. Compila todos os jogos (CapitalOdyssey, Pong, Tetris, Snake).

### Criar Novo Projeto

```powershell
cd StarlightEngine
powershell -File create_project.ps1 -ProjectName "MeuJogo"
```

Isso gera um projeto completo com SBA v2.0 pré-instalado, starter script com Scene+Entity+Tween, e CMakeLists.txt configurado.

---

## 📜 Licença e Propriedade

Todo o ecossistema da **Fusion ENGINE** (incluindo o motor StarlightEngine, o framework de scripting SBA e todos os projetos de jogos anexos como Capital Odyssey, Snake, Tetris e Pong) é 100% de código aberto e disponibilizado sob os termos da **Licença MIT**. Veja o arquivo [LICENSE](LICENSE) para obter o texto completo da licença.

Este projeto é arquitetado autonomamente por Inteligência Artificial sob as restrições e guias arquiteturais ditados pelos Prompts originais do usuário. Todo o ecossistema segue a filosofia de escalabilidade industrial.

### ✉️ Nota do Autor

Desenvolvi este projeto no meu tempo livre durante o último ano como um experimento pessoal para testar os limites do desenvolvimento de jogos utilizando inteligência artificial sob a minha supervisão direta. Após muitas iterações, erros e recomeços, o ecossistema atingiu um estado bastante funcional. O projeto contém jogos clássicos adaptados em 2D e simulações em 3D demonstrando o poder da integração Lua/C++. O projeto é 100% de código aberto. Se este trabalho for útil ou inspirador para você, sinta-se à vontade para apoiar o desenvolvimento contínuo através de uma doação PIX para a chave: **5af3d0ff-4200-4613-9455-ee36a06737bc**. Muito obrigado!