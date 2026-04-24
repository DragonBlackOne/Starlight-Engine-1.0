---
name: model-optimization
description: Best practices for 3D models.
---

# Model Optimizations

1. **Triangle Count**: Keep under 50k for main characters, 1k for props.
2. **UVs**: Ensure 0-1 range.
3. **Tangents**: Export with tangents/binormals for normal mapping support.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
