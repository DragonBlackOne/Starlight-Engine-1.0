---
name: event-system
description: Decoupled events.
---

# Event System

1. **Pattern**: Observer.
2. **Emit**: `events.emit("player_died", player_id)`.
3. **Subscribe**: `events.on("player_died", on_player_died)`.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
