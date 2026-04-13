---
description: Diagnosticar e corrigir erros de compilação Rust (E0XXX)
---

# Debug Rust Errors

Skill para diagnosticar e resolver erros de compilação Rust no projeto Starlight.

## Quando Usar
- Quando `cargo check` ou `cargo build` falhar
- Quando encontrar erros E0XXX do rustc
- Quando precisar entender mensagens de erro do borrow checker

## Passos

### 1. Capturar Erro Completo
```bash
cargo check -p <crate_name> 2>&1 | Select-String "error" -Context 5,10
```

### 2. Identificar Tipo de Erro
- **E0502**: Conflito de borrow (mutable vs immutable)
- **E0433**: Tipo/módulo não encontrado (import faltando)
- **E0599**: Método não existe para o tipo
- **E0382**: Valor já movido (ownership)
- **E0308**: Tipos incompatíveis

### 3. Soluções Comuns

#### E0502 - Borrow Conflict
```rust
// ERRADO: borrow konflito
if let Some(comp) = world.get::<Component>(entity) {
    let mut resource = world.resource_mut::<Resource>(); // ERROR!
    resource.update(comp.0);
}

// CORRETO: extrair valor primeiro
let value = world.get::<Component>(entity).map(|c| c.0);
if let Some(v) = value {
    let mut resource = world.resource_mut::<Resource>();
    resource.update(v);
}
```

#### E0433 - Import Faltando
```rust
// Adicionar import necessário
use rapier3d::prelude::*; // Glob import
use specific::module::Type; // Import específico
```

#### E0599 - Método não encontrado
- Verificar se trait está importado
- Verificar se tipo está correto
- Usar `rustc --explain E0599` para detalhes

### 4. Verificar Correção
```bash
cargo check -p <crate_name>
```

### 5. Rodar Testes
```bash
cargo test -p <crate_name>
```

## Crates do Projeto
- `engine_core` - Core ECS, física, PyO3 bindings
- `engine_render` - WGPU rendering pipeline
- `engine_audio` - Audio com rodio


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
