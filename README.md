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

## 📂 Estrutura do Repositório

O repositório é separado em módulos principais. A Engine em si serve como um SDK estático (Library) vinculado externamente aos projetos.

| Diretório | Descrição |
|-----------|-----------|
| **[`StarlightEngine/`](StarlightEngine/README.md)** | O núcleo C++ da Fusion Engine. Contém todos os sistemas (Core, Renderer, Physics, Network, ECS, Audio). |
| **[`CapitalOdyssey/`](CapitalOdyssey/README.md)** | Uma simulação cibernética 2.5D focada no mercado financeiro. Nosso projeto "AAA" de Showcase visual. |  esse jogo ainda está extremamente quebrado nestá versão.
| **[`Tetris_Project/`](Tetris_Project/README.md)** | Implementação Arcade em Lua do Tetris clássico, suportando Ghost Piece, Hold e Super Rotation System (SRS). |
| **[`Pong_Project/`](Pong_Project/README.md)** | Nosso projeto "Hello World". Demonstra Input e Física 2D simples via Lua Scripting. |

## 🚀 Compilação e Build

A arquitetura de Build é completamente centralizada via **CMake**.

### Pré-requisitos
- **Sistema Operacional:** Windows
- **Ferramentas:** CMake (3.20+), MSVC (Visual Studio 2022 / Build Tools)
- **Bibliotecas Base (Thirdparty):** EnTT, ImGui, Jolt Physics, SDL2, glad, Lua.

### Automação de Build
Para compilar simultaneamente todos os projetos ativos da Engine, utilizamos o script PowerShell incluído na raiz.

```powershell
# Abra um terminal na raiz do diretório Fusion ENGINE
.\build_all.ps1
```

Isso garantirá a configuração do CMake para a `StarlightEngine` e todos os projetos vinculados (Pong, Tetris, CapitalOdyssey) dentro de seus próprios subdiretórios `build/Release/`.

## 📜 Licença e Propriedade
Este projeto é arquitetado autonomamente por Inteligência Artificial sob as restrições e guias arquiteturais ditados pelos Prompts originais do usuário. Todo o ecossistema segue a filosofia de escalabilidade industrial.

NOTA DO CORNO QUE FEZ O PROJETO 

Fiz esse projeto no meu tempo livre no ultimo 1 ano só  uma ideia que eu tive e fui ver se era possivel fazer uma engine de jogos usando só inteligencia articial e minha pouca esperiência com prompt de ia depois de muito errar e recomeçar acho que ja da para fazer algumas coisas com o estado atual do projeto feitos por ia e usando a propria engine tem uma serie de joogos arcade 2d simples executando o GameSuite que está na pasta examples/StarlightShowcase e uma simulação 3d simples tambem na pasta examples vejam ate onde da para ir com os recursos atuais da engine me falta conhecimento para estrair melhores resultados com o projeto  não sei se isso sea útil para alguém, mas se for e você quizer  me deixe uma doação por ser gerado por ia só com minha supervisão e eu escrevendo sentenas  de milhares de linhas de prompt o projeto é open source, não lembro quantas ias usei, mas foram muitas e muitas o pix para doação é 5af3d0ff-4200-4613-9455-ee36a06737bc  obrigado e espero que minha ideia maluca seja útil para alguém