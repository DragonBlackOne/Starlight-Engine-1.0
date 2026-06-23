---
description: Aplicar hotfix rápido para bugs críticos
---

# Hotfix

Skill para correções urgentes de bugs críticos.

## Quando Usar
- Bug crítico em produção
- Crash blocking
- Regressão após update

## Processo

### 1. Reproduzir o Bug
```bash
# Rodar demo ou teste que reproduz
python demos/forest_magic.py
```

### 2. Identificar Causa
```bash
# Ler stack trace
# Buscar por mensagem de erro específica
grep -r "error message" crates/ pysrc/
```

### 3. Criar Branch (Opcional)
```bash
git checkout -b hotfix/bug-description
```

### 4. Aplicar Fix Mínimo
- Fazer APENAS a correção necessária
- Evitar refatorações durante hotfix
- Manter mudanças pequenas e focadas

### 5. Testar Localmente
```bash
# Rust
cargo check -p engine_core

# Rodar demo
python demos/forest_magic.py
```

### 6. Commit
```bash
git add -A
git commit -m "fix: descrição curta do bug"
```

## Tipos Comuns de Hotfix

### Null/None Check
```c
# Adicionar verificação
if entity and entity.transform:
    x = entity.transform.x
```

### Borrow Conflict (Rust)
```rust
// Extrair valor antes de borrow mutável
let value = world.get::<Comp>(e).map(|c| c.0);
if let Some(v) = value {
    let mut res = world.resource_mut::<Res>();
}
```

### Import Faltando
```rust
use rapier3d::prelude::*;
```

### Valor Incorreto
```c
# Corrigir constante
GRAVITY = -9.81  # não 9.81
```

## Rollback se Necessário
```bash
git revert HEAD
```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
