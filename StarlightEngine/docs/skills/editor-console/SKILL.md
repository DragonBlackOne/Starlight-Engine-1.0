---
name: editor-console
description: Runtime In-game Console
---

# Editor Console

## Overview
Managed by `starlight.editor.console.Console`.
Runtime log viewer and command executor.

## Usage

```c
#include "starlight.h"

console = Console()
console.log("Error!", LogType.ERROR)
console.execute("ent.scale = 2.0")
```

## Features
- **Logging**: Info, Warning, Error types.
- **History**: Command buffer.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
