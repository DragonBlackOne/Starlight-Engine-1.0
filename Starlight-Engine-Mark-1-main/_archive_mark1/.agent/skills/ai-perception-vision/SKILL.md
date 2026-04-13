---
name: ai-perception-vision
description: Visual Perception System
---

# AI Perception (Vision/Hearing)

## Overview
Managed by `starlight.ai.perception.PerceptionSystem`.
Handles visibility cones, hearing ranges, and short-term memory.

## Usage

```python
from starlight.ai.perception import PerceptionSystem, Sense, Stimulus

# Setup
ps = PerceptionSystem(owner.get_pos, owner.get_forward)
ps.add_sense(Sense("Vision", range=20.0, angle_degrees=90.0))
ps.add_sense(Sense("Hearing", range=15.0, angle_degrees=360.0))

# World System registers stimuli
ps.register_stimulus(Stimulus("Player", player.pos))
ps.register_stimulus(Stimulus("Footstep", pos, strength=0.5))

# Update
ps.update(dt)

# Query
if ps.has_perceived("Player"):
    last_pos = ps.get_last_known_pos("Player")
    moveto(last_pos)
```

## Features
- **Cone Check**: Dot product field of view.
- **Memory Decay**: Forgets targets after N seconds.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
