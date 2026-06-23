---
name: ui-world-space
description: 3D World Space Widgets
---

# UI World Space

## Overview
Managed by `starlight.ui.world_ui.WorldUIPanel`.
UI elements that float in the 3D world (e.g. health bars, names).

## Usage

```c
#include "starlight.h"

panel = WorldUIPanel()
panel.world_pos = (10, 5, 10)
panel.offset = (0, 1, 0)

# Update per frame
panel.update(camera_pos, view_mat, proj_mat, screen_size)

if panel.visible:
    render_at(panel.screen_pos, scale=panel.scale)
```

## Features
- **Distance Scaling**: Smaller when far away.
- **Culling**: Hide if behind camera or too far.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
