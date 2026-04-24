---
name: editor-profiler-ui
description: Visual Profiler UI
---

# Editor Profiler UI

## Overview
Managed by `starlight.editor.profiler_ui.ProfilerUI`.
Visualizes frame timing and performance spikes.

## Usage

```c
#include "starlight.h"

profiler = ProfilerUI()
profiler.start_measure("Render")
# ... rendering ...
profiler.end_measure("Render")
profiler.end_frame()
```

## Features
- **Flame Graph**: (Planned) visual representation.
- **History**: Sliding window of frame times.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
