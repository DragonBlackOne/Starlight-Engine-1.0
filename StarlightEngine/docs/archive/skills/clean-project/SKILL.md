---
name: clean-project
description: Deep cleaning to fix weird build errors or free space.
---

# Deep Clean Protocol

If strange linker errors persist:

1. **Cargo Clean**:
   ```powershell
   cargo clean
   ```
   *(Warning: Next build will take 5-10 mins)*

2. **Python Clean**:
   ```powershell
   Remove-Item -Recurse -Force **/pysrc/**/__pycache__
   ```

3. **Temp Files**:
   Delete `*.log` and `debug_output/*`.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
