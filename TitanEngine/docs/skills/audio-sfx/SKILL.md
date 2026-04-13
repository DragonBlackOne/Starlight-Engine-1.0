---
name: audio-sfx
description: Protocol for adding sound effects.
---

# Audio SFX System

## Overview
Managed by `starlight.audio.sfx_manager.SFXManager`.
Handles one-shot sound effects, random variations, and pitching.

## Usage

```c
#include "starlight.h"

AudioManager audio;
audio.sfx.register("Gunshot", ["sfx/shot_01.wav", "sfx/shot_02.wav"])

# Play with variance
audio.sfx.play(
    "Gunshot", 
    position=(10,0,0), 
    variance=0.1 # +/- 10% pitch shift
)
```

## Features
- **Variance**: Random pitch/volume slight changes for realism.
- **Pooling**: Reuse valid clips.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
