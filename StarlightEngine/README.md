# Starlight Engine (Fusion Core)
> **Este projeto é feito por IA e só o prompt é feito por um humano.**

A **Starlight Engine** atua como o coração tecnológico do ecossistema Fusion. Trata-se de uma framework de simulação de alto desempenho desenvolvida em C++17 moderno. Seu design é estritamente orientado a **Entity-Component-System (ECS)** e adota o padrão de Sub-sistemas Desacoplados.

## ⚙️ Arquitetura e Sub-Sistemas

A Engine é gerenciada por um objeto central (Singleton) `starlight::Engine` que itera sobre o Ciclo de Vida dos sistemas nativos via `Initialize`, `Update`, `FixedUpdate` e `Render`.

### 1. ECS (Entity Component System)
- Potencializado pela biblioteca **EnTT**.
- Evita a pesada orientação a objetos tradicionais em prol de memória contígua e iterações de cache-friendly para GameObjects.
- `TransformComponent`, `MeshComponent`, `PointLightComponent`, `AudioComponent`, e `LuaScriptComponent`.

### 2. RenderGraph e Renderer (OpenGL)
- Pipeline focado em **PBR (Physically Based Rendering)** e Deferred/Forward misto.
- Conta com **Shadow System** (Cascaded Shadow Maps) operando em resoluções industriais (2048x2048 em 4 cascatas).
- `Renderer2D` otimizado para chamadas em batch, garantindo interfaces neons, sprites clássicos e geração procedural 2.5D isométrica (sem texturas externas caso ocorram falhas).
- GPU Culling e Frustum Culling em fases nativas.

### 3. Máquina Virtual e Scripting (Lua / Sol2)
O coração da versatilidade do usuário reside no **ScriptSystem**. Quase todo o escopo matemático, de input, de física e renderização é mapeado em bindings de Sol2.
- A Engine injeta a classe global `Engine` no estado LUA, e tabelas embutidas como `gfx`, `input`, `window`, e `audio`.
- Isso garante que a reconstrução de mecânicas de gameplay não exija re-compilação longa do C++.

### 4. InputSystem
Lida com eventos via SDL2 integrados a uma arquitetura de "Binding de Ações", garantindo que inputs brutos (`pal::KeyCode::W`) se transformem em abstrações semânticas (`"Up"`, `"Left"`, `"Hold"`).

## 🔨 Integração de CMake
A Starlight é construída primariamente como uma biblioteca estática (`.lib`) que os outros executáveis (como `Tetris_Project.exe`) fazem "link" em tempo de compilação.
Suas dependências e headers isolados (`/include` e `/src`) garantem um padrão de projeto limpo.
