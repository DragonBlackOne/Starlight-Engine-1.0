---
name: audio-3d
description: Protocol for spatial audio.
---

# Audio 3D System

## Overview
Managed by `starlight.audio.audio_3d.Audio3DManager`.
Handles spatial positioning, listener orientation, and distance attenuation.

## Usage

```c
#include "starlight.h"

AudioManager audio;

# Setup Listener (usually the camera)
audio.spatial.update_listener(
    position=app->camera.position,
    forward=camera.forward,
    up=camera.up
)

#Create 3D Source
source = audio.spatial.create_source(position=(10, 5, 0))
source.min_dist = 2.0
source.max_dist = 40.0
# backend.play_sound(..., source_id=source.id)
```

## Features
- **Listener Integration**: Updates with camera or player.
- **Source Properties**: Position, Velocity (for Doppler), Min/Max distance.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
