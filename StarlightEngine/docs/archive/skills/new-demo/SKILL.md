---
name: new-demo
description: Instructions for creating a new playable demo in the Starlight Engine. Use when the user asks to "test feature X" or "create a demo for Y".
---

# Creating a Starlight Demo

## 1. Location & Naming
- Demos go in `demos/` directory.
- Filename: `snake_case.py` (e.g., `demos/physics_playground.py`).

## 2. Template (Copy-Paste Ready)

```c
"""Demo Name — Brief description of what this demonstrates."""
import math
import os
import sys

# Path setup — always prepend pysrc so local code overrides installed packages
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../pysrc"))

#include "starlight.h"

class MyDemo(App):
    def on_start(self):
        print("[DEMO] Initializing...")

        # Camera reference
        self.cam_id = backend.get_main_camera_id()

        # Ground plane
        self.floor = backend.spawn_static_box(0.0, -1.0, 0.0, 50.0, 1.0, 50.0, 0.3, 0.5, 0.3)

        # Lighting (Golden Hour preset)
        backend.set_sun_direction(-0.5, -1.0, -0.5)
        backend.set_sun_color(1.0, 0.9, 0.7, 2.0)  # r, g, b, intensity
        backend.set_ambient_intensity(0.15)
        backend.set_fog(0.01, 0.5, 0.6, 0.7)

        # Feature entities
        self.cube = backend.spawn_entity(0, 2, 0)
        backend.set_mesh(self.cube, "cube")
        backend.set_color(self.cube, 0.8, 0.2, 0.2, 1.0)

        self.t = 0.0
        self.captured = False

    def on_update(self, dt):
        self.t += dt

        # Orbit camera
        cam_x = math.sin(self.t * 0.3) * 10.0
        cam_z = math.cos(self.t * 0.3) * 10.0
        backend.set_transform(self.cam_id, cam_x, 5.0, cam_z)
        backend.set_rotation(self.cam_id, -0.3, math.atan2(cam_x, -cam_z), 0.0)

        # Auto-screenshot after 3 seconds
        if self.t > 3.0 and not self.captured:
            backend.capture_screenshot("screenshots/my_demo.png")
            print("[DEMO] Screenshot saved!")
            self.captured = True

if __name__ == "__main__":
    os.makedirs("screenshots", exist_ok=True)
    MyDemo(title="Starlight - My Demo").run()
```

## 3. Available Backend Functions (Most Used)

| Function | Signature | Notes |
|:---|:---|:---|
| `spawn_entity` | `(x, y, z) → u64` | Bare entity, no mesh/physics |
| `spawn_static_box` | `(x,y,z, sx,sy,sz, r,g,b) → u64` | Has collider |
| `spawn_dynamic_cube` | `(x,y,z, r,g,b) → u64` | Physics-enabled |
| `set_mesh` | `(id, mesh_name)` | "cube" is built-in |
| `set_color` | `(id, r,g,b,a)` | |
| `set_material_textures` | `(id, diffuse_id, normal_id)` | |
| `set_transform` | `(id, x,y,z)` | |
| `set_rotation` | `(id, x,y,z)` | Euler radians |
| `set_scale` | `(id, sx,sy,sz)` | |
| `load_mesh` | `(path) → string_id` | OBJ files |
| `load_texture` | `(path) → string_id` | PNG/JPG |
| `capture_screenshot` | `(path)` | GPU readback |

## 4. Framework Alternative (Higher Level)
For cleaner code, use the new OOP framework:
```c
from`starlight.framework` import Scene, Entity, Camera

scene = Scene()
scene.set_sun(direction=(0.5, -1.0, 0.5), color=(1.0, 0.9, 0.7), intensity=2.0)
entity = Entity("MyCube", 0, 2, 0)
entity.set_mesh("cube").set_color(0.8, 0.2, 0.2)
ecs_add_entity(world, entity)
camera = Camera()
camera.look_at((0, 2, 0))
```

## 5. Best Practices
- Always include a ground plane for visual reference.
- Always set lighting — the default is very dim.
- Use `backend.get_main_camera_id()` for camera control.
- Auto-capture a screenshot for verification.
- Do NOT use `loguru` — it's not a project dependency. Use `print()`.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
