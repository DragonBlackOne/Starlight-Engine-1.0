---
name: microsoft-rust-style
description: Pragmatic Rust guidelines adapted from Microsoft's open source standards.
---

# Microsoft Pragmatic Rust

## 1. Error Handling
- **Prefer `Result`**: Always return `Result` for fallible operations.
- **Context**: Use `.with_context(|| "msg")` (anyhow/eyre) to provide debug info.
- **Panic**: Only panic on logic bugs (unreachable code), never on user input.

## 2. Clippy
- **Enforce**: Run `cargo clippy` in CI.
- **Pedantic**: Enable `#![warn(clippy::pedantic)]` for libraries.

## 3. Documentation
- **Examples**: Public functions must have doc examples.
- **Safety**: `unsafe` blocks must have a `// SAFETY:` comment explaining why it's safe.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
