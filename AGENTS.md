# Starlight Engine — Development Guidelines (v15.0.0)

## Project Overview

Starlight Engine is a high-performance C++20 game engine built with SDL2, OpenGL, Lua/Sol2, EnTT ECS, and Jolt Physics.
The core library is **StarlightCore** (static lib). Game projects link against the prebuilt `.lib`.

## Project Structure

```text
Starlight Engine/
├── StarlightEngine/       # Core engine (v15.0.0)
│   ├── include/           # Public headers (.hpp)
│   ├── src/               # Implementation (.cpp)
│   │   ├── ai/            # NavigationSystem (A*)
│   │   ├── audio/         # AudioSystem (Procedural FM & Chiptune)
│   │   ├── core/          # Engine, Window, Input, Log, Version
│   │   ├── ecs/           # ECS systems (SpriteRenderer, Camera2D, Tilemap)
│   │   ├── gui/           # ImGui integration
│   │   ├── network/       # Multiplayer systems
│   │   ├── physics/       # Jolt Physics wrapper
│   │   └── renderer/      # OpenGL renderer (2D + 3D)
│   ├── thirdparty/        # Vendored: imgui, nlohmann/json, pl_mpeg, wicked
│   ├── tests/             # GoogleTest unit tests
│   └── assets/            # Default scripts, shaders, fonts, audio
├── Pong_Project/          # Pong game (Lua, 2D)
├── Snake_Project/         # Snake game (Lua, 2D)
├── Tetris_Project/        # Tetris game (Lua, 2D)
├── CapitalOdyssey/        # Market simulation (Lua, 2D)
├── FusionFight/           # Fighting game (Lua, 2.5D)
├── GodHand_Project/       # 3D Beat 'Em Up (Lua, 3D)
└── TechDemo_Project/      # 3D Architecture Showcase (Lua, 3D)
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
| :--- | :--- |
| `python starlight.py build [target]` | Unified build orchestrator (engine, all, or specific game) |
| `python starlight.py run <game>` | Launch standalone game with working directory safety |
| `python starlight.py test` | Execute GoogleTest suite with health reporting |
| `python starlight.py lint` | Static analysis across all Lua game scripts |
| `python starlight.py validate` | Validate asset integrity, shaders, and configs |
| `python starlight.py stats` | Codebase telemetry (C++, Lua, Python, Shaders) |
| `python starlight.py new-game <Name>` | Scaffold a standardized 2D/3D game project |
| `cmake --build build --config Release --parallel` | Quick engine rebuild |
| `powershell -File build_all.ps1` | Full rebuild (engine + all games) |

## Code Conventions

- **Naming:** PascalCase for classes, camelCase for methods/vars, `m_` prefix for members
- **Style:** Google style, 4-space indent, 120 columns
- **Headers:** `#pragma once`, minimize includes, prefer forward declarations
- **C++20:** Use `[[maybe_unused]]`, designated initializers, concepts where appropriate
- **Math:** `glm::vec3`, `glm::mat4` throughout
- **ECS:** EnTT registry with `TransformComponent` on every entity
- **3D Rotation in Lua:** In Lua scripts, `engine.set_rotation(id, rx, ry, rz)` expects Euler angles in **radians** (`glm::quat(glm::vec3(rx, ry, rz))`). Always convert degrees to radians via `math.rad(deg)`.
- **Errors:** Log via `Log::Info/Warn/Error`, return `bool` for initialization
- **Tri-Language Graphics Synergy**: C++ provides the high-performance RHI and procedural vertex/tangent generators; Lua exposes high-level scene composition, lighting atmospheric presets, and character kinematics; Python provides automated asset generation, normal map baking (Sobel), and asset pipeline validation.

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
6. For 3D projects, use `add_starlight_game(TargetName)` without `MODE_2D` and ensure full 3D shader synchronization
7. Use `[[maybe_unused]]` and `(void)param;` for any unused virtual override parameters to comply with MSVC `/WX`
8. Run `clang-format` on changed files before asking to commit

