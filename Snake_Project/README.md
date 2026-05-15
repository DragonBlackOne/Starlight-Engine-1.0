# Cyber Snake — Starlight Edition

Built with **Fusion Engine (Starlight Engine SDK)**

## Features
- Classic Snake gameplay with modern neon aesthetics
- **Combo System**: Eat food quickly for multiplied points
- **HSV Rainbow Body**: Snake body changes color as you eat
- **Screen Shake**: Feedback on death and food collection
- **Color Cycling Food**: Rainbow food particles
- Hub menu with engine tech demo info

## Controls
| Key | Action |
|-----|--------|
| WASD / Arrows | Move |
| R | Restart |
| ESC | Return to Hub |
| 1 | Launch Snake from Hub |

## Build
```powershell
cmake -B build -S .
cmake --build build --config Release
.\build\Release\Snake_Project.exe
```

## SBA v2.0 Framework
Uses the Starlight Bridge API v2.0:
- Color.hsv for rainbow effects
- ScreenShake for game feel
- MathX utilities
- input API for controls
