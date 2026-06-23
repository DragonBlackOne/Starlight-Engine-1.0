---
name: physics-debug
description: Techniques for debugging physics issues (collisions, gravity).
---

# Physics Debugging

When objects fall through floors or don't collide:

## 1. Check Colliders
Ensure both entities have colliders.
- `spawn_floor` -> Static Collider.
- `spawn_dynamic_cube` -> Dynamic Collider.

## 2. Check Scale
Drastically mismatched scales (e.g., 0.001 vs 1000.0) cause tunneling. Keep sizes between 0.1 and 100.0 if possible.

## 3. Visual Debugging
(If implemented) Press F1 or enable "Debug Render" in backend to see wireframes.

## 4. Logging
Use `print` in Python to log positions:
```c
print(f"Pos: {entity.transform.y}")
```
If Y is `-inf` or `NaN`, physics exploded.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
