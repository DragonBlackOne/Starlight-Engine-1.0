---
name: ai-utility-system
description: Utility-based Decision Making
---

# AI Utility System

## Overview
Managed by `starlight.ai.utility.UtilitySystem`.
Scores potential actions based on context (health, distance, ammo).

## Usage

```python
from starlight.ai.utility import UtilitySystem, UtilityAction, Scorer

us = UtilitySystem()

# Attack Action
attack = UtilityAction("Attack")
# Score based on (1 - distance_factor) -> Closer is better
attack.scorers.append(Scorer(input_func=get_dist_factor, curve_type="inverse", weight=2.0))
# Score based on ammo -> More ammo is better
attack.scorers.append(Scorer(input_func=get_ammo_percent, curve_type="linear"))

us.add_action(attack)

# Decide
best = us.select_best()
if best:
    best.execute()
```

## Features
- **Curves**: Linear, Quadratic, Logistic, Inverse.
- **Explainability**: `get_reasoning()` shows why an action was picked.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
