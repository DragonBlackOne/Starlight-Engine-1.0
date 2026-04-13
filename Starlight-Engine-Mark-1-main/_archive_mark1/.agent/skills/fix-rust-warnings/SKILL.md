---
description: Corrigir warnings de compilação Rust
---

# Fix Rust Warnings

Skill para resolver warnings de compilação no Rust.

## Quando Usar
- Quando cargo check mostrar warnings
- Para manter código limpo
- Antes de releases

## Warnings Comuns

### 1. Unused Imports
```rust
// Warning: unused import: `std::sync::Arc`

// SOLUÇÃO: Remover linha
// use std::sync::Arc;  // DELETE
```

### 2. Unused Variables
```rust
// Warning: unused variable: `x`

// SOLUÇÃO 1: Prefixar com underscore
let _x = calculate();

// SOLUÇÃO 2: Remover se não necessário
```

### 3. Unused Mut
```rust
// Warning: variable does not need to be mutable

// SOLUÇÃO: Remover mut
let value = get_value(); // não let mut value
```

### 4. Dead Code
```rust
// Warning: function `old_function` is never used

// SOLUÇÃO 1: Remover função
// SOLUÇÃO 2: Adicionar #[allow(dead_code)] se intencional
#[allow(dead_code)]
fn old_function() {}
```

### 5. Unused Fields
```rust
// Warning: field `shadow_sampler` is never read

// SOLUÇÃO: Prefixar com underscore ou usar
pub struct RenderState {
    _shadow_sampler: Sampler, // underscore prefix
}
```

## Comandos

### Listar Todos Warnings
```bash
cargo check --workspace 2>&1 | Select-String "warning"
```

### Warning Específico
```bash
cargo check --workspace 2>&1 | Select-String "unused import"
```

### Clippy para Mais Sugestões
```bash
cargo clippy --workspace
```

## Automação
```bash
# Corrigir automático (quando possível)
cargo fix --workspace --allow-dirty
```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