## Testing

- Tests use GoogleTest, enabled via `-DSTARLIGHT_BUILD_TESTS=ON`
- Test files go in `StarlightEngine/tests/`
- Test target: `StarlightTests`

## Commercial Game Engine Architecture Roadmap

When developing or expanding engine systems, follow the staged modular architecture:

1. **Module 1**: Directory Structure, SIMD Math Library (Vec/Mat/Quat), and Custom Allocators (Arena, Stack, Pool, Double-Buffer, TLS).
2. **Module 2**: Windowing (SDL/GLFW), Event Broker, and Multi-platform RHI Abstraction.
3. **Module 3**: Core ECS (Sparse Sets / Archetypes) and Scene Graph.
4. **Module 4**: 2D Sprite Batching and 3D PBR Deferred/Forward+ Render Pipelines.
5. **Module 5**: Asset Pipeline with Asynchronous glTF/Texture Loading & Hot-Reloading.
6. **Module 6**: Physics Engine Integration (Jolt 3D / Box2D 2D) and Lua/C# Scripting Runtime.
7. **Module 7**: Dear ImGui WYSIWYG Editor, Viewport, Hierarchy, and GPU/CPU Profiler.
8. **Module 8**: Character & Graphics Toolset for procedural 3D character sculpting (`character_studio.py`), normal/AO texture baking (`texture_baker.py`), atmospheric sky generation (`sky_baker.py`), and runtime model loading (`engine.spawn_model`).

Always validate unit tests and full compilation before advancing across module boundaries.

## Standalone Game Execution Standards

1. **Launchers**: Every game project in `Fusion ENGINE/` must provide:
   - A `play_<project>.bat` at the repository root using `cd /d "%~dp0<ProjectName>"` and `start "" "%~dp0build\<ProjectName>\Release\<ProjectName>.exe"`.
   - A `run_<project>.ps1` in the project's folder for PowerShell execution.
2. **Working Directory**: Standalone executables must always run with `-WorkingDirectory` set to the project root so asset virtual paths (`assets/scripts/...`, `assets/textures/...`) resolve without error.
3. **F5 Hot-Reloading**: Game scenes in C++ should catch `pal::KeyCode::F5` to clear ECS registries, reset scripting runtime, and hot-reload `assets/scripts/main.lua` or game scripts instantly.
4. **Arcade Quality**: Games must implement complete game loops (Title Screen, Mode Selection, Audio SFX, Screen Shake, Particle Systems, High Scores, and GameOver/Victory screens).

## Game Export & Store Distribution Standards (Steam / Itch.io / Standalone)

1. **Unified Exporter CLI**: Export distribution packages via `python starlight.py export <game> [--zip] [--steam] [--appid <ID>] [--pak]`.
2. **Distribution Structure**: Exported packages in `dist/<GameName>/` must be 100% self-contained and run directly without IDE or engine source dependencies.
3. **SteamPipe Integration**: Passing `--steam` automatically writes `steam_appid.txt`, SteamPipe manifests (`app_build_<appid>.vdf`, `depot_build_<depotid>.vdf`), and deployment scripts for `steamcmd`.
4. **Clean Asset Tree**: Packaging must sanitize non-production assets (`*.tmp`, `*.log`, `.gitkeep`, scratch files) or optionally pack into a single `game.pak` VFS archive.
5. **Standalone ZIP Archive**: Passing `--zip` creates an optimized `.zip` archive ready for upload to Itch.io, GOG, or direct distribution.

## Standalone 2D Arcade Game Architecture Standards

