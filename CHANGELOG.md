# Changelog

All notable changes to the **Fusion ENGINE** ecosystem are documented in this
file. The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and the project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [15.0.0] - 2026-08-30

### Novidades v15.0.0 (Codename "Cosmos" — 3D Character Studio & Next-Gen Graphics Toolset)

- **1. 3D Character Studio & Anatomical Mesh Sculptor CLI (`tools/character_studio.py`)**:
  - Procedural programmatic sculpting engine generating production-grade 3D character meshes (`gene_hd.obj`, `punk_hd.obj`) with contoured muscle loops, multi-part joint UV unwrapping, and smooth vertex normals.
  - Subcommand integrated in developer CLI: `python starlight.py bake-character [gene/punk/all]`.
- **2. PBR Texture Baker & Material Synthesizer CLI (`tools/texture_baker.py`)**:
  - 100% self-contained pure Python PBR texture generator baking tangent-space Normal Maps via 3x3 Sobel edge gradient operators, Cavity Ambient Occlusion Maps, and Roughness/Metallic channels.
  - Subcommand integrated in developer CLI: `python starlight.py bake-textures`.
- **3. Atmospheric Sky Dome & Solar Glare Baker CLI (`tools/sky_baker.py`)**:
  - Physical equirectangular atmospheric sky dome synthesizer simulating Rayleigh/Mie solar scattering, sun discs, and horizon color gradients (`DesertNoon`, `SunsetGold`, `CyberNight`).
  - Subcommand integrated in developer CLI: `python starlight.py bake-sky`.
- **4. Runtime 3D Model Loading Subsystem (`AssetLoader.cpp` / `ScriptSystem.cpp`)**:
  - Bound `engine.spawn_model(tag, modelPath, scale)` in Lua, allowing game scripts to dynamically spawn and render arbitrary Wavefront `.obj` meshes with automated fallback to contoured anatomical primitives.
- **5. Screen-Space Derivative Cotangent Frame & NaN Guarding (`pbr.frag`)**:
  - Re-engineered screen-space normal mapping with defensive singularity checks against zero UV derivatives and orthonormal tangent basis fallback, eliminating GPU black screens on Radeon/Intel hardware.
- **6. Contoured Organic Humanoid Geometry Generator (`AssetLoader::CreateHumanoidMesh`)**:
  - Replaced box primitives with contoured organic capsule and sphere loops featuring smoothed normals and mapped UV quadrants for realistic character anatomy.
- **7. Atmosphere & Dynamic Color Grading Presets (`core.lua`)**:
  - Added high-level atmospheric management module `Atmosphere` with calibrated presets (`DesertNoon`, `SunsetGold`, `CyberNight`) driving ACES filmic exposure, contrast, saturation, and atmospheric fog.
- **8. Version Milestone 15 ("Cosmos")**:
  - Upgraded engine versioning macros to `STARLIGHT_VERSION_MAJOR 15`, `STARLIGHT_VERSION_STRING "15.0.0"`, `STARLIGHT_VERSION_CODENAME "Cosmos"`, and `STARLIGHT_VERSION_NUMBER 150000`.

### Melhorias v15.0.0 (Improvements & Aesthetic Refinements)

- **Authentic God Hand Brawler Framing & Locomotion**:
  - Over-the-shoulder third-person chase camera with calibrated pitch clamp (`maxPitch = 32.0°`, `distance = 3.2m`) and North-facing cantina spawn orientation.
  - Signature multi-part character rig featuring blonde anime hair spikes, cheek bandage, slate-blue duster vest with orange chevron trim, white undershirt, dark leather chaps, and glowing God Hand armor.
  - Spring-damper secondary physics for flowing duster coat tails and ascot scarf.
- **Poisson Disk Shadow Filtering & Split-Sum IBL Ambient Floor**:
  - 16-tap Poisson disk Cascaded Shadow Map filtering and physical sky/ground hemispherical ambient floor preventing crushed blacks in unlit areas.

### Atualizações v15.0.0 (Ecosystem & Quality Assurance)

