# Quick Start Guide: Starlight Mark-C 🚀

Follow these steps to get your first Starlight application running.

## 1. Prerequisites
- **C Compiler**: MSVC (Windows), GCC (Linux), or Clang (macOS).
- **CMake**: Version 3.10 or higher.
- **Git**: To clone the repository and fetch dependencies.

## 2. Building the Project
Starlight uses **CMake** to automatically fetch dependencies like SDL2 and Flecs:

```powershell
# 1. Create a build directory
mkdir build -Force
cd build

# 2. Configure and Build (Release recommended)
cmake ..
cmake --build . --config Release
```

## 3. Running the Demos
Binaries are located in `build/Release/` (Windows) or `build/` (Linux).
Try the PBR showcase or the Retro Pong Hub:

```powershell
./starlight_demo_pbr
./starlight_pong_deluxe
```

## 4. Your First "Hello Starlight"
Creating a game is simple using the **SLF Framework**. Create a `main.c`:

```c
#include "starlight.h"

void OnUpdate(SLF_App* app) {
    // Check for "esc" action (bound below)
    if (starlight_is_action_just_pressed(app, "esc")) {
        app->running = false;
    }
}

void OnUI(SLF_App* app) {
    vec4 color = {0, 1, 0, 1}; // Green
    starlight_ui_draw_text(app, NULL, "HELLO STARLIGHT!", 100, 100, color);
}

int main() {
    SLF_Config config = {
        .on_update = OnUpdate,
        .on_ui = OnUI
    };
    
    // Initialize with 1280x720 window
    starlight_framework_init("My First Game", 1280, 720, &config);
    return 0;
}
```

## 5. Next Steps
- **[Framework Guide](framework_guide.md)**: Learn about Scene Stacks and Asset Pools.
- **[Retro Genesis](retro_development.md)**: Start building 16-bit Mode 7 games.
- **[API Reference](api_reference.md)**: Search the full list of engine functions.

---
**🛡️ Welcome to the world of Pure C gamedev.**

