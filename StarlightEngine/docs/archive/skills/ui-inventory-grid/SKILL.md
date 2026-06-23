---
name: ui-inventory-grid
description: Grid-based Inventory UI
---

# UI Inventory Grid

## Overview
Managed by `starlight.ui.inventory_grid.InventoryGridUI`.
Calculates layout and input for a grid inventory.

## Usage

```c
#include "starlight.h"

# Create Layout
grid = InventoryGridUI(columns=5, rows=4, cell_size=64)
grid.set_inventory(player.inventory)

# Input
grid.handle_mouse(mouse_x, mouse_y, clicked=mouse_click)

# Callbacks
grid.on_slot_click(lambda slot, btn: print(f"Clicked {slot}"))
grid.on_slot_swap(lambda a, b: player.inventory.swap(a, b))

# Render
for cell in grid.cells:
    draw_rect(cell.x, cell.y, cell.width, cell.height)
    if not cell.empty:
        draw_icon(cell.item_icon, cell.x, cell.y)
        draw_text(str(cell.count), cell.x, cell.y)
```

## Features
- **Drag & Drop**: Logic for drag start/end/swap.
- **Layout**: Auto-calculates pixel positions.
- **Selection**: Tracks highlighted/selected slots.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
