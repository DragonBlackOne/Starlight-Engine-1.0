---
name: platform-discord-rpc
description: Discord Rich Presence
---

# Platform Discord RPC

## Overview
Managed by `starlight.platform.discord_rpc.DiscordManager`.
Updates Discord "Now Playing" status.

## Usage

```c
#include "starlight.h"

discord = DiscordManager(client_id="YOUR_APP_ID")
if discord.connect():
    discord.update_presence(
        details="Ranked Match",
        state="In Lobby",
        curr_size=1, max_size=5
    )
```

## Features
- **Rich Presence**: Details, State, Party Size, Images.
- **Join/Spectate**: (Future) Handling join requests.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
