---
name: ecs-component
description: Instructions for creating a new ECS Component in Rust (Bevy ECS).
---

# ECS Components in Starlight

All components are defined in `crates/engine_core/src/ecs/components.rs`.

## Existing Components

| Component | Fields | Purpose |
|:---|:---|:---|
| `Transform` | `x, y, z` | World position |
| `Rotation` | `x, y, z` | Euler rotation (radians) |
| `Scale` | `x, y, z` | Scale multiplier |
| `Color` | `r, g, b, a` | Vertex color |
| `MeshName` | `String` | Mesh asset ID |
| `Material` | `diffuse, normal` | Texture pair |
| `Camera` | `fov, near, far` | Perspective camera |
| `RigidBodyComponent` | `RigidBodyHandle` | Rapier physics body |
| `ColliderComponent` | `ColliderHandle` | Rapier collider |
| `CharacterController` | `speed, max_slope, offset` | Player movement |
| `Parent` | `u64` | Hierarchy parent ID |
| `PreviousTransform` | `x, y, z` | Interpolation state |
| `PreviousRotation` | `x, y, z` | Interpolation state |

## Creating a New Component

### 1. Define in Rust
```rust
use bevy_ecs::prelude::*;

#[derive(Component, Debug, Clone)]
pub struct Health {
    pub current: f32,
    pub max: f32,
}

impl Health {
    pub fn new(max: f32) -> Self {
        Self { current: max, max }
    }
}
```

### 2. Add to Entity Spawn (if needed by default)
In `lib.rs`, inside `spawn_entity` or a new spawner:
```rust
let id = app.world.spawn((
    Transform::new(x, y, z),
    Rotation::new(0.0, 0.0, 0.0),
    // ... other components ...
    Health::new(100.0),  // ← Add here
)).id();
```

### 3. Expose to Python (Optional)
Create accessor functions in `lib.rs`:
```rust
#[pyfunction]
fn get_health(entity_id: u64) -> PyResult<(f32, f32)> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        let entity = Entity::from_bits(entity_id);
        if let Some(h) = app.world.get::<Health>(entity) {
            Ok((h.current, h.max))
        } else {
            Err(PyValueError::new_err("No Health component"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_health(entity_id: u64, current: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let entity = Entity::from_bits(entity_id);
        if let Some(mut h) = app.world.get_mut::<Health>(entity) {
            h.current = current;
            Ok(())
        } else {
            Err(PyValueError::new_err("No Health component"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}
```

### 4. Register & Rebuild
1. Add `m.add_function(wrap_pyfunction!(get_health, m)?)?;` in `register_functions()`
2. Run the `rust-build` skill

## Rules
- Always derive `Component` (required by Bevy ECS)
- Derive `Debug` and `Clone` for convenience
- Keep fields `pub` for ECS query access
- If the component uses `bytemuck` for GPU upload, also derive `Pod` and `Zeroable` with `#[repr(C)]`


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
