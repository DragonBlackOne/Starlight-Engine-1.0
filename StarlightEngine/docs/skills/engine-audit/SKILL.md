---
description: Auditar a arquitetura e componentes da engine Starlight
---

# Engine Audit Skill

Esta skill define o processo para auditar o núcleo da Starlight Engine, garantindo alinhamento com práticas de alta performance em Rust GameDev.

## Checklist de Auditoria

### 1. Arquitetura Core (`engine_core`)
- [ ] **ECS**: Uso correto de `bevy_ecs` (Componentes Puros, Systems Paralelos).
    - Verificar se há lógica pesada fora de systems.
    - Verificar queries com `.iter_mut()` excessivos.
- [ ] **Integração Python (PyO3)**:
    - Verificar conversão de tipos (overhead).
    - Verificar se o `Global Interpreter Lock (GIL)` está sendo liberado em operações longas (`c::allow_threads`).
- [ ] **Physics**:
    - Verificar passo de tempo fixo (`FixedTimestep`).
    - Verificar sincronização Transform <-> RigidBody (Interpolation).

### 2. Rendering (`engine_render`)
- [ ] **WGPU Pipeline**:
    - Verificar gerenciamento de `BindGroups` (estão sendo recriados todo frame?).
    - Verificar uso de `StagingBuffers` para upload de dados.
- [ ] **Features**:
    - Suporte a PBR (Physically Based Rendering).
    - Sombras (Cascaded Shadow Maps?).
    - Post-Processing (Bloom, Tone Mapping).

### 3. Audio (`engine_audio`)
- [ ] **Backend**: Uso de `rodio` ou `kira`.
- [ ] **Gerenciamento**:
    - Pooling de sons.
    - Streaming para música vs Buffer para SFX.

## Comandos Úteis

### Verificar Dependências
```bash
cargo tree -p engine_core
```

### Buscar "Unsafe" (Potenciais riscos de memória)
```bash
rg "unsafe" crates/
```

### Contar Linhas e Complexidade (Se `tokei` disponível)
```bash
tokei crates/
```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
