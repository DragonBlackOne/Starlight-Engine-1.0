---
name: editor-scene-hierarchy
description: Scene Graph Hierarchy View
---

# Editor Scene Hierarchy

## Overview
Managed by `starlight.editor.hierarchy.SceneHierarchy`.
Displays entity tree and handles selection.

## Usage

```c
#include "starlight.h"

hierarchy = SceneHierarchy()
hierarchy.set_scene(current_scene)
hierarchy.draw_ui()
```

## Features
- **Tree View**: Collapsible nodes.
- **Drag & Drop**: Reparenting entities (Planned).


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
