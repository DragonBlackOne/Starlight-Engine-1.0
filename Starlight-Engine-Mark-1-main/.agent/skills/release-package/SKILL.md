---
name: release-package
description: packaging the game for distribution.
---

# Release Packaging

To create a distributable build:

1. **Build Core**: `cargo build --release -p engine_core`
2. **Prepare Folder**: `mkdir Build`
3. **Copy Assets**: `cp -r assets Build/assets`
4. **Copy Python Source**: `cp -r pysrc Build/pysrc`
5. **Copy Extension**: `cp target/release/backend.dll Build/pysrc/starlight/backend.pyd`
6. **Copy Scripts**: `cp demos/my_game.py Build/launch.py`

7. **Run**: Ensure user has Python installed.
   ```powershell
   cd Build
   python launch.py
   ```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
