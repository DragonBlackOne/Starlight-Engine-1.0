# 🔱 starlight ENGINE: THE OMEGA COMPENDIUM (2026)

Welcome to the **Ultimate Unified Engine**. This manual serves as the definitive guide to the industrial-grade, high-performance, and visual-elite engine born from the fusion of *Starlight* and *Quimera*.

---

## 🏛️ CORE ARCHITECTURE
- **Engine Core**: Modular architecture with `EngineModule` support.
- **ECS (Entity Component System)**: High-performance `EnTT` integration.
- **Job System**: Multi-threaded task execution via `wiJobSystem`.
- **Octree**: Spatial partitioning for frustum culling and physics optimization.

---

## 🎨 ADVANCED RENDERING PIPELINE
The starlight Renderer is a state-of-the-art PBR pipeline with:
- **PBR (Physically Based Rendering)**: Metallic/Roughness workflow with IBL.
- **Cascaded Shadow Maps (CSM)**: 4 cascades for high-fidelity outdoor shadows.
- **SSAO (Ambient Occlusion)**: Temporal Screen-Space Ambient Occlusion.
- **HDR Bloom**: High-extraction extraction and 10-pass Gaussian blur.
- **SSR (Screen Space Reflections)**: Raymarched real-time reflections.
- **ACES Filmic Tone Mapping**: Cinema-quality color grading.
- **Volumetric Fog**: Distance-based depth fog.
- **Instanced Rendering**: Hardware-accelerated batching for 100,000+ objects.

---

## 🛠️ STUDIO & WORKFLOW
- **starlight Studio (F2)**: Interactive editor with Hierarchy, Inspector, and Asset Browser.
- **3D Picking**: Ray-cast selection directly in the viewport.
- **Hot Reload**: Automatic resource refreshing via `FileWatcher`.
- **Developer Console**: In-game command terminal with `CVar` support.

---

## 🤖 SPECIALIZED SYSTEMS
- **Level of Detail (LOD)**: Automated distance-based mesh optimization.
- **Ozz-Animation**: Skeletal animation support with professional blending and skinning.
- **Global Event Bus**: Decoupled messaging for modular communication.
- **Physics (Jolt)**: Industrial-grade rigid body dynamics (v5.5.0 Stable).
- **Navigation**: Dynamic grid-based pathfinding for AI.

---

## 📂 ASSET STRUCTURE
All engine resources are located in `assets/`:
- `/audio`: Themes and effects.
- `/fonts`: Typographic assets.
- `/images`: Textures and sprites.
- `/models`: 3D meshes (OBJ/GLB).
- `/shaders`: OpenGL source files.
- `/scripts`: Lua/Logic scripts.

---

> [!IMPORTANT]
> **PROCEED WITH CAUTION**: The power of the Omega Fusion is vast. Use the Developer Console (`help`) to explore CVars and tweak performance in real-time.

*Starlight Engine v2.1.0 - 2026 Odyssey Edition*
