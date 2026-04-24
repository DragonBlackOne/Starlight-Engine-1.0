---
name: game-stat-system
description: RPG Stats and Modifiers
---

# Game Stat System

## Overview
Managed by `starlight.game.stat_system.StatSystem`.
Supports base values, min/max bounds, and stackable modifiers (Flat/Percent).

## Usage

```c
#include "starlight.h"

class Player:
    def __init__(self):
        self.stats = StatSystem()
        self.stats.add_stat("health", base=100, max_val=200)
        self.stats.add_stat("strength", base=10)

    def equip_sword(self):
        # +5 Flat Strength
        mod = Modifier("iron_sword", 5.0, ModType.FLAT)
        self.stats.add_modifier("strength", mod)

    def rage_mode(self):
        # +50% Strength for 10 seconds
        mod = Modifier("rage", 0.5, ModType.PERCENT, duration=10.0)
        self.stats.add_modifier("strength", mod)

    def update(self, dt):
        self.stats.update(dt)  # Updates timed modifiers
```

## Modifier Application Order
1. **Base Value**
2. **Flat Modifiers** (Summed)
3. **Percent Modifiers** (Summed, then Multiplied)

`Value = (Base + Σ Flat) * (1.0 + Σ Percent)`


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
