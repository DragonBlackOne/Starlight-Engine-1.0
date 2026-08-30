# Fusion ENGINE — Build/Config & Subsystem Audit Findings

Research-only audit. No source files were modified.

Date: 2026-08-27

## Summary by severity

| ID | Severity | Area | Finding |
|----|----------|------|---------|
| C1 | Critical | CI / CMake | Pong standalone configure always fails (`add_fusion_game` undefined) |
| C2 | Critical | Runtime config | `engine.ini` overrides `mode2D`, forces all 2D games into 3D mode |
| M1 | Major | Asset copy | `add_fusion_game` never copies the game's own `assets/` to build output |
| M2 | Major | Build script | `build_all.ps1 -Clean` deletes the wrong directories |
| M3 | Major | Build script | `build_all.ps1` sync mutates game source trees (robocopy /MIR) |
| m1 | Minor | CMake | Two divergent helper macro families + dead 2D macros |
| m2 | Minor | CMake | Games link `StarlightCore` by target, not prebuilt `.lib` |
| m3 | Minor | Assets | `assets/audio/beep.wav` referenced but missing everywhere (latent) |

---

## C1 — CI Pong standalone configure always fails (`add_fusion_game` undefined)

- `add_fusion_game` is defined in `cmake/FusionGameHelpers.cmake:2`, which is `include()`d **only** by the workspace-root `CMakeLists.txt:34`.
- None of the 6 game `CMakeLists.txt` include the helper; they all call `add_fusion_game(...)` directly, e.g. `Pong_Project/CMakeLists.txt:8`.
- `.github/workflows/build.yml:70-71` configures Pong **standalone**: `cmake -B Pong_Project/build -S Pong_Project`, then builds `--target Pong_Project`.
- Result: CMake configure error "Unknown CMake command add_fusion_game". This CI step always fails.

Grep confirms `add_fusion_game` appears only in:
- `CMakeLists.txt` (comment, line 33)
- `cmake/FusionGameHelpers.cmake` (definition, line 2)
- The 6 game CMakeLists (calls)

## C2 — `engine.ini` overrides `mode2D` and forces 2D games into 3D mode

- `Engine.cpp:150`: `activeConfig.mode2D = configSys->GetBool("Window","mode2D", config.mode2D)` — if the ini loads, its `mode2D` value overrides the in-code value from `main.cpp`.
- `ConfigSystem::Load` returns `true` on a well-formed ini (`ConfigSystem.cpp:73`), so the override always applies for present inis.
- `add_fusion_game` (FusionGameHelpers.cmake:39-43) copies a game-specific `engine.ini` if present, otherwise the engine default. Only **GodHand_Project** has its own `engine.ini` (`mode2D=false`, correct for 3D).
- The engine default (`StarlightEngine/assets/engine.ini:13`) has `mode2D=false`.
- Therefore Pong, Snake, Tetris, CapitalOdyssey, FusionFight all get `mode2D=false` even though their `main.cpp` sets `config.mode2D=true`. Confirmed in build outputs:
  - `build/Pong_Project/Release/assets/engine.ini` → `mode2D=false`
  - `build/FusionFight/Release/assets/engine.ini` → `mode2D=false`
- The renderer then takes the 3D path (`Engine.cpp:226-227`, perspective projection) instead of 2D ortho (`Engine.cpp:224`).

Fix directions: ship `mode2D=true` in each 2D game's ini, or don't let the ini override the code-set `mode2D` for games.

## M1 — `add_fusion_game` never copies the game's own `assets/` to build output

- `POST_BUILD` in FusionGameHelpers.cmake:26-48 copies only:
  - engine `scripts`, `shaders`, `textures`
  - `engine.ini`
