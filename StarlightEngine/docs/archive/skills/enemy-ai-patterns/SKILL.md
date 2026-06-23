---
name: enemy-ai-patterns
description: Common enemy behaviors.
---

# Enemy AI

1. **Patrol**: Move between waypoints.
2. **Aggro**: If `dist(player, self) < radius`, switch to Chase.
3. **Attack**: If `dist < range`, stop and fire.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
