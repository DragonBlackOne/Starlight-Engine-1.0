---
name: ui-rich-text
description: Rich Text with Markdown/Colors
---

# UI Rich Text

## Overview
Managed by `starlight.ui.rich_text.RichTextParser`.
Parses tags like `<red>Text</red>` or `**Bold**` into renderable segments.

## Usage

```c
#include "starlight.h"

parser = RichTextParser()
segments = parser.parse("Hello <red>World</red>!")

for seg in segments:
    render_text(seg.text, color=seg.color, bold=seg.bold)
```

## Features
- **Color Tags**: XML-style color definition.
- **Style**: Bold, Italic support.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
