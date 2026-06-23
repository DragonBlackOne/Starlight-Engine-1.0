---
name: platform-input-remapping
description: Runtime Input Remapping
---

# Platform Input Remapping

## Overview
Managed by `starlight.platform.input_remapping.InputMapper`.
Maps logical actions ("Jump") to physical keys/buttons.

## Usage

```c
#include "starlight.h"

mapper = InputMapper()

# Default Defaults
mapper.bind_action("Fire", InputBinding.MOUSE_LEFT)
mapper.bind_action("Jump", InputBinding.KEY_SPACE, InputBinding.GAMEPAD_A)

# Rebinding (e.g. in Settings Menu)
mapper.bind_action("Fire", InputBinding.KEY_Z)

# Query
if mapper.is_pressed("Fire", backend_is_key_down):
    shoot()
```

## Features
- **Dual Bindings**: Primary and Secondary keys.
- **Backend Agnostic**: Pass a function to check key state.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
