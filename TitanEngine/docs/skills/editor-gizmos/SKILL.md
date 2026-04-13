---
name: editor-gizmos
description: Transform Gizmos (Translate/Rotate/Scale)
---

# Editor Gizmos

## Overview
Managed by `starlight.editor.gizmos.Gizmos`.
Immediate mode drawing for debugging and tools.

## Usage

```c
#include "starlight.h"

gizmos = Gizmos()
gizmos.draw_line((0,0,0), (10,10,10), color=(1,0,0))
gizmos.draw_wire_cube((0,5,0), (1,1,1))
```

## Features
- **Shapes**: Lines, Cubes, Spheres (Wireframe).
- **Interaction**: (Planned) Axis handles.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
