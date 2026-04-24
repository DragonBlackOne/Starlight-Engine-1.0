---
name: save-system
description: Saving game state.
---

# Save System

1. **Format**: JSON or Binary (Pickle).
2. **Location**: `user_data/saves/`.
3. **Async**: Write to disk in a separate thread to avoid frame drops.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
