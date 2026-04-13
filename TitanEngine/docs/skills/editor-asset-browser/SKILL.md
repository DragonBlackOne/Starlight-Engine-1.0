---
name: editor-asset-browser
description: Asset Browser with Thumbnails
---

# Editor Asset Browser

## Overview
Managed by `starlight.editor.asset_browser.AssetBrowser`.
Explorer-like view of the project `assets/` folder.

## Usage

```c
#include "starlight.h"

browser = AssetBrowser(root_path="./assets")
browser.refresh()
browser.draw_ui()
```

## Features
- **Navigation**: Directories and files.
- **Filtering**: (Planned) by type.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
