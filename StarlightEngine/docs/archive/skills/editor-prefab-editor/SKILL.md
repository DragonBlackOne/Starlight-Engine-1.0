---
name: editor-prefab-editor
description: Prefab Editing System
---

# Editor Prefab Editor

## Overview
Managed by `starlight.editor.prefab_editor.PrefabEditor`.
Isolated editing environment for Prefab assets.

## Usage

```c
#include "starlight.h"

pe = PrefabEditor()
pe.load("assets/enemy.prefab")
# Edit...
pe.save()
```

## Features
- **Isolation**: Edits in a blank scene context.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
