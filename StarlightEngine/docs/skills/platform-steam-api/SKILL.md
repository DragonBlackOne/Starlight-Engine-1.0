---
name: platform-steam-api
description: Steamworks API Integration
---

# Platform Steam API

## Overview
Managed by `starlight.platform.steam_api.SteamManager`.
Wrapper around `steam_api64.dll` via `ctypes`.

## Usage

```c
#include "starlight.h"

steam = SteamManager(app_id=480) # Spacewar ID

if steam.init():
    user = steam.get_persona_name()
    print(f"Hello {user}")
    
    steam.unlock_achievement("FIRST_LOGIN")
    
# On Update
# steam.run_callbacks()
```

## Features
- **Identity**: Get player name/ID.
- **Stats/Achievements**: Unlock and store.
- **Cloud**: (via `game-save-cloud` integration).


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
