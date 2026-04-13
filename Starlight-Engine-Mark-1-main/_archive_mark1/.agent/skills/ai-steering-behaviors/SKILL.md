---
name: ai-steering-behaviors
description: Seek, Flee, Arrive, Flocking Behaviors
---

# AI Steering Behaviors

## Overview
Managed by `starlight.ai.steering_system.SteeringBehavior`.
Calculates steering forces to move agents smoothly.

## Usage

```python
from starlight.ai.steering_system import SteeringBehavior

steering = SteeringBehavior()

# Seek
# Returns (x,y,z) force vector
force = steering.seek(
    position=agent.pos, 
    target=target.pos, 
    max_speed=5.0
)

# Flee
flee_force = steering.flee(
    position=agent.pos, 
    target=enemy.pos, 
    max_speed=5.0
)

# Apply
agent.velocity += force * dt
```

## Features
- **Seek**: Move towards target.
- **Flee**: Move away from target.
- **Arrive**: Slow down as target is approached (can be implemented by scaling force).


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