- It never copies e.g. `GodHand_Project/assets/*` (stages, rigs, `stage_desert`, character models/textures) or each 2D game's own textures/scripts that live in `assets/`.
- `PathResolver::Resolve` (PathResolver.cpp:55-103) searches only: CWD, exe dir, exe dir parent, exe dir grandparent.
- Games are launched from the build dir (`run_godhand.ps1` sets working dir to `build\GodHand_Project\Release`), so the source-tree `assets/` is unreachable after a **clean** build.
- Game-specific assets currently present in build output are historical remnants, not guaranteed by CMake.

This is the main gap versus `create_project.ps1` + `StarlightProject.cmake` (`add_starlight_game`).

## M2 — `build_all.ps1 -Clean` deletes the wrong directories

- `-Clean` (build_all.ps1:60-63) removes `$Engine\build` and `$Root\<proj>\build` (standalone dirs).
- The script itself configures/buils into the workspace root `build/` (`-B build -S .`, line 118; `--build build`, lines 129/134/148).
- Result: a `-Clean` build leaves the actual workspace `build/` entirely intact — not actually clean.

## M3 — `build_all.ps1` sync mutates game source trees (robocopy /MIR)

- Section 1 (lines 82-93) robocopies engine `scripts/fonts/shaders/audio/textures` **into** each game's source `assets/`.
- Mixes framework and game-owned assets, and over repeated runs produces nested duplicates (e.g. `assets/shaders/shaders/` observed in Snake).

## m1 — Two divergent helper macro families + dead 2D macros

- `add_fusion_game` + `STARLIGHT_2D_ONLY` (FusionGameHelpers.cmake)
- `add_starlight_game` + `STARLIGHT_MODE_2D` (StarlightProject.cmake, used by `create_project.ps1:207`)
- Neither 2D macro appears in any source file; all games set `mode2D` in code. Dead macros that could trigger unintended conditional compilation if ever used.

## m2 — Games link `StarlightCore` by target, not prebuilt `.lib`

- `add_fusion_game` uses `target_link_libraries(... PRIVATE StarlightCore)` (FusionGameHelpers.cmake:7).
- Contradicts AGENTS.md "games link prebuilt StarlightCore.lib". Only works because games are always built inside the workspace where the target exists; no true standalone/link-prebuilt path.

## m3 — Missing `assets/audio/beep.wav` (latent)

- Every game's `sba_bridge.lua` defines `function Sound() Engine.play_sound("assets/audio/beep.wav") end`.
- No `beep.wav` exists in any assets tree or engine defaults.
- No game calls `Sound()`, so it is latent/unused.

---

## Reviewed without build/config defect

- `ecs/`: CameraSystem, Camera2DSystem, SpriteRendererSystem, TilemapSystem, SpriteAnimationSystem, LODSystem
- `ai/`: BehaviorTree, NavigationSystem (noted WorldToGrid/GridToWorld asymmetry — add a unit test)
- `network/NetworkSystem`, `gui/DashboardSystem`, `gui/RmlRenderInterfaceGL3`, `gui/RmlSystemInterfaceSDL`
- `core/Engine.cpp` (except C2), `FightingSystem.cpp`
- `build_all.ps1`, top-level `CMakeLists.txt`, CI `build.yml`

## Root cause

Two **competing/divergent build approaches**: `StarlightProject.cmake`/`create_project.ps1` (`add_starlight_game`, standalone-capable) vs the 6 shipped games' `add_fusion_game` (workspace-only, missing include). The shipped path breaks CI (C1), misses game-asset copying (M1), and inherits the wrong `engine.ini` (C2).

---

## Suggested fixes (not applied)

1. Give each game CMakeLists a self-contained include of the helper, or switch games to `add_starlight_game` (matching `create_project.ps1`).
2. Copy each game's own `assets/` in POST_BUILD, or launch games with the source dir as CWD/asset root.
3. Ensure 2D games ship `mode2D=true` via their own `engine.ini`, or make `Engine.cpp:150` respect the code-set value for games.
4. Make `build_all.ps1 -Clean` remove the workspace `build/`; stop robocopying engine assets into game source trees (or page it under a check).
5. Add `beep.wav` or remove the `Sound()` helper.
