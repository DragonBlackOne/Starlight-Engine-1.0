---
name: ui-minimap
description: Minimap and Radar System
---

# UI Minimap

## Overview
Managed by `starlight.ui.minimap.Minimap`.
2D radar that converts world coordinates to screen space.

## Usage

```c
#include "starlight.h"

# Setup
minimap = Minimap(size=200, world_range=100)

# Update Player
minimap.set_player_pos(player.pos, player.yaw)

# Update Icons
minimap.add_icon(MinimapIcon("enemy_1", pos, color=(1,0,0)))

# Render Loop
points = minimap.get_render_data()
for pt in points:
    # Draw circular blip
    draw_circle(pt.screen_x, pt.screen_y, radius=3, color=pt.color)
```

## Features
- **Circle Clamping**: Icons stick to the edge if out of range.
- **Rotation**: Rotate map with player facing.
- **Layers**: Filter by icon type (enemy, quest, ally).


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
