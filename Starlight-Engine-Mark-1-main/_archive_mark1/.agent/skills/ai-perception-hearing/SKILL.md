---
name: ai-perception-hearing
description: Auditory Perception System
---

# AI Perception (Hearing)

## Overview
Managed by `starlight.ai.perception.PerceptionSystem`.
Auditory events are processed as `Stimulus` with a position and strength.

## Usage

```python
from starlight.ai.perception import PerceptionSystem, Sense, Stimulus

# Add Hearing Sense
sense = Sense("Hearing", range=15.0, angle_degrees=360.0)
ai.perception.add_sense(sense)

# Emit Sound
# World logic calls this when an event happens
# strength=1.0 is loud (gunshot), 0.1 is quiet (step)
stim = Stimulus(tag="Gunshot", position=player.pos, strength=1.0)
ai.perception.register_stimulus(stim)

# React
if ai.perception.has_perceived("Gunshot"):
    investigate(ai.perception.get_last_known_pos("Gunshot"))
```

## Features
- **Omnidirectional**: Hearing usually covers 360 degrees.
- **Occlusion**: Can be extended to check for walls dampening sound.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
