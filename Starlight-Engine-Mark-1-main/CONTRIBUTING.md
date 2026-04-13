# Contributing to Starlight Engine

Thank you for your interest in contributing! This document provides guidelines for developing on the Starlight Engine v5.0+ (Rust Edition).

## 🛠️ Development Setup

The engine is a hybrid **Rust/Python** project. Core logic lives in Rust, while gameplay API lives in Python.

1.  **Environment Setup:**
    ```bash
    git clone ...
    python -m venv venv
    # Activate venv (see README)
    pip install maturin
    ```

2.  **Building the Engine:**
    You must compile the Rust backend to use it in Python.
    ```bash
    # Compiles Rust and installs 'starlight_rust' into your venv
    maturin develop --release
    ```
    *Note: Use `--release` for performance. Debug builds are significantly slower.*

3.  **Rust Development:**
    If you are modifying `starlight_rust/`:
    ```bash
    cd starlight_rust
    cargo check       # Fast syntax check
    cargo clippy      # Linter (highly recommended)
    cargo build       # Compile without installing
    ```

## 🧪 Running Tests

### Rust Unit Tests
Run core engine tests (ECS, Physics, Math):
```bash
cd starlight_rust
cargo test
```

### Python Integration Tests
Run scripts in `demos/` or `tests/` manually:
```bash
python demos/simple_cube.py
```

## rules Coding Standards

*   **Rust:** Follow standard Rust style (`rustfmt`).
    *   Run `cargo clippy` before committing.
    *   Avoid `unsafe` unless absolutely necessary (e.g., FFI).
*   **Python:** Follow PEP 8.
*   **Commits:** Use [Conventional Commits](https://www.conventionalcommits.org/) (e.g., `feat: add shadow maps`, `fix: resolve AABB collision`).

## 📁 Project Structure

*   `starlight_rust/`: The Engine (Renderer, ECS, Physics). functionality goes here.
*   `pysrc/`: The Python API (wrappers). User-facing API changes go here.
*   `demos/`: Verification scripts.