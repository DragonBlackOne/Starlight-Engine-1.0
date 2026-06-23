---
name: game-save-cloud
description: Cloud Save Integration Hooks
---

# Cloud Save System

## Overview
Managed by `starlight.game.save_cloud.CloudSaveManager`.
Abstracts local vs cloud storage (Steam/Custom).

## Usage

```c
#include "starlight.h"

# Init (defaults to 'local' provider)
cloud = CloudSaveManager(provider="local")

# Save
data = {"xp": 100, "level": 2}
cloud.save("slot_1", data)

# Load
save_data = cloud.load("slot_1")
if save_data:
    print("Loaded!")

# List
saves = cloud.list_saves() # Returns list sorted by date
```

## Providers
- `"local"`: JSON files in `saves/` folder.
- `"steam"`: Hooks for Steam Cloud (placeholder).
- `"custom"`: Hooks for HTTP API (placeholder).


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
