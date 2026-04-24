---
name: procedural-generation
description: Algorithms for content generation.
---

# Procedural Gen

1. **Strategy**: Don't build whole worlds at once. Chunk them.
2. **Tools**: Use `noise-algorithms` skill.
3. **Integration**: Generate data in Python, spawn meshes in Rust via `spawn_chunk(data)`.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
