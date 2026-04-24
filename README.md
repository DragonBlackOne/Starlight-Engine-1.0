<!-- Este projeto é feito por IA e só o prompt é feito por um humano. -->
# ♾️ Fusion ENGINE: Multiverse Singularity

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
![License](https://img.shields.io/badge/license-MIT-orange.svg)

> [!IMPORTANT]
> **AI-Generated Project Notice**
> Este projeto é feito por **IA** e só o **prompt** é feito por um humano.

## 🌌 Visão Geral

O **Fusion ENGINE** é uma suíte de desenvolvimento de jogos e simulações de alto desempenho, consolidada a partir de mais de 15 repositórios e tecnologias industriais. O núcleo do projeto é o **TitanEngine**, um simulador PBR (Physically Based Rendering) de nível AAA projetado para estabilidade e escala multiversal.

---

## 🛠️ Componentes do Ecossistema

| Motor | Propósito | Principais Tecnologias |
| :--- | :--- | :--- |
| **TitanEngine** | Simulação 3D Industrial | C++20, Jolt Physics, Vulkan/OpenGL, ECS (EnTT), Lua |
| **Starlight Engine** | Framework Retro/Mini-games | C puro, OpenGL 2.1+, Chiptune Audio |
| **Quimera Project** | Pesquisa e Skeleton Engine | Arquitetura Modular, Testes de Integração |

---

## 🚀 TitanEngine: Principais Funcionalidades

### 🎨 Visual Suite (Phase 10 AAA)
- **PBR Rendering**: Materiais fisicamente realistas com suporte total a Albedo, Metallic, Roughness e AO.
- **Post-Processing**: Bloom (High-quality Gaussian), SSAO, SSR (Screen Space Reflections), e Tonemapping ACES.
- **Instanced Rendering**: Capacidade de processar 50+ entidades PBR dinâmicas com performance otimizada para GPUs integradas e dedicadas.
- **Cinema Lighting**: Rig de 4 luzes cinematográficas (Key, Fill, Rim, Bounce) integrado.

### ⚙️ Physics & Core
- **Jolt Physics**: Integração de física industrial com suporte a milhares de corpos rígidos e alocação de memória temporária de 64MB.
- **Job System**: Baseado no Wicked Engine para multithreading massivo.
- **Scripting (Lua)**: Lógica dirigida por scripts com bindings avançados para sistemas de câmera e entidades.
- **ECS (EnTT)**: Arquitetura baseada em entidades e componentes para máximo cache-locality.

---

## 🎮 Ricochete (Showcase Game)

Dentro do ecossistema Fusion, incluímos o **Ricochete**, um minijogo que demonstra as capacidades da engine:
- **Modo 2D**: Rodando via ImGui Foreground DrawList com efeitos CRT e Scanlines.
- **Modo 3D**: Arena Neon com chão espelhado e física de impacto Jolt.

---

## 🔨 Como Compilar

O projeto utiliza **CMake** para orquestração de build.

1. **Pré-requisitos**:
   - Visual Studio 2022 (ou compilador C++20 compatível).
   - CMake 3.20+.
   - Dependências externas (SDL2, Jolt, Lua, EnTT, GLM).

2. **Passos**:
   ```powershell
   cmake -B build_new -S TitanEngine
   cmake --build build_new --config Release
   ```

3. **Execução**:
   Localize o executável `TitanEditor.exe` na pasta `build_new/Release` e execute-o.

---

## 📜 Licença

Este projeto utiliza licenças permissivas (MIT/Zlib) para todas as suas dependências, permitindo o uso industrial e educacional.

---
*Gerado com ❤️ por Antigravity (Advanced Agentic Coding AI).*
