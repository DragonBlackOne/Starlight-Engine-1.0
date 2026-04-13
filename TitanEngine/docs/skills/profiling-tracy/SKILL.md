---
name: profilng-tracy
description: Using Tracy for profiling rust.
---

# Tracy Profiler

## 1. Setup
Add `tracy-client` feature to `Cargo.toml`.

## 2. Instrumentation
```rust
let _span = tracy_client::span!("update_physics");
```

## 3. Usage
Run the game, open Tracy Server (Windows), and connect.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
