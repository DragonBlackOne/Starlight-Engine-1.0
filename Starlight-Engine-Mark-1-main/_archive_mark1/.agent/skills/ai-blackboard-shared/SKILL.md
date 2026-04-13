---
name: ai-blackboard-shared
description: Shared Blackboard for Squad AI
---

# AI Shared Blackboard

## Overview
Managed by `starlight.ai.blackboard.SharedBlackboard`.
Allows agents to share data (e.g., "target position", "alert state") with timestamps.

## Usage

```python
from starlight.ai.blackboard import SharedBlackboard

# Squad Setup
squad_board = SharedBlackboard()

# Agent 1 (Spotter)
squad_board.set_timed("enemy_pos", (100, 0, 50), timestamp=time.time())

# Agent 2 (Sniper)
if squad_board.is_fresh("enemy_pos", current_time=time.time(), max_age=5.0):
    target = squad_board.get("enemy_pos")
    shoot(target)
```

## Features
- **Data Expiry**: Validate data freshness.
- **Squad Communication**: Shared knowledge base.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
