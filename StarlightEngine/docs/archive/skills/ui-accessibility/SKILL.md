---
name: ui-accessibility
description: TTS and High Contrast Modes
---

# UI Accessibility

## Overview
Managed by `starlight.ui.accessibility.AccessibilityManager`.
Ensures game is playable by users with disabilities.

## Usage

```c
#include "starlight.h"

acc = AccessibilityManager()

# Text-to-Speech
acc.speak("Menu Opened")

# Color Blindness
final_color = acc.transform_color(original_color)
```

## Features
- **TTS**: Voice feedback for UI events.
- **Color Correction**: Simulation/Correction for Daltonism.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
