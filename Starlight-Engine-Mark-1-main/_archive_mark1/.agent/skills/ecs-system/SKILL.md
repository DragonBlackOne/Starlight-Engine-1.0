---
name: ecs-system
description: Instructions for creating a new ECS System in Rust.
---

# Creating a System

Systems contain the logic that operates on Components.

## 1. Definition
Functions that take `Query`, `Res`, or `Commands`.

```rust
pub fn regenerate_health(time: Res<Time>, mut query: Query<&mut Health>) {
    for mut health in query.iter_mut() {
        health.current += 1.0 * time.delta_seconds();
    }
}
```

## 2. Registration
You MUST add the system to the `App` builder in `crates/engine_core/src/lib.rs` (inside `run_engine` or setup):

```rust
app.add_systems(Update, regenerate_health);
```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
