---
name: game-tutorial-system
description: Tutorial Step Manager
---

# Game Tutorial System

## Overview
Managed by `starlight.game.tutorial.TutorialSystem`.
Sequences tutorial steps based on conditions or timeouts.

## Usage

```c
#include "starlight.h"

tut = TutorialSystem()

# Steps
step1 = TutorialStep("move", "Use WASD to Move", 
                     condition=lambda: player.velocity.length() > 0.1)
step2 = TutorialStep("jump", "Press SPACE to Jump", 
                     condition=lambda: input.is_key_down(Keys.Space))

tut.add_step(step1)
tut.add_step(step2)

# Run
tut.start()

# Loop
tut.update(dt)
if tut.active:
    draw_tutorial_ui(tut.current_message)
```

## Features
- **Conditions**: Steps auto-complete when lambda returns True.
- **Timeouts**: Steps can auto-skip after N seconds.
- **Highlighting**: ID of UI element to highlight.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
