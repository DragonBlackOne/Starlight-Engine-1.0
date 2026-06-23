---
name: modding-support
description: How to support modders.
---

# Modding Support

## 1. Dynamic Loading
The engine already loads Python scripts dynamically.
- Users can drop `my_mod.py` in `demos/` and run it.

## 2. Asset Overrides
- Modders can replace assets in `assets/` to reskin game.

## 3. Safe API
- Expose only `starlight.*`, NOT `engine.backend` directly to avoid segfaults.
- Isolate mod state if possible.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
