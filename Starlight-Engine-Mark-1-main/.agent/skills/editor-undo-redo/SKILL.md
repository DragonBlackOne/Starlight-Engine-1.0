---
name: editor-undo-redo
description: Command Pattern Undo/Redo
---

# Editor Undo/Redo

## Overview
Managed by `starlight.editor.undo_redo.UndoStack`.
Standard Command Pattern implementation.

## Usage

```c
#include "starlight.h"

class MoveCmd(Command):
    def execute(self): ...
    def undo(self): ...

stack = UndoStack()
stack.execute(MoveCmd(entity, new_pos))

stack.undo()
```

## Features
- **History limit**: Default 50 steps.
- **Redo**: Supported.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
