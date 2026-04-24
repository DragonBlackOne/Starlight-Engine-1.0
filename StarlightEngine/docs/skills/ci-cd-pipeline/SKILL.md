---
name: ci-cd-pipeline
description: Continuous Integration.
---

# CI/CD Pipeline

## GitHub Actions Workflow

Create `.github/workflows/build.yml`:

```yaml
name: Starlight Build
on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  build:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v4

      - name: Install Rust
        uses: dtolnay/rust-toolchain@stable

      - name: Cache Cargo
        uses: actions/cache@v4
        with:
          path: |
            ~/.cargo/registry
            ~/.cargo/git
            target
          key: ${{ runner.os }}-cargo-${{ hashFiles('**/Cargo.lock') }}

      - name: Build
        run: cargo build --release

      - name: Test
        run: cargo test --workspace

      - name: Upload Artifact
        uses: actions/upload-artifact@v4
        with:
          name: backend-dll
          path: target/release/backend.dll
```

## Local Verification (Before Push)
```powershell
cargo check --workspace     # Fast syntax check
cargo test --workspace      # Run tests
cargo build --release       # Full build
```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
