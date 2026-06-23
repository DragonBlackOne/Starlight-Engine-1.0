---
name: ui-settings-menu
description: Auto-generated Settings Menu
---

# UI Settings Menu

## Overview
Managed by `starlight.ui.settings_menu.SettingsMenu`.
Data-driven settings with persistence.

## Usage

```c
#include "starlight.h"

menu = SettingsMenu("saves/config.json")

# Define Settings
menu.add(Setting(
    "Volume", "audio.master", 
    kind="slider", min_val=0, max_val=100, default=50,
    on_change=lambda v: backend.set_volume(v/100)
))

menu.add(Setting(
    "Fullscreen", "display.fullscreen", 
    kind="toggle", default=False
))

# Load & Apply
if menu.load():
    menu.apply_all()
```

## Features
- **Types**: Slider, Toggle, Dropdown.
- **Persistence**: Save/Load JSON automatically.
- **Categories**: Group settings by category tag.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
