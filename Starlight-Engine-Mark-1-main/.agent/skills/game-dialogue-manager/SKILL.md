---
name: game-dialogue-manager
description: Branching Dialogue Backend
---

# Game Dialogue Manager

## Overview
Managed by `starlight.game.dialogue.DialogueManager`.
Node-based dialogue tree with choices, conditions, and callbacks.

## Usage

```c
#include "starlight.h"

dm = DialogueManager()

# Define Nodes
start = DialogueNode("start", "Merchant", "Welcome! Would you like to trade?")
trade = DialogueNode("trade", "Merchant", "Here are my wares.")
bye = DialogueNode("bye", "Merchant", "Safe travels.")

# Add Choices
start.add_choice("Yes", "trade")
start.add_choice("No", "bye")

# Register
dm.add_node(start)
dm.add_node(trade)
dm.add_node(bye)

# Run
dm.start("start")
print(dm.current_text) # "Welcome..."
dm.choose(0)           # Pick "Yes" -> Goes to "trade"
```

## Features
- **Conditions**: Show choices only if conditions met (e.g. `has_gold >= 10`).
- **Callbacks**: `on_enter`, `on_exit`, `on_select` used for scripts/events.
- **Auto-advance**: Nodes can auto-transition without user choice.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
