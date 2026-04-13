---
name: python-numpy-integration
description: Using Numpy for speed.
---

# Numpy

1. **Avoid**: Converting large lists to numpy per frame.
2. **Buffer Protocol**: Use `buffer()` to pass numpy arrays to Rust directly (zero-copy).


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
