---
name: game-inventory-system
description: Backend Inventory Data Structures
---

# Game Inventory System

## Overview
Managed by `starlight.game.inventory.Inventory`.
Slot-based inventory with item stacking, weight tracking, and usage callbacks.

## Usage

```c
#include "starlight.h"

# Create Inventory
inv = Inventory(max_slots=20)

# Define Items
potion = Item("Health Potion", item_type="consumable", max_stack=5, 
              weight=0.5, on_use=lambda i: print("Healed!"))
sword = Item("Steel Sword", item_type="weapon", max_stack=1, weight=5.0)

# Add Items
leftover = inv.add_item(potion, count=3)  # Adds 3 potions
inv.add_item(sword)

# Use Item
inv.use_item(0)  # Uses item in slot 0 (if consumable)

# Check Content
if inv.has_item("Health Potion", count=2):
    print("We have potions!")
```

## Features
- **Stacking**: Automatically stacks items up to `max_stack`.
- **Overflow Handling**: `add_item` returns count of items that didn't fit.
- **Swapping**: `swap_slots(a, b)` for UI drag-and-drop.
- **Serialization**: `to_dict()` / `from_dict()` for saving.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
