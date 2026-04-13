---
name: ai-pathfinding-astar
description: Multi-threaded A* Pathfinding
---

# AI Pathfinding

## Overview
Managed by `starlight.ai.pathfinding.Pathfinder`.
Standard A* search on NavMesh polygons.

## Usage

```python
from starlight.ai.pathfinding import Pathfinder
from starlight.ai.navmesh import NavMesh

nav = NavMesh()
# ... build/load navmesh ...

pf = Pathfinder(nav)
path = pf.find_path(start=(0,0,0), end=(50,0,50))

# Result: List of points [(0,0,0), (10,0,10), ..., (50,0,50)]
actor.follow_path(path)
```

## Features
- **NavMesh Support**: Node graph is built from polygons.
- **Heuristic**: Euclidean distance.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
