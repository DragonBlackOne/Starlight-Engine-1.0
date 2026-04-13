---
name: quaternion-rotation
description: Handle rotations correctly.
---

# Quaternion & Euler Rotation

## Starlight Convention
The engine uses **Euler angles (radians)** for rotation, stored in the `Rotation` component.

```c
# Set rotation (pitch, yaw, roll) in radians
backend.set_rotation(entity_id, pitch, yaw, roll)

# Convert degrees to radians
import math
angle_rad = math.radians(45)  # 45° → 0.785 rad
```

## Common Rotations
| Intent | Values (radians) |
|:---|:---|
| Face +X | `(0, π/2, 0)` |
| Face -X | `(0, -π/2, 0)` |
| Face +Z | `(0, 0, 0)` |
| Face -Z | `(0, π, 0)` |
| Look up 30° | `(-π/6, 0, 0)` |
| Look down 30° | `(π/6, 0, 0)` |

## Look-At Calculation
```c
import math

def look_at(source_pos, target_pos):
    """Calculate pitch and yaw to face a target."""
    dx = target_pos[0] - source_pos[0]
    dy = target_pos[1] - source_pos[1]
    dz = target_pos[2] - source_pos[2]
    dist = math.sqrt(dx*dx + dy*dy + dz*dz)
    if dist < 0.001:
        return (0, 0)
    pitch = math.asin(dy / dist)
    yaw = math.atan2(dx, -dz)
    return (-pitch, yaw)
```

## Rapier Physics (Rust)
Rapier uses quaternions internally:
```rust
use rapier3d::na::UnitQuaternion;
let rotation = UnitQuaternion::from_euler_angles(pitch, yaw, roll);
rb.set_rotation(rotation, true);
```

## Gotcha: Gimbal Lock
Euler angles can suffer from gimbal lock at ±90° pitch. For smooth camera, clamp pitch to `(-π/2 + 0.01, π/2 - 0.01)`.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
