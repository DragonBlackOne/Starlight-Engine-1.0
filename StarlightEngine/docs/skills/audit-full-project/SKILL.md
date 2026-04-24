---
description: Auditoria completa do projeto - código, assets, dependências
---

# Full Project Audit

Skill para auditoria completa do projeto Starlight.

## Quando Usar
- Antes de releases
- Após grandes refatorações
- Periodicamente para manutenção

## Checklist de Auditoria

### 1. Compilação
```bash
# Rust - todos os crates
cargo check --workspace

# Verificar warnings
cargo check --workspace 2>&1 | Select-String "warning"
```

### 2. Imports Não Utilizados
```bash
# Rust
cargo check --workspace 2>&1 | Select-String "unused import"

# Python
# Usar ferramenta como autoflake ou ruff
```

### 3. Código Duplicado
- Buscar funções com nomes similares
- Verificar helpers repetidos entre módulos

### 4. Assets Órfãos
```bash
# Listar assets não referenciados
# Verificar assets/ vs código
```

### 5. Dependências
```bash
# Rust - verificar versões
cargo outdated

# Verificar vulnerabilidades
cargo audit
```

### 6. Documentação
- README.md atualizado?
- Docstrings em funções públicas?
- SKILL.md para cada skill?

### 7. Testes
```bash
# Rust
cargo test --workspace

# Python
python -m pytest tests/
```

### 8. Estrutura de Diretórios
```
Starlight/
├── assets/          # Texturas, modelos, sons
├── crates/          # Código Rust
│   ├── engine_core/
│   ├── engine_render/
│   └── engine_audio/
├── pysrc/starlight/ # Código Python
├── demos/           # Scripts de exemplo
├── .agent/skills/   # Skills do agente
└── tests/           # Testes
```

### 9. Arquivos Temporários
- Remover `*.log`, `*.tmp`
- Limpar `target/` se necessário
- Verificar `.gitignore`

## Output
Gerar relatório com:
- [ ] Erros encontrados
- [ ] Warnings pendentes
- [ ] Recomendações de melhoria


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
