---
name: starlight-standard
description: Enforces Starlight Engine coding standards for Mark-C (Pure C Architecture).
---

# Starlight Coding Standard (Mark-C)

When writing C code for the Starlight Engine, adhere to these strictly enforced rules.

## 1. Project Structure
```
include/          ← Public API headers (.h)
src/              ← Implementation source files (.c)
assets/           ← Game resources (models, shaders, etc.)
build/            ← Build artifacts (CMake generated)
_archive_mark1/   ← Legacy Rust/Python assets
```

## 2. Naming Conventions

| Element | Convention | Example |
|:---|:---|:---|
| Functions | `starlight_snake_case` | `starlight_engine_init()` |
| Subsystems | `subsystem_function` | `renderer_draw_mesh()` |
| Structs | `PascalCase` | `typedef struct { ... } RenderContext;` |
| Files | `snake_case.c` | `gltf_loader.c` |
| Constants | `UPPER_SNAKE` | `#define MAX_ENTITIES 1024` |

## 3. Linguistic Standard (English Only) 🇺🇸
- **Comments**: All technical explanation must be in English.
- **Strings**: Debug logs and UI text must be in English.
- **Variables**: Descriptive English names (`light_intensity` instead of `int_luz`).

## 4. Function Prefixes
- Always use a prefix to avoid symbol collisions:
  - `starlight_` for core framework functions.
  - `engine_` for basic lifecycle management.
  - `renderer_`, `physics_`, `audio_` for specific modules.

## 5. Coding Style
- **Indentation**: 4 spaces.
- **Braces**: K&R style or consistent line-breaking.
- **Documentation**: Use Javadoc-style comments for complex logic in headers.

## 6. MSVC Compatibility
- Avoid GNU-specific extensions (unless wrapped).
- Ensure all public functions are declared as `extern` if exported.
- Use standard C99/C11 types (`uint32_t`, `bool`, etc.).

---
> **Attention Agent:** This standard is the only one active for Mark-C. Ensure compliance to maintain a professional, clean, and globalized codebase.
