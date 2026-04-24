---
name: assets-manage
description: Instructions for checking, adding, and using game assets (Models, Textures, Audio). Use when a feature requires external content.
---

# Asset Management Protocol

Starlight expects assets in specific locations relative to the executable (or CWD).

## 1. Directory Structure
Ensure assets are placed here:
- `assets/models/` -> `.glb` or `.gltf` files.
- `assets/textures/` -> `.png` or `.jpg` files.
- `assets/audio/` -> `.ogg` or `.wav` files.

## 2. Using Models
When spawning an entity with a custom mesh:
1. Ensure `my_model.glb` is in `assets/models/`.
2. Use the filename (without extension is preferred by some loaders, but full name is safer) or the unique ID if registered.
   *Currently, the engine loads specific hardcoded names or requires strict paths.*
   
   **Standard Call:**
   ```c
   Entity.spawn(self, x, y, z, model="my_model.glb")
   ```
   *(Note: Verify if backend supports dynamic loading by string path or if it needs precaching)*

## 3. Placeholder Assets
If an asset is missing, DO NOT FAIL.
- Use `model="cube"` for missing meshes.
- Use `Color.RED` or `Color.MAGENTA` to indicate missing textures.

## 4. Verification
Before writing code that uses `assets/models/hero.glb`, run:
```powershell
Test-Path assets/models/hero.glb
```
If it returns False, inform the user or use a placeholder.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
