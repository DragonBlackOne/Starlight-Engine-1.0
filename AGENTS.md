# Fusion ENGINE — Development Guidelines

## Project Overview

Fusion ENGINE is a C++20 game engine built with SDL2, OpenGL, Lua/Sol2, EnTT ECS, and Jolt Physics.
The core library is **StarlightCore** (static lib). Game projects link against the prebuilt `.lib`.

## Project Structure

```
Fusion ENGINE/
├── StarlightEngine/       # Core engine
│   ├── include/           # Public headers (.hpp)
│   ├── src/               # Implementation (.cpp)
│   │   ├── ai/            # NavigationSystem (A*)
│   │   ├── audio/         # AudioSystem
│   │   ├── core/          # Engine, Window, Input, Log
│   │   ├── ecs/           # ECS systems (SpriteRenderer, Camera2D, Tilemap)
│   │   ├── gui/           # ImGui integration
│   │   ├── network/       # Multiplayer systems
│   │   ├── physics/       # Jolt Physics wrapper
│   │   └── renderer/      # OpenGL renderer (2D + 3D)
│   ├── thirdparty/        # Vendored: imgui, nlohmann/json, pl_mpeg, wicked
│   ├── tests/             # GoogleTest unit tests
│   └── assets/            # Default scripts, shaders, fonts, audio
├── Pong_Project/          # Pong game (Lua)
├── Snake_Project/         # Snake game (Lua)
├── Tetris_Project/        # Tetris game (Lua, 2D)
└── CapitalOdyssey/        # Market simulation (Lua, 2D)
```

## Build System

- **CMake 3.20+**, **MSVC**, **C++20**
- Game projects link **prebuilt StarlightCore.lib** (do NOT use `add_subdirectory`)
- Build order: `StarlightEngine` → game projects (individually)
- Use `--parallel` for multi-core compilation (`/MP` enabled)
- Precompiled headers via `StarlightCorePCH.h`
- Unity/jumbo build enabled for StarlightCore

### Useful Commands

| Command | Description |
|---------|-------------|
| `cmake --build build --config Release --parallel` | Quick engine rebuild |
| `powershell -File build_all.ps1` | Full rebuild (engine + all games) |
| `cmake -B build -S . -DSTARLIGHT_BUILD_TESTS=ON` | Enable tests |

## Code Conventions

- **Naming:** PascalCase for classes, camelCase for methods/vars, `m_` prefix for members
- **Style:** Google style, 4-space indent, 120 columns
- **Headers:** `#pragma once`, minimize includes, prefer forward declarations
- **C++20:** Use `[[maybe_unused]]`, designated initializers, concepts where appropriate
- **Math:** `glm::vec3`, `glm::mat4` throughout
- **ECS:** EnTT registry with `TransformComponent` on every entity
- **Errors:** Log via `Log::Info/Warn/Error`, return `bool` for initialization

## Visual & Aesthetic Standards

- **Theme:** Outrun / Retro Synthwave Cyberpunk style (Hot Magenta, Cyber Cyan, Neon Violet, Sunset Orange, Gold, Obsidian).
- **Guidelines:** Refer to the comprehensive [AESTHETICS.md](file:///d:/Projetos/Fusion%20ENGINE/AESTHETICS.md) guide before writing or editing any rendering logic, stage assets, UI layouts, or particle effects.
- **Slicing & Aspect Ratio:** Spritesheets must use a 4x2 grid of 256x512 pixels with a 1:2 aspect ratio (`drawW = h * 0.5`) to prevent waist cutoff and stretching.

## When Modifying Code

1. Understand the full file's context and conventions before editing
2. Keep changes minimal and focused on the task
3. Always run a build after changes to verify compilation
4. If adding new systems, register them in `Engine.cpp`
5. For 2D-only projects, set `config.mode2D = true` in the project's `main.cpp`
6. Run `clang-format` on changed files before asking to commit

## Testing

- Tests use GoogleTest, enabled via `-DSTARLIGHT_BUILD_TESTS=ON`
- Test files go in `StarlightEngine/tests/`
- Test target: `StarlightTests`
