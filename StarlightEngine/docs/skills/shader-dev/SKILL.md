---
name: shader-dev
description: Guidelines for writing WGSL shaders for WGPU/Starlight. Covers struct alignment, bind groups, and common pitfalls.
---

# WGSL Shader Development

## 1. File Location
Shaders live in `assets/shaders/`. The main shader is `shader.wgsl`.

## 2. Bind Group Layout (Current Architecture)

| Group | Binding | Type | Content |
|:---|:---|:---|:---|
| **0** | 0 | Uniform | `Uniforms` (view_proj, model, camera_pos, time) |
| **0** | 1 | Uniform | `LightUniform` (sun, fog, point lights) |
| **0** | 2 | Texture | Shadow depth map |
| **0** | 3 | Sampler | Shadow comparison sampler |
| **1** | 0 | Texture | Diffuse texture |
| **1** | 1 | Sampler | Diffuse sampler |
| **1** | 2 | Texture | Normal map |
| **1** | 3 | Sampler | Normal sampler |
| **1** | 4 | Uniform | `MaterialUniform` (metallic, roughness) |
| **2** | 0 | Texture | Skybox cubemap |
| **2** | 1 | Sampler | Skybox sampler |

## 3. Struct Definitions (Must Match Rust Exactly)

### Uniforms (Group 0, Binding 0)
```wgsl
struct Uniforms {
    view_proj: mat4x4<f32>,
    model: mat4x4<f32>,
    light_view_proj: mat4x4<f32>,
    camera_pos: vec4<f32>,
    time: f32,
    wind_strength: f32,
    _padding: vec2<f32>,
};
```

### LightUniform (Group 0, Binding 1)
```wgsl
struct LightUniform {
    direction: vec3<f32>,
    fog_density: f32,
    color: vec3<f32>,
    ambient: f32,
    sun_intensity: f32,
    _pad0: f32,
    _pad1: f32,
    _pad2: f32,
    point_lights: array<PointLight, 4>,
    point_light_count: u32,
    fog_r: f32,
    fog_g: f32,
    fog_b: f32,
    _padding_end: vec4<f32>,
};
```

### PointLight
```wgsl
struct PointLight {
    position: vec4<f32>,
    color: vec4<f32>,
};
```

## 4. Memory Alignment Rules (CRITICAL)

WGPU uses std140/std430-like alignment. The rules that cause the most bugs:

| WGSL Type | Size | Alignment | Trap |
|:---|:---|:---|:---|
| `f32` | 4 bytes | 4 bytes | Safe |
| `vec2<f32>` | 8 bytes | 8 bytes | Safe |
| `vec3<f32>` | 12 bytes | **16 bytes** | ⚠️ Takes 16 bytes in uniform buffers! |
| `vec4<f32>` | 16 bytes | 16 bytes | Safe |
| `mat4x4<f32>` | 64 bytes | 16 bytes | Safe |
| `array<T, N>` | N × stride(T) | align(T) | Stride is rounded to align |

> **GOLDEN RULE**: When adding fields before an `array` or `vec4`, the offset of the next field must be a multiple of 16. Use padding `f32` fields to fill gaps.

### How to Calculate Buffer Size
```
Size of LightUniform:
  direction (vec3) + fog_density (f32)  = 16 bytes
  color (vec3) + ambient (f32)          = 16 bytes
  sun_intensity + 3×padding             = 16 bytes
  point_lights (4 × 32 bytes)           = 128 bytes
  point_light_count + fog_rgb           = 16 bytes
  _padding_end (vec4)                   = 16 bytes
  TOTAL                                 = 208 bytes
```

## 5. Render Pass Order
1. **Shadow Pass** — Depth-only render from sun's perspective
2. **Main Pass** — Full PBR rendering with shadow sampling
3. **Skybox Pass** — Background cubemap
4. **Blit Pass** — Copy to screen surface

## 6. Alpha Testing
For transparent textures (leaves, ferns), use `discard`:
```wgsl
@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    let tex_color = textureSample(t_diffuse, s_diffuse, in.tex_coords);
    if (tex_color.a < 0.5) {
        discard;
    }
    // ... PBR calculations
}
```

## 7. Hot Reloading
Currently, shaders are loaded at startup. Restart the demo to see WGSL changes. Rust struct changes require a full `cargo build --release` (see `rust-build` skill).


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
