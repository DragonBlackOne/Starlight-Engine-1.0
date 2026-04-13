---
name: wgpu-renderer
description: WebGPU Renderer architecture guide. Covers coordinate system, bind groups, render passes, and debugging.
---

# WGPU Renderer Architecture

## 1. Coordinate System
- **Clip Space**: Z range is `[0, 1]` (not [-1, 1] like OpenGL). Y is up.
- **Textures**: Top-left origin (Y flipped from OpenGL).
- **Projection Correction**: The renderer applies `OPENGL_TO_WGPU_MATRIX` to flip Y and adjust Z. This happens in `state.rs`.

## 2. Bind Group Layout

| Group | Purpose | Bindings |
|:---|:---|:---|
| **Group 0** | Scene Globals | Uniforms, LightUniform, Shadow Map, Shadow Sampler |
| **Group 1** | Material | Diffuse Tex, Diffuse Sampler, Normal Tex, Normal Sampler, MaterialUniform |
| **Group 2** | Skybox | Cubemap Texture, Cubemap Sampler |

**Instance data** (model matrix, color) is passed via **vertex buffers** (instanced), NOT bind groups.

## 3. Render Pass Pipeline

```
┌─────────────────┐
│  Shadow Pass    │ → Depth-only pass from sun's perspective
│  shadow.wgsl    │   Writes to 2048×2048 depth texture
└────────┬────────┘
         ↓
┌─────────────────┐
│   Main Pass     │ → Full PBR lighting + shadow sampling
│  shader.wgsl    │   Backface culling ON, depth test ON
└────────┬────────┘
         ↓
┌─────────────────┐
│  Skybox Pass    │ → Cubemap background (depth ≤ test)
│  shader.wgsl    │   Uses same shader, skybox entry point
└────────┬────────┘
         ↓
┌─────────────────┐
│   Blit Pass     │ → Copies render target → window surface
│  blit.wgsl      │   Handles MSAA resolve if enabled
└─────────────────┘
```

## 4. Key Source Files

| File | Purpose |
|:---|:---|
| `crates/engine_render/src/renderer/state.rs` | `RenderState` — owns GPU resources, manages passes |
| `crates/engine_render/src/renderer/pipeline.rs` | Pipeline creation (vertex layout, shaders) |
| `crates/engine_render/src/resources/material.rs` | `LightUniform`, `Uniforms`, `MaterialUniform` structs |
| `assets/shaders/shader.wgsl` | Main vertex/fragment shaders |
| `assets/shaders/shadow.wgsl` | Shadow depth pass shader |

## 5. Instance Buffer Layout
Each entity is rendered via instanced draw calls. The instance buffer contains:

```rust
struct InstanceRaw {
    model: [[f32; 4]; 4],  // 64 bytes — Model matrix
    color: [f32; 4],       // 16 bytes — Entity color (RGBA)
}
// Total: 80 bytes per instance
```

## 6. Debugging WGPU Errors

### "Buffer is bound with size X where shader expects Y"
This is a **struct alignment mismatch** between Rust and WGSL. Fix:
1. Count bytes in both Rust and WGSL structs
2. Remember: `vec3<f32>` in WGSL takes **16 bytes** (not 12)
3. Add `f32` padding fields until sizes match
4. Rebuild Rust AND restart the demo

### "compact index N"
An entity references an invalid resource (texture, mesh). Usually means an asset wasn't loaded before the first frame.

### "Validation Error in RenderPass::end"
A draw command is using a pipeline or bind group that doesn't match the current pass layout. Check that shadow pass uses shadow pipeline.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
