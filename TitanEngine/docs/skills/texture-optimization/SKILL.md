---
name: texture-optimization
description: Best practices for texture assets.
---

# Texture Optimizations

1. **Size**: Power of 2 (512, 1024, 2048).
2. **Format**: .png for lossless (GUI), .jpg for diffuses (if size critical).
3. **Compression**: Engine uses internal compression (BC7) on load.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
