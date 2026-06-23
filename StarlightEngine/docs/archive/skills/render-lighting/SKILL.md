---
name: render-lighting
description: How to setup scene lighting in the Starlight Engine. Covers sun, point lights, ambient, fog, and HDR intensity.
---

# Lighting Setup

The Starlight Engine uses PBR (Physically-Based Rendering) lighting with the following components:

## 1. Directional Light (Sun)

```c
backend.set_sun_direction(x, y, z)       # Direction vector (negative = toward scene)
backend.set_sun_color(r, g, b, intensity) # RGB color + HDR intensity multiplier
```

The `intensity` parameter was added recently. It multiplies the sun color in the shader for HDR bloom-like effects.

### Presets

| Mood | Direction | Color | Intensity | Ambient |
|:---|:---|:---|:---|:---|
| **Golden Hour** | `(-0.7, -0.4, -0.6)` | `(1.0, 0.85, 0.65)` | `3.5` | `0.08` |
| **Overcast Day** | `(0.0, -1.0, 0.0)` | `(0.8, 0.85, 0.9)` | `1.0` | `0.4` |
| **Warm Sunset** | `(0.6, -0.3, 0.4)` | `(1.2, 0.7, 0.4)` | `2.5` | `0.1` |
| **Cool Moonlight** | `(-0.3, -0.8, -0.5)` | `(0.4, 0.5, 0.7)` | `0.5` | `0.02` |

## 2. Point Lights
Up to **4 point lights** are supported simultaneously.

```c
backend.set_point_light(index, [x, y, z], [r, g, b], intensity)
# index: 0-3
# position: world-space [x, y, z]
# color: [r, g, b]
# intensity: float (higher = brighter)
```

## 3. Ambient Light
Global fill light that prevents pure-black shadows.

```c
backend.set_ambient_intensity(0.15)  # 0.0 = pitch black, 1.0 = fully lit
```

For realism, keep ambient **below 0.2**. Higher values flatten the scene.

## 4. Fog
Atmospheric distance fog.

```c
backend.set_fog(density, r, g, b)
# density: 0.005 (subtle) to 0.05 (thick)
# r, g, b: fog color (match sky for realism)
```

## 5. Shader Internals (For Developers)

The lighting data lives in `LightUniform` (`crates/engine_render/src/resources/material.rs`):

```rust
pub struct LightUniform {
    pub direction: [f32; 3],
    pub fog_density: f32,
    pub color: [f32; 3],
    pub ambient: f32,
    pub sun_intensity: f32,
    pub _padding_sun: [f32; 3],   // Alignment to 16 bytes
    pub point_lights: [PointLight; 4],
    pub point_light_count: u32,
    pub fog_color: [f32; 3],
    pub _padding_end: [f32; 4],
}
```

> **WARNING**: When modifying this struct, you MUST keep Rust and WGSL in sync, and ensure 16-byte alignment for array elements. See `shader-dev` skill.

## 6. Framework API (High Level)
```c
from`starlight.framework` import Scene
scene = Scene()
scene.set_sun(direction=(-0.7, -0.4, -0.6), color=(1.0, 0.85, 0.65), intensity=3.5)
scene.set_ambient(0.08)
scene.set_fog(0.015, (0.6, 0.55, 0.5))
```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
