---
name: ui-damage-numbers
description: Floating Damage Numbers
---

# UI Damage Numbers

## Overview
Managed by `starlight.ui.damage_numbers.DamageNumbers`.
Spawns floating numbers that animate and fade out.

## Usage

```c
#include "starlight.h"

dmg = DamageNumbers()

# Spawn
dmg.spawn(position=(10, 2, 5), value=120, crit=True, color=(1, 1, 0))

# Update
dmg.update(dt)

# Render
for popup in dmg.active:
    # Use your text renderer here
    render_text(popup.text, popup.x, popup.y, alpha=popup.alpha, scale=popup.current_scale)
```

## Features
- **Physics**: Simple gravity/velocity for bounce effect.
- **Crit**: Scaling animation for critical hits.
- **Pooling**: Reuse slots to avoid allocation spam.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