- **GoogleTest QA Suite**: Added `EngineVersionTest` for v15.0.0 Cosmos with 1,349/1,349 passing unit tests (100% success rate across 77 test suites).
- **Toolchain Upgrades**: Synchronized `starlight.py`, `lua_linter.py`, `asset_pipeline.py`, `qa_runner.py`, and project standard libraries (`core.lua`) across all 8 game projects.

## [12.0.0] - 2026-08-30

### Novidades v12.0.0 (Mega Suite of 100 Updates & Subsystems)

- **1. Core Math & SIMD (Updates 1–10)**: Implemented `MathUtils` with Ray-OBB Intersection, Ray-Plane Intersection, Quat Slerp/Nlerp, RGB <-> HSL <-> HSV bi-directional converters, Fast Inverse Square Root (`FastInvSqrt`), Bhaskara fast sin/cos approximations (`FastSin`/`FastCos`), and transform decomposition (`DecomposeTransform`).
- **2. Audio Synthesis & Acoustics (Updates 11–25)**: Added procedural White, Pink, and Brown noise generators, dynamic obstruction low-pass filtering, multi-voice polyphony management, audio ducking for dialogue/music, pan-pot stereo positioning, ADSR volume envelopes, frequency glissando pitch bends, randomized sound cues ($\pm 5\%$), and procedural synthesizers for explosions, laser blasters, power-up fanfares, and vehicle engine revs.
- **3. Renderer & Visual Post-Processing (Updates 26–40)**: Added chromatic aberration shader post-fx, screen-space vignette color tinting, radial blur and speed lines, FXAA anti-aliasing toggle, anamorphic bloom lens flares, pixel-snap ortho rendering, dynamic ambient color temperature grading, additive/multiplicative decal blending, and camera zoom smoothing.
- **4. Input & Accessibility (Updates 41–50)**: Added dual-stick deadzone calibration, cursor lock/confine modes, input binding persistence, combo buffer (30 frames FIFO), double-tap direction detection, smooth mouse wheel delta, vibration pulse patterns, and invert Y-axis toggle.
- **5. ECS, Scenes & Hierarchies (Updates 51–60)**: Added bitmask entity layer and tag system, fast name-to-entity ID hashing, hierarchy sibling reordering, transform look-at targeting, automated scene transition manager, and 2D spatial hash grid broadphase query.
- **6. Lua Standard Library & Runtimes (Updates 61–75)**: Expanded easing library (Elastic, Bounce, Back, Cubic, Quintic), Finite State Machine factory (`FSM.create`), EventBus Pub/Sub broker (`EventBus.on`/`emit`), Script lifecycle timers (`Timer.after`/`every`), Cubic Bezier curve evaluator (`bezier.eval`), and categorized debug loggers.
- **7. Developer CLI & Benchmark Suite (Updates 76–85)**: Added `python starlight.py bench` (automated multi-core benchmark), `python starlight.py clean-dist` (build staging cleaner), `python starlight.py audit` (static analysis and asset hygiene audit), dynamic FPS limiter CLI flags, and memory leak detection.
- **8. Game Enhancements (Updates 86–100)**: Curve-ball paddle spin and neon particle ribbons in Pong, speed-boost turbo mode and portal gates in Snake, ghost piece projection and T-Spin score bonuses in Tetris, candlestick charts and market crash events in Capital Odyssey, Super Art cancels in Fusion Fight, and 500-cube rigid body stress simulation in Tech Demo.

### Atualizações v12.0.0 (Ecosystem & Quality Assurance)

- **QA Suite Expansion**: Added unit tests in `test_CoreModule.cpp` for `JobSystemTest`, `DecalSystemTest`, `ProfilerTest`, `MathUtilsTest`, and `EngineVersionTest` (1,349 GoogleTests passing with 100% success rate across 77 test suites).
- **Synchronized Game Projects**: Recompiled and validated all 8 projects (`StarlightEngine`, `CapitalOdyssey`, `Pong_Project`, `Tetris_Project`, `Snake_Project`, `FusionFight`, `GodHand_Project`, `TechDemo_Project`).
- **Toolchain Upgrades**: Upgraded `starlight.py`, `qa_runner.py`, `lua_linter.py`, `asset_pipeline.py`, `game_exporter.py`, and distribution archives to v12.0.0 Apex standards.

