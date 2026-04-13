# Starlight Engine - Project Structure

## 📂 Root Directories

*   **`starlight_rust/`**: **The Engine Backend** (Rust).
    *   `src/engine/`: Core loops, specialized systems (ECS, Input, App).
    *   `src/graphics/`: WGPU renderer, shader management, buffers.
    *   `src/lib.rs`: PyO3 bindings (The bridge to Python).
*   **`pysrc/`**: **The Engine Frontend** (Python).
    *   `starlight/`: The importable python package.
        *   `app.py`: Main application wrapper.
        *   `math.py`: NumPy helpers.
*   **`demos/`**: Example scripts.
    *   `forest_magic.py`: Main tech demo.
    *   `easy_game.py`: API example.
*   **`assets/`**: Game resources.
    *   `textures/`: png/jpg files.
    *   `shaders/`: `.wgsl` shader code.
*   **`src/starlight_legacy/`**: **Archived** code from v4.0 and earlier.

## 📄 Key Files
*   `Cargo.toml`: Rust dependencies and build config.
*   `pyproject.toml`: Python package config (Maturin settings).
*   `task.md`: Development task tracking.
