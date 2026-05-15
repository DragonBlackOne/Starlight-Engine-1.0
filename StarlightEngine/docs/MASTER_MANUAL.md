# 🔱 Starlight ENGINE: THE OMEGA COMPENDIUM (2026)

Welcome to the **Ultimate Unified Engine**. This manual serves as the definitive guide to the industrial-grade Fusion Engine and its **SBA v2.0** game development framework.

---

## 🏛️ CORE ARCHITECTURE
- **Engine Core**: Modular architecture with `EngineModule` hot-toggling.
- **ECS (Entity Component System)**: High-performance `EnTT` integration with cache-friendly iteration.
- **Job System**: Multi-threaded fiber-based execution via `wiJobSystem`.
- **Octree**: Spatial partitioning for frustum culling and physics optimization.

---

## 🎨 ADVANCED RENDERING PIPELINE
The Starlight Renderer is a state-of-the-art PBR pipeline:

| Feature | Details |
|---------|---------|
| **PBR** | Metallic/Roughness workflow with IBL (Image-Based Lighting) |
| **CSM** | 4-cascade shadow maps at 2048x2048 |
| **SSAO** | Temporal screen-space ambient occlusion |
| **HDR Bloom** | Multi-pass Gaussian with 10-pass blur |
| **SSR** | Raymarched real-time reflections |
| **ACES** | Cinema-quality filmic tone mapping |
| **Volumetric Fog** | Distance-based depth fog |
| **Instanced Rendering** | Hardware-accelerated batching (100K+ objects) |
| **Renderer2D** | Batched quads/sprites for UI and 2D games |
| **GPU Culling** | Compute shader-based frustum + occlusion culling |

---

## 🎮 SBA v2.0 — STARLIGHT BRIDGE API

The high-level Lua SDK for rapid game development:

### Game SDK (`sba_bridge.lua`)
| System | API |
|--------|-----|
| **Entity** | `Entity("Tag", x, y, z)` → `:setColor()`, `:setScale()`, `:setMaterial()`, `:move()`, `:destroy()` |
| **Light** | `Light(x,y,z, r,g,b, intensity)` → `:setColor()`, `:setIntensity()` |
| **Tween** | `Tween.to(entity, {y=5}, 1.5, "easeOutElastic")` — 8 easing functions |
| **Scene** | `Scene.register("Game", {onEnter, onUpdate, onRenderUI, onExit})` |
| **Events** | `Events.on("hit", fn)` / `Events.emit("hit", data)` |
| **Coroutine** | `Coroutine.start(fn)` / `Coroutine.wait(seconds)` |

### Standard Library (`core.lua`)
| System | API |
|--------|-----|
| **MathX** | `clamp`, `lerp`, `sign`, `distance`, `distance3D`, `smoothstep`, `remap`, `random_range`, `normalize2D`, `angle`, `wrap` |
| **Physics2D** | `CheckAABB`, `CheckCircle`, `PointInRect`, `RayCircle` |
| **Timer** | `Timer.after(sec, fn)`, `Timer.every(sec, fn)`, `Timer.cancel(id)` |
| **Color** | `Color.hsv(h,s,v)`, `Color.pulse(r,g,b,t)`, `Color.lerpRGB(...)` |
| **ScreenShake** | `trigger(intensity, duration)`, `update(dt)`, `getOffset()` |
| **ValueTween** | `ValueTween.to(table, key, endVal, duration, easing)` |
| **Class** | OO factory with `is_a()` inheritance checking |

---

## 🛠️ STUDIO & WORKFLOW
- **Starlight Studio (F2)**: Interactive editor with Hierarchy, Inspector, and Asset Browser.
- **3D Picking**: Ray-cast selection via `Engine.get_mouse_hit()`.
- **Hot Reload**: Automatic resource refreshing via `FileWatcher`.
- **Developer Console**: In-game command terminal with `CVar` support.

---

## 🤖 SPECIALIZED SYSTEMS
- **Level of Detail (LOD)**: Distance-based mesh optimization.
- **Ozz-Animation**: Skeletal animation with blending and skinning.
- **Global Event Bus**: Decoupled messaging (Lua `Events` + C++ `EventBus`).
- **Physics (Jolt 5.5.0)**: Industrial-grade rigid body dynamics.
- **Navigation**: Grid-based pathfinding for AI agents.

---

## 📂 ASSET STRUCTURE
```
assets/
├── audio/       # Sound effects and music
├── fonts/       # Typography assets
├── textures/    # Images and sprites
├── models/      # 3D meshes (OBJ/GLB)
├── shaders/     # OpenGL GLSL source files
└── scripts/     # Lua scripts
    ├── core.lua           # Standard Library
    ├── sba_bridge.lua     # Game SDK
    └── game_main.lua      # Your game logic
```

---

*Starlight Engine v2.1.0 — SBA v2.0 Powered — 2026 Odyssey Edition*
