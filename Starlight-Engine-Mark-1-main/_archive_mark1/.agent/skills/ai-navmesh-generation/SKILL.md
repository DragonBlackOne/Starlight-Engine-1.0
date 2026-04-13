---
name: ai-navmesh-generation
description: Recast/Detour Navigation Mesh Generation
---

# AI NavMesh

## Overview
Managed by `starlight.ai.navmesh.NavMesh`.
Container for traversable polygons and connectivity.

## Usage

```python
from starlight.ai.navmesh import NavMesh

nm = NavMesh()

# Low-level construction (usually done by tool/importer)
nm.add_poly([(0,0), (10,0), (10,10), (0,10)])
nm.add_poly([(10,0), (20,0), (20,10), (10,10)])

# Bake connectivity
nm.bake()

# Query
poly_idx = nm.find_poly(5, 5) # Returns index of first poly
```

## Features
- **Graph Building**: Auto-connects neighbors based on shared edges.
- **Spatial Query**: Find poly containing X,Z.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
