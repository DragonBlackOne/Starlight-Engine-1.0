---
name: rust-concurrency
description: Threading in Rust.
---

# Concurrency

1. **Rayon**: Use `par_iter()` for parallel loops.
2. **Tokio**: Use for async I/O (loading assets).
3. **Mutex**: Use `parking_lot::Mutex` for better performance than std.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