## [11.0.0] - 2026-08-29

### Novidades v11.0.0 (New Features & Subsystems)

- **Input Action Mapping & Semantic Rebinding Subsystem**: Implemented input mapping abstraction in `InputSystem` (`BindAction`, `IsActionPressed`, `IsActionJustPressed`, `IsActionJustReleased`, `GetActionAxis`, `ClearActionBindings`, `GetActionKeys`) and exposed full Lua `input` suite (`input.bind_action`, `input.is_action_pressed`, `input.get_action_axis`, `input.vibrate`, `input.get_mouse_position`).
- **Spatial 3D Doppler Effect & Distance Attenuation Zones**: Added dynamic Doppler velocity pitch shifting and distance attenuation in `AudioSystem` (`SetListenerVelocity`, `SetDopplerFactor`, `SetDistanceAttenuation`, `SetReverbParameters`) exposed to Lua `audio`.
- **Automated Game Exporter & Store Distribution Pipeline (Steam / Itch.io / Standalone)**: Added `tools/game_exporter.py` and CLI `starlight.py export <game> [--zip] [--steam] [--appid <ID>] [--pak]` producing 100% self-contained standalone builds, automated SteamPipe manifests (`app_build.vdf`, `depot_build.vdf`, `upload_to_steam.bat`), `steam_appid.txt`, and release `.zip` archives.
- **Physics-Based Dynamic Material Audio Synthesis Subsystem**: Added kinetic-energy impact synthesis in `AudioSystem::PlayImpact` with 6 material types (`Metal`, `Wood`, `Concrete`, `Flesh`, `CyberShield`, `Glass`), generating material-resonant frequencies, low-pass damping, and transient envelopes dynamically.
- **Dynamic Camera Trauma & Non-Linear Screen Shake Subsystem**: Implemented $T^2$ non-linear trauma shake physics with rotational roll and auto-decay in `Renderer` (`AddCameraTrauma`, `GetCameraTrauma`, `SetCameraTrauma`), bound to `camera.shake`, `camera.add_trauma`, and `engine.add_camera_trauma`.
- **Advanced Color Grading & Tonemapping Post-Processing Controls**: Added real-time post-processing adjustments in `Renderer::SetColorGrading(exposure, contrast, saturation, gamma, vignetteStrength)` and `Renderer::SetBloomThreshold(threshold)` exposed to Lua in `engine` and `gfx`.
- **Parametric 3D Mesh Generation Subsystem**: Implemented parametric 3D primitive geometry in `AssetLoader` with `CreateTorusMesh(majorRadius, minorRadius, radSegs, tubeSegs)` and `CreateIcosphereMesh(radius, subdivisions)` with seamless UV texturing and normal generation.
- **Parametric Lua Mesh Bindings**: Exposed `assets.create_torus(majorR, minorR, radSegs, tubeSegs)` and `assets.create_icosphere(radius, subdivisions)` for real-time procedural entity creation in game scripts.
- **Framebuffer Screenshot Capture API**: Added high-resolution raw framebuffer backbuffer capture in `Window::CaptureScreenshot(path)` and Lua binding `engine.capture_screenshot(path)` / `window.capture_screenshot(path)`.
- **Version Milestone 11**: Updated versioning macros to `STARLIGHT_VERSION_MAJOR 11`, `STARLIGHT_VERSION_STRING "11.0.0"`, `STARLIGHT_VERSION_CODENAME "Titan"`, and `STARLIGHT_VERSION_NUMBER 110000`.

### Melhorias v11.0.0 (Improvements & Sound Suites)

- **Procedural Combat Audio Suite in `FusionFight`**: Added `Audio.playLightHit()`, `Audio.playHeavyHit()`, `Audio.playBlock()`, `Audio.playParry()`, `Audio.playSpecial()` with dynamic material impacts and camera trauma.
- **Procedural Brawler Audio Suite in `GodHand_Project`**: Integrated kinetic impact audio in `SFX.playPunchLight()`, `SFX.playPunchMedium()`, `SFX.playPunchHeavy()`, `SFX.playGuardBreak()`, and `SFX.playPropBreak()` with camera trauma roll.
- **Procedural Audio Suite for Capital Odyssey (`CapitalOdyssey`)**: Added profit-pitch modulated buy chimes (`audio.fm_note`), sell cash register feedback, and F12 snapshot trigger.
- **Universal Live Telemetry Overlay (`F3`) in Capital Odyssey**: Integrated live F3 stats HUD overlay showing frame timing, active entities, audio voices, memory usage, draw calls, and batch quads.
- **Mesh Memory & Vertex Inspection Ergonomics**: Added `Mesh::GetIndexCount()` for real-time index count validation.

