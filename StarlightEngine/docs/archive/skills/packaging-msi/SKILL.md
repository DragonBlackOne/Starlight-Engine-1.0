---
name: packaging-msi
description: Creating a Windows Installer.
---

# Windows Packaging (MSI)

## 1. Tool
Use **CMake's CPack** to generate the `.msi` or `.zip` distribution natively. The deprecated `cargo wix` is **no longer supported** in the Pure C11 Mark-C architecture.

## 2. Setup
Ensure `include(CPack)` is declared at the bottom of your `CMakeLists.txt`. Compile the project via MSVC:
```powershell
cmake -S . -B build
cmake --build build --config Release
```

## 3. Create the Installer
From inside the `build/` directory, invoke CPack specifying the WIX generator (or ZIP):
```powershell
cd build
cpack -G WIX -C Release
```
This generates the standalone binary installer `.msi` in the `build/` root.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
