# Tetris Project

Sample 2D Tetris game built with the **Fusion Engine (StarlightEngine SDK)**.

## SBA v2.0 Framework Included

- `Entity` OO wrapper
- `Scene` state machine
- `Tween` system (8 easing functions)
- `Events` pub/sub bus
- `Coroutine` runner
- `Color`, `MathX`, `Physics2D`, `Timer`, `ScreenShake`

## Build

```powershell
# 1. Build the engine (one-time per config)
cmake -B StarlightEngine/build -S StarlightEngine
cmake --build StarlightEngine/build --config Release --parallel

# 2. Build this project
cmake -B build -S .
cmake --build build --config Release --parallel
.\build\Release\Tetris_Project.exe
```

## Controls

- Arrow keys / WASD — move piece
- Up arrow / W — rotate
- Space — hard drop
- Down arrow / S — soft drop
- P / Esc — pause
- R — restart

## Notes

- This project links the prebuilt `StarlightCore.lib` (do **not** use
  `add_subdirectory(StarlightEngine)`).
- The Lua scripts in `assets/scripts/` are synced from the engine copy by
  `build_all.ps1` at the root of the repository.
