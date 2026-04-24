---
name: render-materials
description: Guidelines for creating PBR materials.
---

# PBR Materials

## Material System
Each entity can have a `Material` with two texture slots:

```c
# Load textures
diffuse_id = backend.load_texture("assets/textures/wood.png")
normal_id = backend.load_texture("assets/textures/wood_normal.png", is_normal=True)

# Apply to entity
backend.set_material_textures(entity_id, diffuse_id, normal_id)
```

## PBR Parameters
The `MaterialUniform` controls metallic and roughness:

| Parameter | Range | Description |
|:---|:---|:---|
| `metallic` | 0.0 – 1.0 | 0 = dielectric (wood), 1 = metal |
| `roughness` | 0.0 – 1.0 | 0 = mirror, 1 = matte |

Currently set via Rust defaults. Future API: `backend.set_material_params(entity_id, metallic, roughness)`.

## Built-in Textures
| ID | Content |
|:---|:---|
| `"default"` | White 1×1 texture |
| `"flat_normal"` | Flat normal map (0.5, 0.5, 1.0) |

## Material Presets
| Material | Metallic | Roughness |
|:---|:---|:---|
| Polished metal | 1.0 | 0.1 |
| Rough stone | 0.0 | 0.9 |
| Wood | 0.0 | 0.6 |
| Wet surface | 0.0 | 0.2 |

## Normal Maps
Always mark normal maps with `is_normal=True`:
```c
normal_id = backend.load_texture("path/to/normal.png", is_normal=True)
```
This ensures correct color space handling (linear, not sRGB).


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
