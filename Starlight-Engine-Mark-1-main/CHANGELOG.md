# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [7.0.0] - 2026-03-29 ("Mark-C Purist Unification") 🛡️✨

The engine has achieved its definitive form: **Pure C11**. We completely expelled the hybrid Rust/Python experimental core to enforce a lightweight, lightning-fast native environment.

### 🚀 Major Features (Master Audit Triumphs)
- **C-Native Transformation**: Replaced heavy Rust WGPU wrappers with blazing fast OpenGL 4.1 C algorithms.
- **Data Integrity Core**: Critical fixes to game-saves via Atomic Transaction logic (`fclose` `rename` `tmp`), ending 0-byte corruptions forever.
- **Linear HDR Render Pipeline**: Purged legacy double-Gamma issues in shaders. The engine now operates mathematically correct PBR math outputting raw Linear values to the ACES Tonemapping Post-Processing.
- **UI Render Batching**: Upgraded generic text rendering from `O(N)` draw calls to massive `O(1)` instanced batching in `slf.c`.
- **Job System Thread Yielding**: Implemented `spin_count` backoff mechanisms to prevent 100% CPU locking on Ryzen idle threads.
- **Dynamic Audio Ducking**: Reinitialized the Miniaudio Backend natively into the Starlight Deluxe Hub with zero blocking threads (Goodbye `SDL_Delay` debounce freezes).
- **Asset Purge**: Wiped all redundant 3D `.obj` files to save megabytes of dead-weight clone time.

### 🧹 Cleanup
- **Archived**: Dropped Rust `crates` completely. The future is `src/*.c`.
- **Refactor**: All C++ syntaxes (like lambdas) in Flecs wrappers were strictly rewritten into Pure C99/C11 standards.

## [6.0.0] - 2026-01-15 ("Rust Grand Unification") 🦀
*Note: This architecture was deprecated and archived in favor of Mark-C.*
The engine was temporarily rewritten from the ground up, moving the core loop, ECS, and Renderer entirely to Rust (`starlight_rust`). Python was a high-level scripting layer.

## [5.0.0] - 2025-12-17 ("Kinetic Update")

### Major Changes
- **Scene Graph**: Introduced a hierarchical node system (`Node`) for 3D objects.
- **glTF 2.0 Native Support**: Robust glTF/GLB loader with hierarchy support.

## [Legacy Versions]
*Refer to git history for versions 4.1.0 and below.*