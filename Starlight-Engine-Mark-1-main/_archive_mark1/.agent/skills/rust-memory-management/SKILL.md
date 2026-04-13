---
name: rust-memory-management
description: Rust safety guidelines.
---

# Memory Management

1. **Ownership**: Pass by reference `&T` when possible.
2. **Cloning**: Avoid `clone()` in hot loops.
3. **Unsafe**: Avoid unless interfacing with FFI/GL.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
