---
name: dependency-check
description: Verificar e atualizar dependências Rust e Python
---

# Dependency Check

Skill para gerenciamento de dependências.

## Quando Usar
- Periodicamente para manutenção
- Quando encontrar problemas de compatibilidade
- Antes de releases

## Rust

### 1. Listar Dependências
```powershell
cargo tree
```

### 2. Verificar Desatualizadas
```powershell
cargo install cargo-outdated
cargo outdated
```

### 3. Atualizar
```powershell
cargo update  # Atualiza Cargo.lock dentro das constraints
```

### 4. Vulnerabilidades
```powershell
cargo install cargo-audit
cargo audit
```

## Python

### 1. Listar Instaladas
```powershell
pip list
```

### 2. Verificar Desatualizadas
```powershell
pip list --outdated
```

## Dependências Principais do Projeto

### Rust (Cargo.toml)
| Crate | Purpose |
|:---|:---|
| `bevy_ecs` | Entity Component System |
| `rapier3d` | 3D Physics simulation |
| `wgpu` | WebGPU rendering |
| `pyo3` | Python bindings |
| `glam` | Vector/matrix math |
| `winit` | Window & input management |
| `bytemuck` | Safe struct casting for GPU |
| `rodio` | Audio playback |
| `image` | Texture loading |

### Python
O projeto tem **zero dependências Python externas** para runtime.
- `backend.pyd` é o módulo compilado Rust (via PyO3)
- Demos usam apenas a standard library (`sys`, `os`, `math`, `random`)
- `numpy` é opcional (não requerido)

> **NOTE**: O projeto NÃO depende de `loguru`, `pygame`, `moderngl`, ou qualquer outro pacote Python externo.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
