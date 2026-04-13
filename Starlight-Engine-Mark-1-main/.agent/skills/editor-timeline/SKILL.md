---
name: editor-timeline
description: Cinematic Timeline Editor
---

# Editor Timeline

## Overview
Managed by `starlight.editor.timeline.TimelineEditor`.
Keyframe animation editor for cutscenes.

## Usage

```c
#include "starlight.h"

timeline = TimelineEditor()
timeline.add_track("Camera Position")
timeline.add_key("Camera Position", time=0.0, value=(0,0,0))
timeline.play()
```

## Features
- **Tracks**: Property animation curves.
- **Playback**: Play/Pause/Seek.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
