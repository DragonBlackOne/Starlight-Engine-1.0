---
name: anthropic-python-concepts
description: Advanced Python patterns for AI Agents (Functional, data-driven).
---

# Anthropic Python Patterns

## 1. Type Driven
- **Pydantic**: Use `Pydantic` models for structured data exchange between Agent and Tools.
- **Literals**: Use `Literal["option1", "option2"]` for finite choices.

## 2. Functional Core
- **Immutability**: Prefer frozen dataclasses.
- **Pure Functions**: Keep logic separate from Side Effects (I/O).

## 3. Tool Definition
- **Docstrings**: The Agent reads docstrings to understand tools. Be verbose and descriptive.
- **Return Type**: Explicitly state what the tool returns.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
