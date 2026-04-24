---
name: game-achievements
description: Achievement Tracking System
---

# Game Achievements

## Overview
Managed by `starlight.game.achievement.AchievementSystem`.
Tracks progress, unlocks, and persistence.

## Usage

```c
#include "starlight.h"

achs = AchievementSystem()

# Register
achs.register(Achievement("first_kill", "First Blood", "Kill 1 enemy"))
achs.register(Achievement("collector", "Collector", "Collect 100 coins", target=100))

# Updates
achs.unlock("first_kill")       # Instant unlock
achs.progress("collector", 10)  # Progress 10/100

# Callbacks
achs.on_unlock(lambda a: print(f"ACHIEVEMENT UNLOCKED: {a.name}"))
```

## Features
- **Progressive**: Tracks count (e.g. 50/100 kills).
- **Hidden**: Secret achievements support.
- **Serialization**: Save/load unlock state.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
