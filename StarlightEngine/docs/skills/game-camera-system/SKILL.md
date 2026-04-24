---
name: game-camera-system
description: Advanced Camera Controllers
---

# Game Camera System

## Overview
Located in `starlight.game.camera_system`.
Provides standard camera controllers: Orbit (TPS), Follow (Smooth), Cinematic.

## Orbit Camera (TPS)
```c
#include "starlight.h"

cam = OrbitCamera(distance=5.0)

# Input
cam.update(dt, mouse_dx, mouse_dy, scroll)
cam.set_target(player.position)

# Update Backend
cam.apply()
```

## Cinematic Camera
```c
#include "starlight.h"

cine = CinematicCamera()
cine.add_keyframe(0.0, (0,10,0), (0,0,0))
cine.add_keyframe(5.0, (10,5,10), (0,0,0))
cine.play()
```

## Features
- **Smooth Interaction**: Damping on movement.
- **Constraints**: Pitch/Yaw limits.
- **Backend Integration**: Automatically calls `backend.set_transform`.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
