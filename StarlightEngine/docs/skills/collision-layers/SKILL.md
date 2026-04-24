---
name: collision-layers
description: Setting up collision masks.
---

# Collision Layers

## Bitmask System
Rapier3D uses groups and filters for collision filtering.

## Layer Convention
| Bit | Layer | Purpose |
|:---|:---|:---|
| `1` (0x01) | World | Static geometry, floors, walls |
| `2` (0x02) | Player | Player character |
| `4` (0x04) | Enemy | NPC enemies |
| `8` (0x08) | Projectile | Bullets, arrows |
| `16` (0x10) | Trigger | Sensors (no collision response) |
| `32` (0x20) | Pickup | Items, collectibles |

## Setting Filters
```c
# Entity belongs to group 2 (Player), collides with 1 (World) and 4 (Enemy)
backend.set_collision_filter(entity_id, group=2, mask=1|4)

# Projectile hits enemies but not player
backend.set_collision_filter(bullet_id, group=8, mask=1|4)

# Trigger zone detects player only
backend.set_collision_filter(trigger_id, group=16, mask=2)
```

## Rust Implementation
In `lib.rs`, collision groups use Rapier's `CollisionGroups`:
```rust
use rapier3d::geometry::CollisionGroups;

let groups = CollisionGroups::new(
    Group::from_bits_truncate(membership),  // Which group this belongs to
    Group::from_bits_truncate(filter),       // Which groups it collides with
);
collider_builder = collider_builder.collision_groups(groups);
```

## Rules
- Two objects collide only if A's `mask` includes B's `group` AND B's `mask` includes A's `group`
- Default: all layers collide with all layers
- Use sensors (Trigger layer) for overlap-only detection


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
