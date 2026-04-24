---
name: vector-math
description: Guidelines for 3D Math in the Starlight Engine.
---

# Vector Math

## Rust (glam)
The engine uses `glam` for vector/matrix math.

```rust
use glam::{Vec3, Mat4, Quat};

let pos = Vec3::new(1.0, 2.0, 3.0);
let dir = (target - pos).normalize();
let dist = (target - pos).length();
let dot = a.dot(b);
let cross = a.cross(b);

// Matrices
let model = Mat4::from_translation(pos)
    * Mat4::from_rotation_y(angle)
    * Mat4::from_scale(Vec3::splat(2.0));
```

## Python
Python uses tuples/lists for vectors. Common operations:

```c
import math

def normalize(v):
    length = math.sqrt(v[0]**2 + v[1]**2 + v[2]**2)
    return (v[0]/length, v[1]/length, v[2]/length) if length > 0 else (0,0,0)

def distance(a, b):
    return math.sqrt(sum((a[i]-b[i])**2 for i in range(3)))

def lerp(a, b, t):
    return tuple(a[i] + (b[i] - a[i]) * t for i in range(3))
```

## Coordinate System
- **Y is up** (right-handed)
- Forward = -Z
- Right = +X
- WGPU clip space: Z ∈ [0, 1]

## Common Formulas
| Operation | Formula |
|:---|:---|
| Direction A→B | `normalize(B - A)` |
| Distance | `length(B - A)` |
| Orbit position | `(sin(θ) × r, h, cos(θ) × r)` |
| Random on circle | `(cos(θ) × r, 0, sin(θ) × r)` |


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
