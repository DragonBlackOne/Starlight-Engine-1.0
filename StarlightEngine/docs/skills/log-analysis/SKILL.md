---
name: log-analysis
description: Analyzing Starlight logs (Python and Rust).
---

# Log Analysis

## 1. Python Logs
Python uses `print()` for demo output. Look for `[ERROR]` or `[DEMO]` prefixes.
- Demo logs go to stdout.
- Error tracebacks go to stderr.

## 2. Rust/Backend Logs
Output to Standard Console (stdout/stderr).
- Look for `wgpu` validation errors (usually massive text blocks).
- Look for `panicked at` messages (Rust crashes).
- `cargo build` warnings appear during compilation.

## 3. Common Errors

| Error Pattern | Meaning | Fix |
|:---|:---|:---|
| `DeviceLost` | GPU driver crash or shader infinite loop | Check shader for infinite loops |
| `Buffer too small` / `Buffer is bound with size X where shader expects Y` | WGPU struct alignment mismatch | See `shader-dev` skill for alignment rules |
| `Engine not initialized` | Python called backend before `App.run()` | Ensure initialization order |
| `Entity has no X` | Missing component on entity | Use correct spawner or add component |
| `panicked at 'called unwrap() on None'` | Rust null access | Check entity existence before accessing |

## 4. Redirect Output to File
```powershell
python demos/my_demo.py 2>&1 | Tee-Object -FilePath "demo_output.txt"
```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
