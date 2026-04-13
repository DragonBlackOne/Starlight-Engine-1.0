---
name: rust-build
description: Instructions for building the Starlight Rust Core and updating Python extensions. Use when changes are made to 'crates/' or when 'backend.pyd' is missing.
---

# Rust Build & Deploy Protocol

Follow this strict protocol whenever Rust code is modified.

## 1. Kill Python First
Python locks the `.pyd` file. Always kill it before copying.
```powershell
taskkill /F /IM python.exe 2>$null
```

## 2. Build
Always use release mode. The workspace has 3 crates: `engine_core`, `engine_render`, `engine_audio`.
```powershell
cargo build --release
```
If you only changed one crate, you can target it:
```powershell
cargo build --release -p engine_core
```

## 3. Deploy the Binary
The output DLL is at `target/release/backend.dll`. Python expects it as `pysrc/starlight/backend.pyd`.

```powershell
Copy-Item "target\release\backend.dll" "pysrc\starlight\backend.pyd" -Force
```

> **CRITICAL**: The destination is `pysrc/starlight/backend.pyd`, NOT `pysrc/engine/`. The old `engine` path is deprecated.

## 4. Verify
```powershell
python -c "import sys; sys.path.insert(0,'pysrc'); from starlight import backend; print('Build OK:', dir(backend)[:5])"
```

## 5. Common Errors

| Error | Fix |
|:---|:---|
| `The process cannot access the file` | Kill Python first (step 1) |
| `cannot find -lbackend` | Check `Cargo.toml` `[lib]` section has `crate-type = ["cdylib"]` |
| `ImportError: DLL not found` | Rebuild in release mode; check MSVC toolchain is installed |
| Shader struct size mismatch (`Buffer is bound with size X where shader expects Y`) | Check `bytemuck` alignment — every `vec3` in WGSL takes 16 bytes. See `shader-dev` skill |

## 6. Full One-Liner (Copy-Paste Ready)
```powershell
taskkill /F /IM python.exe 2>$null; cargo build --release; sleep 1; Copy-Item "target\release\backend.dll" "pysrc\starlight\backend.pyd" -Force
```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
