---
name: game-interaction-system
description: Interaction (Press F) System
---

# Game Interaction System

## Overview
Managed by `starlight.game.interaction.InteractionSystem`.
Handles proximity-based interactions ("Press F to Interact").

## Usage

```c
#include "starlight.h"

sys = InteractionSystem()

# Register Object
door = Interactable(
    obj_id="door_1",
    position=(10, 0, 5),
    radius=2.0,
    prompt="Open Door",
    on_interact=lambda: print("Door Opened!")
)
sys.register(door)

# Update Loop
sys.update(player_pos=(10.5, 0, 5))

# Check UI
if sys.can_interact:
    show_ui(f"Press F to {sys.current_prompt}")

# Input Hearing
if input.is_key_just_pressed(Keys.F):
    sys.interact()
```

## Features
- **Spatial Lookup**: Automatically finds nearest interactable.
- **One-shot**: Objects can be marked to only interact once.
- **Dynamic prompts**: Context-sensitive prompts.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