1. **Projection & Viewport**: In 2D game scenes (`mode2D = true`), always invoke `renderer.SetOrthoProjection(w, h)` in `OnEnter()` to preserve exact pixel-space mapping (e.g. 1920x1080) and avoid perspective camera overrides.
2. **System Disabling**: Always set `[Systems] EditorSystem=false` in project `engine.ini` files for standalone game builds so games render clean on desktop outside editor docking interfaces.
3. **Class Architecture**: Use `Class()` factory for OOP entities (`Paddle`, `Ball`, `ParticleSystem`, `PowerUp`, `GameManager`) with `Init()`, `Update(dt, ...)`, and `Draw(ox, oy)` lifecycle hooks.
4. **Dynamic Audio**: Leverage procedural FM synthesis via `audio.play_synth` for zero-asset dynamic SFX and tempo-scaling synthwave BGM.
5. **Sound Suite Completeness**: Standalone 2D arcade games must provide comprehensive procedural sound coverage:
   - UI: Navigation blip, selection confirm chime, menu back.
   - Gameplay: Countdown ticks (3-2-1-GO!), paddle hit pitch modulation based on contact point and rally depth, wall bounce, smash impact, hazard deflections, and goal explosions.
   - Outcomes: Melodic victory fanfares and defeat cadences.
6. **Classic Mode as Primary Anchor**: Arcade titles must feature an authentic 1:1 Classic Mode (e.g. 1972 segmented score layout, authentic physics, monochrome/phosphor CRT palettes) positioned as the prominent default option.

## Multi-Threading & Asynchronous Job System Standards (v15.0.0)

1. **Job Dispatching**: Heavy simulations (particle systems, mesh deformation, spatial queries, background asset streaming) must use `JobSystem::Dispatch` or `JobSystem::ParallelFor` across available CPU worker threads.
2. **Lua Concurrency Interface**: Game scripts can distribute compute-heavy tasks via `jobs.dispatch(func, priority)` and partition array iterations via `jobs.parallel_for(count, func, chunkSize)`.
3. **Decal Projection**: Visual surface markings (impact craters, energy burns, blood splatters, tire/footprints) must be spawned via `decals.spawn(x, y, z, sx, sy, sz, texture, lifetime, blendMode)` to avoid costly vertex geometry duplication.
4. **Real-time Profiler**: Use `STARLIGHT_PROFILE_SCOPE("ScopeName")` in C++ and `profiler.begin_sample / end_sample` in Lua to instrument hot code paths for live display in the 120-frame oscilloscope ring buffer.

## Runtime Diagnostics, Telemetry & Reflection (v15.0.0)

1. **Version Queries**: In C++, use `Engine::GetVersionString()` / `Engine::GetVersionCodename()`. In Lua scripts, use `engine.version` or `engine.get_version_info()`.
2. **Live Telemetry & Profiling**: Lua scripts can query engine health in real time via `engine.get_telemetry()` or access circular frame-time histories via `profiler.get_history()`.
3. **Subsystem Reflection**: Lua scripts can inspect active subsystems via `engine.get_systems()` or check individual subsystem status with `engine.is_system_active("JobSystem")`.

## Version Evolution & Release Policy

Every version update (major or minor) must never be a purely cosmetic version number change. Each version release must strictly deliver substantive value across three mandatory pillars:

1. **Novidades (New Features & Capabilities)**:
   - Introduction of new C++ engine subsystems, high-performance RHI/ECS/Audio/Physics APIs, Lua scripting runtime bindings, developer CLI tools, or advanced rendering features.
2. **Melhorias (Improvements & Optimizations)**:
   - Measurable CPU/GPU performance gains, memory/allocator optimizations, architectural refactoring, ergonomic API redesigns, extended test suites, and bug fixes.
3. **Atualizações (Upgrades & Ecosystem Synchronization)**:
   - Updating existing game projects to leverage newly introduced engine APIs, synchronizing CMake files, CLI orchestrators (`starlight.py`), automated QA suites, `CHANGELOG.md`, and technical documentation.

### Version Scope & Milestone Tiers

- **Major Epoch Milestones (Ending in `.0`, e.g. v10.0, v11.0, v12.0...)**:
  - Grand, massive, era-defining architectural evolutions that inaugurate a spectacular era of technological evolution. Introduces revolutionary engine subsystems, foundational RHI/ECS paradigms, major new rendering pipelines, or transformative developer workflows.