### Atualizações v11.0.0 (Ecosystem & Quality Assurance)

- **Test Suite Expansion**: Added unit tests in `test_CoreModule.cpp` for `ProceduralGeometryTest`, `CameraTraumaTest`, `ColorGradingTest`, `MaterialAudioImpactTest`, `InputActionMappingTest`, and `AudioSpatialParametersTest` (1,345 GoogleTests passing with 100% success rate across 73 test suites).
- **Synchronized Game Projects**: Rebuilt and validated all 8 projects (`StarlightEngine`, `CapitalOdyssey`, `Pong_Project`, `Tetris_Project`, `Snake_Project`, `FusionFight`, `GodHand_Project`, `TechDemo_Project`).
- **Toolchain Upgrades**: Upgraded `starlight.py`, `build_all.ps1`, QA health runners, and linters to v11.0 standards.

## [10.0.0] - 2026-08-29

### Novidades v10.0.0 (New Features & Subsystems)

- **Engine System Reflection & Introspection API**: Added `engine.get_systems()` (listing all active engine subsystems, enabled status, and execution priorities) and `engine.is_system_active(name)` for modular script-level inspection.
- **Global Memory & Allocator Telemetry API**: Added `MemoryManager::GetTotalUsedMemory()`, `MemoryManager::GetTotalCapacity()`, and `MemoryManager::GetAllocatorCount()` for real-time tracking of thread-local frame allocators.
- **Dynamic Target FPS & Frame Pacing**: Added `engine.set_target_fps(fps)` and `engine.get_target_fps()` Lua bindings connected to core `Engine::SetMaxFPS()`.
- **Expanded Script Telemetry API**: `engine.get_telemetry()` now exposes `fps`, `avg_frame_time`, `entities_count`, `active_audio_voices`, `memory_used_kb`, `memory_capacity_kb`, `draw_calls`, `quad_count`, and `target_fps`.

### Melhorias v10.0.0 (Improvements & Sound Suites)

- **Procedural Sound Suite for Snake 2D (`Snake_Project`)**: Dynamic combo-modulated food eating chimes, multi-operator FM fanfares for golden and toxic pickups, explosion noise on death, and menu UI navigation beeps.
- **Procedural Sound Suite for Tetris 2D (`Tetris_Project`)**: Added rotation ticks, hard drop impacts, single/double/triple line clear chimes, and a dedicated 4-note FM chord fanfare for **TETRIS (4-line)** clears.
- **Universal Live Telemetry Overlay (`F3`)**: Implemented standardized `F3` debug overlay across `Pong_Project`, `Snake_Project`, and `Tetris_Project` displaying live FPS, frame timings, entity count, audio voice count, memory usage, draw calls, and quad counts.

### Atualizações v10.0.0 (Ecosystem & Quality Assurance)

- **Test Suite Expansion**: Added unit tests in `StarlightEngine/tests/test_CoreModule.cpp` for `MemoryManagerTelemetryTest` and `Renderer2DStatsTest` (1,340 passing tests with 100% success rate).
- **Synchronized Game Projects**: Rebuilt and validated all 8 projects (`StarlightEngine`, `CapitalOdyssey`, `Pong_Project`, `Tetris_Project`, `Snake_Project`, `FusionFight`, `GodHand_Project`, `TechDemo_Project`).
- **Toolchain Upgrades**: Upgraded `starlight.py`, build scripts, QA health runners, and linters to v10.0 standards.

## [9.0.0] - 2026-08-29

### Added v9.0.0

