---
name: ai-cover-system
description: Tactical Cover Finding System
---

# AI Cover System

## Overview
Managed by `starlight.ai.cover_system.CoverSystem`.
Finds defensive positions using geometry analysis.

## Usage

```python
from starlight.ai.cover_system import CoverSystem

cs = CoverSystem()
# (Assume populated with level data)

# Find cover
cover = cs.find_cover(seeker_pos=npc.pos, threat_pos=player.pos)
if cover:
    npc.move_to(cover.position)
    npc.crouch(cover.type == "half")
```

## Features
- **Flanking Check**: Ensures cover actually blocks the threat.
- **Quality Scoring**: Prefer closer cover with better angle.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