- **Mid-Cycle Evolutionary Updates (Ending in `.5`, e.g. v10.5, v11.5, v12.5...)**:
  - Medium/substantive evolutionary updates. Expands existing subsystems, adds new gameplay mechanics, introduces new developer tools, or executes major performance optimizations.
- **Iterative & Incremental Refinements (Ending in `.1` to `.4` and `.6` to `.9`, e.g. v11.1, v11.2, v11.3, v11.4...)**:
  - Focused, smaller/incremental updates. Adds ergonomic APIs, specific procedural sound improvements, bug fixes, localized optimizations, and ecosystem synchronizations.

## 3D PBR Shading, Lighting & Post-Processing Guardrails

1. **IBL Fallback Safety**: Shaders employing Image-Based Lighting (`pbr.frag`, `deferred_light.frag`) must always guard environment texture lookups with a `useIBL` uniform. When no HDR cubemap is loaded, fallback to a physical ground/sky spherical gradient ambient model to prevent blinding white overexposure.
2. **Bloom Energy Normalization**: Multi-pass bloom downsampling must strictly conserve energy using normalized filter weights (e.g. Jimenez 13-tap filter summing to exact 1.0) and Karis average suppression on the first mip to eliminate subpixel fireflies.
3. **Tonemapping Pipeline**: ACES Filmic tonemapping must operate directly on linear HDR color scaled by scene exposure. Avoid combining exponential Reinhard with ACES to prevent crushing dynamic range and flattening contrast.
4. **Character & Environment Texture Mapping**: 3D character rigs and primary environment geometry must use texture maps (albedo, normal, roughness) with valid UV coordinates instead of plain flat vertex colors to deliver production-grade visual fidelity.
5. **Photorealistic Soft Shadows & Normal Maps**: Directional lighting must utilize Cascaded Shadow Maps with 16-tap Poisson disk filtering to prevent harsh stair-stepping artifacts, and all PBR materials should provide tangent-space normal maps for geometric surface relief.

## 3D Open-World Character Locomotion & Free Camera Standards

1. **Camera-Relative Locomotion**: 3D third-person character movement must always calculate movement vectors relative to camera horizontal forward/right axes (`camForward`, `camRight`), with character yaw smoothly interpolating towards target velocity heading.
2. **Multi-Tier Locomotion Speeds**: Open-world characters must support smooth continuous gait blending: Walk (`Alt` / 3.0 m/s), Run (WASD / 6.5 m/s), and High-Velocity Sprint (`Shift` / 11.5 m/s) with terrain-adaptive Foot IK.
3. **360° Third-Person Orbit Camera**: Exploration cameras must provide full 360° yaw/pitch mouse orbit with customizable pitch clamps (-40° to +75°), smooth spring-arm damping, and mouse-wheel distance zooming (2.0m to 12.0m).
4. **High-Fidelity Anatomical Rigging**: Humanoid character rigs must incorporate distinct anatomical joint structures (head with facial/hair geometry, muscular chest/torso, deltoids, biceps/forearms with wraps/gauntlets, detailed fists, pelvic belt, kneecaps, and treaded boots) with custom PBR albedo, roughness, metallic, and subsurface skin scattering.

## Period-Accurate 3D Character & Environment Replication Standards

1. **Character Signature Silhouettes**: When modeling iconic classic characters, faithfully reproduce their key defining garments (e.g. duster coats with chevron embroidery and fringe tassels, inner scarves/ascots, distinct armored gauntlets, and facial bandages/accessories) using hierarchical geometry and PBR material separation.
2. **Secondary Motion & Cloth Physics**: Long garments (coat tails, scarves, hair tufts) must implement spring-damper inertia physics driven by character acceleration, velocity, angular yaw changes, and vertical jumps.
3. **Authentic Period HUD & UI Layouts**: Match the original game's UI layout, including bottom-corner circular gauges with animated emblems (e.g. Level DIE skull with color tier shifts), segmented health meters, compass radars, and stylized roulette menus with sweeping time hands.





