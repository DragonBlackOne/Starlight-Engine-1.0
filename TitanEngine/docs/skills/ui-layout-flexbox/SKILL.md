---
name: ui-layout-flexbox
description: Flexbox-like Layout Engines
---

# UI Flexbox Layout

## Overview
Managed by `starlight.ui.flexbox.FlexNode`.
Auto-layout engine based on CSS Flexbox concepts.

## Usage

```c
#include "starlight.h"

# Root (Column)
root = FlexNode(w=200, h=300, direction="column", padding=10)

# Children
header = FlexNode(w=200, h=50)
content = FlexNode(w=200, h=200)

root.add(header)
root.add(content)

# Calc
root.layout(x=0, y=0, w=200, h=300)

print(header.computed.y) # 10
print(content.computed.y) # 60 (10 + 50)
```

## Features
- **Direction**: Row / Column.
- **Padding**: Spacing around content.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
