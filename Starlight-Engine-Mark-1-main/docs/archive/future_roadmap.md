# 🚀 Starlight Engine - Future Roadmap (Cython & UX)

This document outlines the strategic plan to elevate Starlight Engine to a professional level using **Cython** for performance and a **Visual Editor** for accessibility.

## 🎯 Core Philosophy
1.  **Performance First**: Heavy logic (Physics, Culling, Batching) moves to **Cython**.
2.  **Accessibility**: Users should interact via a **GUI Editor**, not code.
3.  **Visual Loyalty**: PBR standards and high-end post-processing.

---

## 📅 Phase 1: The Cython Core (Performance)
*Objective: rewrite bottlenecks in compiled C via Cython.*

### 1.1. Cython Setup
- [ ] Create `setup.py` build system.
- [ ] create `engine/core.pyx` for high-performance math.
- [ ] **Goal**: 10x faster Object Transform updates and Frustum Culling.

### 1.2. Optimized Render Loop
- [ ] Move the `render()` loop logic to Cython.
- [ ] Implement **AZDO** (Approaching Zero Driver Overhead) principles where possible using ModernGL advanced features managed by Cython.

### 1.3. Physics Integration
- [ ] Integrate a lightweight C++ Physics engine (like `Chipmunk` or `Bullet`) via Cython bindings, OR optimize a simple AABB collision system in pure Cython.

---

## 🎨 Phase 2: Graphics Evolution (Visuals)
*Objective: AAA-lite visuals.*

### 2.1. True PBR with IBL
- [ ] **Image Based Lighting (IBL)**: Use HDR Cubemaps for realistic ambient lighting and reflections.
- [ ] **PBR Shader Upgrade**: Implement Roughness/Metallic workflow fully (Cook-Torrance BRDF).

### 2.2. Advanced Shadows
- [ ] **Cascaded Shadow Maps (CSM)**: Sharp shadows near player, soft shadows far away.
- [ ] **Soft Shadows**: PCF (Percentage Closer Filtering) or Poisson Sampling.

### 2.3. Post-Processing Stack
- [ ] **Bloom**: Physically based bloom (Karis average).
- [ ] **Tone Mapping**: ACES Filmic Tone Mapping.
- [ ] **SSAO**: Screen Space Ambient Occlusion for depth perception.

---

## 🖱️ Phase 3: Accessibility & Editor (UX)
*Objective: "No-Code" Scene creation.*

### 3.1. Visual Editor (Starlight Studio)
- [ ] **Gizmos**: Move/Rotate/Scale objects using visual handles in the viewport.
- [ ] **Asset Browser**: Drag and drop models/textures from a content folder.
- [ ] **Inspector**: Modify object properties (Position, Color, Scripts) in a side panel.

### 3.2. Scripting System
- [ ] Decouple engine code from game logic.
- [ ] Introduce a `Component` system where users attach Python scripts to objects.
  - Example: `object.add_component(Rotator(speed=5.0))`

### 3.3. Distribution
- [ ] **One-Click Launcher**: A simple `.exe` or Python script that opens the Project Selector.
- [ ] **Templates**: "FPS Template", "City Template", "RPG Template".

---

## 📝 Roadmap Summary

| Feature | Technology | Benefit |
| :--- | :--- | :--- |
| **Transform Math** | **Cython** | Extremely fast object movement. |
| **Physics** | **Cython** | Stable collisions without lag. |
| **UI / Editor** | **ImGui Bundle** | Easy tools for non-programmers. |
| **Rendering** | **ModernGL** | High-performance visuals. |

## 🏁 Next Steps
1.  Initialize `starlight_cython` module.
2.  Port `World.render` loop to Cython.
3.  Flesh out the `EditorApp` in `editor.py` to support Scene Graph.
