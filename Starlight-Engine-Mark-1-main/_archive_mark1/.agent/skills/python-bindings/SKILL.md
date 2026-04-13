---
name: python-bindings
description: Instructions for exposing Rust functions to Python using PyO3. Covers the GAME_APP mutex pattern and registration.
---

# Exposing Rust to Python (PyO3)

All Python-accessible functions live in `crates/engine_core/src/lib.rs`.

## 1. Standard Pattern (GAME_APP Mutex)

Every backend function follows this pattern:

```rust
#[pyfunction]
fn my_function(entity_id: u64, value: f32) -> PyResult<()> {
    // 1. Lock the global game app
    let mut guard = GAME_APP.lock().unwrap();
    
    // 2. Check if engine is initialized
    if let Some(app) = guard.as_mut() {
        // 3. Convert u64 → Bevy Entity
        let entity = Entity::from_bits(entity_id);
        
        // 4. Access ECS world
        if let Some(mut component) = app.world.get_mut::<MyComponent>(entity) {
            component.value = value;
        } else {
            return Err(PyValueError::new_err("Entity missing component"));
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}
```

## 2. For Render State Access
```rust
#[pyfunction]
fn set_sun_color(r: f32, g: f32, b: f32, intensity: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(rs) = app.render_state.as_mut() {
            rs.set_sun_color(r, g, b, intensity);
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Render state not active"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}
```

## 3. Register the Function
At the bottom of `lib.rs`, inside `register_functions()`:

```rust
fn register_functions(m: &Bound<'_, PyModule>) -> PyResult<()> {
    // ...existing functions...
    m.add_function(wrap_pyfunction!(my_function, m)?)?;
    Ok(())
}
```

## 4. Optional Parameters
Use `#[pyo3(signature = ...)]` for defaults:

```rust
#[pyfunction]
#[pyo3(signature = (x, y, z, r=None, g=None, b=None))]
fn spawn_entity(x: f32, y: f32, z: f32, r: Option<f32>, g: Option<f32>, b: Option<f32>) -> PyResult<u64> {
    // ...
}
```

## 5. Return Types

| Python Type | Rust Type |
|:---|:---|
| `int` | `u64`, `u32`, `i32` |
| `float` | `f32`, `f64` |
| `str` | `String` |
| `bool` | `bool` |
| `tuple` | `(f32, f32, f32)` |
| `list[int]` | `Vec<u64>` |
| `None` | `()` (unit type) |

## 6. After Any Change
Run the `rust-build` skill:
```powershell
taskkill /F /IM python.exe 2>$null; cargo build --release; sleep 1; Copy-Item "target\release\backend.dll" "pysrc\starlight\backend.pyd" -Force
```

## 7. Common Errors

| Error | Cause | Fix |
|:---|:---|:---|
| `TypeError: argument 'x': 'int' not supported` | Wrong arg type in Python | Check function signature matches call site |
| `PanicException: called unwrap on a None` | Entity doesn't exist in ECS | Validate `entity_id` before use, use `get::<T>()` not `get_mut` |
| `Engine not initialized` | Function called before `init_engine()` | Ensure demo calls `App().run()` first |


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
