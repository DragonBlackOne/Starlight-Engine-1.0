---
name: ui-widgets
description: Creating ImGui widgets in Python.
---

# UI Widgets

1. **Library**: Use `imgui` via `pysrc/starlight/editor/`.
2. **Loop**: Must be called inside `on_render`.
3. **Example**:
   ```c
   import imgui
   imgui.begin("My Window")
   if imgui.button("Click Me"):
       print("Clicked")
   imgui.end()
   ```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
