# Starlight Engine Architecture (Mark-C)

Starlight Mark-C is a **Native C11 Engine** designed for deterministic performance, high-fidelity rendering, and authentic retro emulation.

## 🏛️ System Layers

### 1. **The Core Loop (60Hz Lock)**
Located in `slf.c`, the engine uses a **Fixed Timestep Accumulator**. 
- Logic runs at exactly 60Hz for deterministic physics and gameplay.
- Rendering runs at the maximum monitor refresh rate (Unlocked/VSync).
- Prevents "Spiral of Death" via delta-time clamping.

### 2. **Framework Layer (SLF)**
- **Scene Stack**: Replaces global state machines with a `Push/Pop` stack. Each scene (`SLF_Scene`) has its own `on_enter`, `on_update`, `on_draw`, and `on_exit` hooks.
- **Asset Pool**: Implements **Reference Counting** for VRAM hygiene. Loading the same texture twice returns the cached pointer.
- **Action Mapping**: High-level input abstraction. One action (e.g., "Jump") can be bound to multiple keys, mouse buttons, or controller buttons.

### 3. **Rendering Pipelines**

#### 💎 **Modern HDR Path**
- **HDR Forward+**: 16-bit texture format for high dynamic range.
- **PBR (Physically Based Rendering)**: Based on the Cook-Torrance BRDF model with IBL (Image Based Lighting).
- **Post-Process Chain**: Bloom (Karis Average), ACES Tonemapping, and FXAA.

#### 🕹️ **Retro Genesis Path**
- **Mode 7 Engine**: Real-time inverse perspective mapping for rotating/scaling planes.
- **Parallax Engine**: Multi-layer background scrolling with depth simulation.
- **Low-Res Downsampling**: Renders to a tiny FBO (e.g., 320x180) then upscales with `GL_NEAREST` for pixel-art purity.

---

## 💾 Memory & Resource Model
- **Explicit Ownership**: The developer is responsible for calling `_destroy` functions.
- **Asset Lifespans**: The `SLF_AssetPool` tracks how many systems are using a resource; it is only freed when the count hits zero.
- **Zero Trash**: No garbage collection. Memory usage is flat and predictable.

---
**🏆 Starlight: Total visibility. Total control.**

