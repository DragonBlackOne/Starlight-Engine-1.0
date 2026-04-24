---
name: game-design-patterns
description: Common patterns from 'Game Programming Patterns' (Nystrom).
---

# Game Design Patterns

## 1. Component
- **Usage**: Decouple domain data into reusable components (ECS).
- **Rule**: A component should not know about the Entity logic, only its own data.

## 2. Observer
- **Usage**: Events. decoupling achievements/audio from physics.
- **Rule**: "Fire and forget".

## 3. Double Buffer
- **Usage**: Rendering.
- **Rule**: Update state A, Render state B. Flip. (Handled by WGPU backend).

## 4. Object Pool
- **Usage**: Particles, Bullets.
- **Rule**: Reuse entities instead of distinct `spawn/despawn` calls to avoid GC spikes (Python) or Allocations (Rust).


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
