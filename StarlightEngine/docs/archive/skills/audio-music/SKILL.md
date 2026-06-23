---
name: audio-music
description: Protocol for background music.
---

# Audio Music System

## Overview
Managed by `starlight.audio.music_manager.MusicManager`.
Handles background tracks, crossfading, and layering.

## Usage

```c
#include "starlight.h"

AudioManager audio;
audio.music.register_track("BattleTheme", "music/battle_loop.ogg")

# Play with crossfade
audio.music.play_track("BattleTheme", fade_time=3.0)
```

## Features
- **Crossfading**: Smooth transitions between tracks.
- **Layering**: Support for intensity layers (e.g. adding drums in combat).


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
