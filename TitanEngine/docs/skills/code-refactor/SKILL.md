---
description: Refatorar código para melhor arquitetura
---

# Code Refactor

Skill para refatoração de código.

## Quando Usar
- Quando código estiver duplicado
- Para melhorar performance
- Para separar responsabilidades

## Padrões de Refatoração

### 1. Extract Function
```c
# Antes
def on_update(self, dt):
    # 50 linhas de código...
    
# Depois
def on_update(self, dt):
    self._handle_input(dt)
    self._update_physics(dt)
    self._render()
```

### 2. Replace Magic Numbers
```c
# Antes
if speed > 5.0:
    
# Depois
MAX_WALK_SPEED = 5.0
if speed > MAX_WALK_SPEED:
```

### 3. Simplify Conditionals
```c
# Antes
if x > 0:
    if y > 0:
        if z > 0:
            
# Depois
if x > 0 and y > 0 and z > 0:
```

### 4. Use Guard Clauses
```c
# Antes
def process(entity):
    if entity:
        if entity.active:
            # código...
            
# Depois
def process(entity):
    if not entity or not entity.active:
        return
    # código...
```

## Rust Específico

### 1. Evitar Clone Desnecessário
```rust
// Antes
let data = expensive_data.clone();

// Depois - usar referência
let data = &expensive_data;
```

### 2. Use Pattern Matching
```rust
// Antes
if result.is_some() {
    let value = result.unwrap();
}

// Depois
if let Some(value) = result {
}
```

### 3. Consolidar Helpers
- Mover funções comuns para módulo utils
- Evitar duplicação entre arquivos

## Checklist
- [ ] Funções menores que 50 linhas
- [ ] Sem código duplicado
- [ ] Responsabilidade única
- [ ] Nomes descritivos


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