- **Starlight Engine v9.0 Evolution (Codename "Supernova")**: Advanced runtime telemetry and diagnostics suite.
- **Engine Diagnostics & Telemetry API**: Added `engine.get_telemetry()` exposing live FPS, average frame time, total time, active ECS entity count, and active audio voice count to Lua.
- **Extended Version API**: Added `STARLIGHT_VERSION_CODENAME ("Supernova")`, `Engine::GetVersionCodename()`, and Lua `engine.get_version_info()` returning detailed version dictionaries.
- **Voice Count Tracking in AudioSystem**: Implemented `AudioSystem::GetActiveVoiceCount()` providing real-time voice utilization telemetry.
- **Toolchain Upgrades**: Updated `starlight.py` CLI suite, QA runner, Lua linter, asset validator, and build system banners to v9.0.

## [8.0.0] - 2026-08-29

### Added v8.0.0

- **Starlight Engine v8.0 Evolution**: Major evolution and rebranding of the engine ecosystem.
- **Native Versioning API**: Introduced `Version.hpp` (`STARLIGHT_VERSION_MAJOR 8`, `STARLIGHT_VERSION_MINOR 0`, `STARLIGHT_VERSION_PATCH 0`) with C++ static queries (`Engine::GetVersionString()`) and Lua bindings (`engine.version`).
- **Unified Python CLI & Developer Toolchain (`starlight.py`)**: Subcommands for `build`, `run`, `test`, `lint`, `validate`, `stats`, and `new-game` project scaffolding.
- **Procedural Sound Suite 2.0 & Polyphony Voice Stealing**: 22 procedural sound events and 16-channel voice stealing in `AudioSystem.cpp`.
- **Authentic Classic 1972 Pong**: Segmented CRT vector score display, 7-zone deflection physics, rally acceleration, and phosphor/amber/B&W filters.
- **Automated QA & Static Lua Linting**: `tools/qa_runner.py`, `tools/lua_linter.py`, and `tools/asset_pipeline.py`.

## [Unreleased]

### Added in Unreleased

- **Commercial 7-Module Architecture**: Comprehensive implementation of custom memory allocators (Arena, Stack, Pool, Double-Buffer), SIMD math, OOP ECS Entity handles, Scene Graph hierarchy, 2D Batching, 3D Cook-Torrance PBR, UUID Asset Pipeline with hot-reloading, Jolt Physics simulation world, and Dear ImGui WYSIWYG Editor panels & profiler.
- **TechDemo_Project**: Dedicated architectural showcase project demonstrating all 7 modules with 5 interactive showrooms, kinetic physics projectile cannon, orbiting neon lighting, and real-time telemetry HUD.
- **GodHand_Project**: Complete PS2 3D retro brawler game based on God Hand (Clover Studio/Capcom), featuring 3D over-the-shoulder camera, 5-hit combat chains, directional dodges (duck/weave, sway, backflip), God Reel roulette special attacks, and dynamic Level 1 -> Level DIE difficulty scaling.
- Procedural 3D Mesh primitives API in `AssetLoader` (`CreatePlaneMesh`, `CreateBoxMesh`, `CreateCylinderMesh`, `CreateSphereMesh`, `CreateCapsuleMesh`, `CreateWedgeMesh`).
- Extended Lua Scripting bindings for 3D primitive spawning, material PBR properties, camera control, and immediate matrix synchronization.
- PBR ambient baseline lighting and synchronized vertex-to-fragment shader varying pipeline.

### Changed in Unreleased

- Repository audit and cleanup: hardened `.gitignore`, added `.gitattributes`,
  archived legacy reference material under `StarlightEngine/docs/archive/`.

### Planned in Unreleased

- Build system unification: parameterize `DEPS_DIR`, extract shared CMake
  helper, deduplicate the four game `CMakeLists.txt`.
- Fix runtime shader lookup bug.
- Track and enable `StarlightEngine/tests/` in CI.
- C++ source audit (dead code, unused includes, system registration).

## [1.0.6] - 2026

### Features Added in 1.0.6

- SBA v2.0 Lua framework (`core.lua`, `sba_bridge.lua`).
- Showcase example, four game projects (Pong, Snake, Tetris, CapitalOdyssey).
- Documentation overhaul with full bilingual support (PT-BR / EN).

## [1.0.5] and earlier

See git history: `git log --oneline --all`.
