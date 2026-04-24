---
name: platform-dlc-manager
description: DLC Content Management
---

# Platform DLC Manager

## Overview
Managed by `starlight.platform.dlc_manager.DLCManager`.
Checks for ownership and installation of downloadable content.

## Usage

```c
#include "starlight.h"

# Define DLC IDs
DLC_TEXTURE_PACK = 1001

dlc = DLCManager()

if dlc.is_owned(DLC_TEXTURE_PACK):
    if dlc.is_installed(DLC_TEXTURE_PACK):
        enable_high_res_textures()
    else:
        show_download_prompt()
```

## Features
- **Ownership Check**: Via Steam/Epic API.
- **Install Check**: Verifies files are present.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
