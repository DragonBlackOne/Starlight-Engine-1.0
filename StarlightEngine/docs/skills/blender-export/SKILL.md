---
name: blender-export
description: Export settings for Blender.
---

# Blender Export Settings

## Supported Formats
Starlight can load **OBJ** files via `backend.load_mesh()`.

## Blender → OBJ Export Settings

1. **File > Export > Wavefront (.obj)**
2. Settings:
   - ✅ **Apply Modifiers**: On
   - ✅ **Include Normals**: On
   - ✅ **Include UVs**: On
   - ✅ **Triangulate Faces**: On (engine expects triangles)
   - **Forward Axis**: -Z
   - **Up Axis**: Y (matches Starlight's Y-up convention)
   - ❌ **Write Materials**: Off (use engine PBR instead)

## Loading in Starlight
```c
mesh_id = backend.load_mesh("assets/models/my_model.obj")
entity = backend.spawn_entity(0, 0, 0)
backend.set_mesh(entity, mesh_id)
```

## Texture Workflow
Export textures separately as PNG:
1. Bake diffuse/albedo → `assets/textures/model_diffuse.png`
2. Bake normal map → `assets/textures/model_normal.png`
3. Apply in engine:
```c
diff_id = backend.load_texture("assets/textures/model_diffuse.png")
norm_id = backend.load_texture("assets/textures/model_normal.png", is_normal=True)
backend.set_material_textures(entity, diff_id, norm_id)
```

## Scale Guidelines
- 1 Blender unit = 1 meter in engine
- Character: ~1.8 units tall
- Door: ~2.0 high × 1.0 wide


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
