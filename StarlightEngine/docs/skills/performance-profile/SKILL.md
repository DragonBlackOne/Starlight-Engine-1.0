---
name: performance-profile
description: How to profile the engine for bottlenecks.
---

# Profiling

## 1. Python Profiling
Use `cProfile` for scripts:
```powershell
python -m cProfile -o profile.stats demos/my_demo.py
```
Analyze with `snakeviz` or similar.

## 2. Rust/GPU Profiling
- Use **nsight systems** (NVIDIA) or **Renderdoc** for GPU frame analysis.
- Use `cargo flamegraph` for Rust CPU usage.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
