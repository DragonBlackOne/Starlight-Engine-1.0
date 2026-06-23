# Contributing to Fusion ENGINE

Thank you for your interest in contributing! This document covers the workflow
and conventions used in the repository.

## Repository layout

```
Fusion ENGINE/
├── StarlightEngine/      Core C++20 engine (static library + tests + assets)
├── Pong_Project/         Sample 2D project (Lua)
├── Snake_Project/        Sample 2D project (Lua)
├── Tetris_Project/       Sample 2D project (Lua)
└── CapitalOdyssey/       Sample 2D project (Lua)
```

Each game project links the prebuilt `StarlightCore.lib` and reuses the
**SBA v2.0** framework (`assets/scripts/core.lua` + `sba_bridge.lua`).

## Build prerequisites

| Tool | Version |
|------|---------|
| CMake | 3.20 or newer |
| MSVC | Visual Studio 2022 / Build Tools (Windows) |
| External deps | Downloaded to `D:/deps` (override with `STARLIGHT_DEPS_DIR` env var) |

The CI workflow at `.github/workflows/build.yml` lists the exact dependency
URLs (glm, entt, sol2, lua, SDL2, JoltPhysics).

## Build commands

```powershell
# Quick engine rebuild
cmake --build build --config Release --parallel

# Full pipeline (engine + all games)
powershell -File build_all.ps1

# Engine + GoogleTests
cmake -B build -S StarlightEngine -DSTARLIGHT_BUILD_TESTS=ON
cmake --build build --config Release --parallel
ctest --test-dir build -C Release
```

## Coding conventions

- C++20, Google style, 4-space indent, 120 column limit.
- PascalCase for classes, camelCase for methods/vars, `m_` prefix for members.
- `#pragma once` in every header. Prefer forward declarations over includes.
- Use `Log::Info/Warn/Error` for diagnostics. Initialization routines return
  `bool` for success/failure.
- ECS components live next to the system that owns them under `src/`.
- New systems must be registered in `src/core/Engine.cpp` (see AGENTS.md).

## Working with Lua / SBA

`StarlightEngine/assets/scripts/core.lua` and `sba_bridge.lua` are the single
source of truth for the scripting API. `build_all.ps1` syncs them into every
game project's `assets/scripts/`. Always edit the engine copy — never edit
the per-project copies directly.

## Before opening a pull request

1. Run the full build (`build_all.ps1`) and ensure it succeeds.
2. If you changed C++ headers, run `clang-format` on the affected files
   (style config in `.clang-format`).
3. If you added a new system, add a GoogleTest under `StarlightEngine/tests/`.
4. Keep commits focused. One logical change per commit.

## Reporting issues

Use GitHub issues. For engine bugs, include:
- Target OS and compiler version
- `cmake` / `cmake --build` output (Release config)
- Minimal reproduction (project + script)
