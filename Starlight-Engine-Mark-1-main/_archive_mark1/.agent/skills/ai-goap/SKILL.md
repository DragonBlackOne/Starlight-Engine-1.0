---
name: ai-goap
description: Goal Oriented Action Planning
---

# AI GOAP System

## Overview
Managed by `starlight.ai.goap.GoapPlanner`.
Dynamic planning system where agents chain actions to satisfy goals.

## Usage

```python
from starlight.ai.goap import GoapPlanner, GoapAction, GoapAgent

planner = GoapPlanner()

# Define Actions
chop_wood = GoapAction(
    name="ChopWood", 
    preconditions={"has_axe": True}, 
    effects={"has_wood": True}, cost=2
)
get_axe = GoapAction(
    name="GetAxe",
    effects={"has_axe": True}, cost=1
)

planner.add_action(chop_wood)
planner.add_action(get_axe)

# Plan
# Goal: Have wood. Start: Nothing.
# Result: [GetAxe, ChopWood]
plan = planner.plan(start_state={}, goal_state={"has_wood": True})
```

## Features
- **A* Search**: Finds lowest cost plan.
- **Dynamic**: Adapts to current state (e.g. already has axe).


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
