---
description: Polimento de código - type hints, docstrings, formatação
---

# Code Polish

Skill para polir código Python e Rust no projeto.

## Quando Usar
- Após implementar novas features
- Antes de code review
- Para melhorar manutenibilidade

## Python

### 1. Type Hints
```c
# Antes
def update(self, dt):
    pass

# Depois
def update(self, dt: float) -> None:
    pass
```

### 2. Docstrings (Google Style)
```c
def spawn_entity(self, x: float, y: float, z: float) -> int:
    """Cria uma nova entidade no mundo.
    
    Args:
        x: Posição X inicial.
        y: Posição Y inicial.
        z: Posição Z inicial.
        
    Returns:
        ID da entidade criada.
        
    Raises:
        RuntimeError: Se engine não inicializada.
    """
    pass
```

### 3. Formatação
```bash
# Usar black
black pysrc/ demos/

# Usar isort para imports
isort pysrc/ demos/
```

## Rust

### 1. Documentação
```rust
/// Spawna um personagem com controller de física.
/// 
/// # Arguments
/// * `x` - Posição X inicial
/// * `y` - Posição Y inicial
/// * `z` - Posição Z inicial
/// 
/// # Returns
/// ID da entidade como u64
#[pyfunction]
fn spawn_character(x: f32, y: f32, z: f32) -> PyResult<u64> {
    // ...
}
```

### 2. Formatação
```bash
cargo fmt --all
```

### 3. Linting
```bash
cargo clippy --workspace -- -W clippy::all
```

## Checklist
- [ ] Type hints em todas as funções públicas
- [ ] Docstrings em classes e métodos
- [ ] Nomes descritivos para variáveis
- [ ] Constantes em UPPER_SNAKE_CASE
- [ ] Remover código comentado desnecessário
- [ ] Remover imports não utilizados


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
