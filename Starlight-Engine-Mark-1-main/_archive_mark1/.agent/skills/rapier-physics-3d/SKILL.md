---
name: rapier-physics-3d
description: Guide for Rapier Physics integration in Rust.
---

# Rapier Physics (3D)

Starlight uses `rapier3d` for real-time physics simulation.

## Architecture

The physics state lives in an ECS resource:
```rust
pub struct PhysicsState {
    pub rigid_body_set: RigidBodySet,
    pub collider_set: ColliderSet,
    pub gravity: Vector<f32>,
    pub integration_parameters: IntegrationParameters,
    pub physics_pipeline: PhysicsPipeline,
    pub island_manager: IslandManager,
    pub broad_phase: DefaultBroadPhase,
    pub narrow_phase: NarrowPhase,
    pub ccd_solver: CCDSolver,
    pub query_pipeline: QueryPipeline,
    pub impulse_joint_set: ImpulseJointSet,
    pub multibody_joint_set: MultibodyJointSet,
}
```

Accessed via `app.world.resource_mut::<PhysicsState>()`.

## Spawning Physics Objects

### Dynamic Cube (falls, bounces)
```python
cube_id = backend.spawn_dynamic_cube(x, y, z, r, g, b)
# Has RigidBody (Dynamic) + Collider (Cuboid 0.5³)
```

### Static Box (immovable wall/floor)
```python
box_id = backend.spawn_static_box(x, y, z, sx, sy, sz, r, g, b)
# Has RigidBody (Fixed) + Collider (Cuboid sx×sy×sz)
# Visual scale = (sx*2, sy*2, sz*2) — collider is half-extents
```

### Character Controller (player)
```python
char_id = backend.spawn_character(x, y, z, speed=5.0, max_slope=0.78, offset=0.1)
# Has RigidBody (KinematicPositionBased) + Collider (Capsule) + CharacterController
```

## Physics Controls (From Python)

| Function | Purpose |
|:---|:---|
| `apply_impulse(id, ix, iy, iz)` | One-time force burst |
| `apply_force(id, fx, fy, fz)` | Continuous force |
| `set_velocity(id, vx, vy, vz)` | Override linear velocity |
| `get_velocity(id) → (vx, vy, vz)` | Read current velocity |
| `move_character(id, dx, dy, dz) → (x, y, z)` | KCC movement with collision |

## Raycasting
```python
hit = backend.raycast(
    ox, oy, oz,        # Origin
    dx, dy, dz,        # Direction (normalized)
    max_toi,           # Max distance
    solid,             # bool: solid vs hollow hits
    ignore_entity_id=None  # Optional: entity to ignore
)
# Returns: (entity_id, hit_x, hit_y, hit_z, normal_x, normal_y, normal_z) or None
```

## Common Issues

| Problem | Cause | Fix |
|:---|:---|:---|
| Object falls through floor | Collider not created | Ensure `spawn_static_box` used, not `spawn_entity` |
| Jittering physics | Transform sync out of order | Physics sync runs at 50Hz (`fixed_update`), not per-frame |
| Tunneling (penetration) | Object moves too fast | Enable CCD: `rigid_body.ccd_enabled(true)` in Rust |
| Body won't move after teleport | Physics body sleeping | Call `body.wake_up(true)` after position set |
| `Entity has no RigidBodyComponent` | Entity was spawned without physics | Use `spawn_dynamic_cube` or add physics manually |

## Collision Events
```python
events = backend.get_collision_events()
# Returns: list of (entity_a_id, entity_b_id, started: bool)
for a, b, started in events:
    if started:
        print(f"Collision started: {a} ↔ {b}")
```

## Physics Step (Rust Internal)
The physics pipeline steps at a fixed 50Hz in `physics_step_system`:
1. `PhysicsPipeline::step()` advances simulation
2. `sync_transforms_system` copies `RigidBody` position → `Transform` component
3. Python reads `Transform` for rendering


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
