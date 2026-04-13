---
name: editor-inspector
description: Reflection-based Property Inspector
---

# Editor Inspector

## Overview
Managed by `starlight.editor.inspector.Inspector`.
Auto-generates UI for editing object properties.

## Usage

```c
#include "starlight.h"

inspector = Inspector()
inspector.inspect(selected_entity)
inspector.draw_ui()
```

## Features
- **Reflection**: Iterates `__dict__` to show fields.
- **Widgets**: Selects appropriate widget (InputText, DragFloat) based on type.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
