---
name: editor-play-mode
description: Play/Simulate/Stop State Machine
---

# Editor Play Mode

## Overview
Managed by `starlight.editor.play_mode.PlayModeManager`.
Switches between Editing and Playing states.

## Usage

```c
#include "starlight.h"

pm = PlayModeManager()
pm.play() # Backs up scene, starts simulation
pm.stop() # Restores scene
```

## Features
- **Scene Backup**: Ensures edits during play don't persist (stub).
- **Pause/Step**: Supported.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
