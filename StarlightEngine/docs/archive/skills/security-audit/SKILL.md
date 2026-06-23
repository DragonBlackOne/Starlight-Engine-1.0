---
description: Auditoria de segurança - dependências, inputs, secrets
---

# Security Audit

Skill para auditoria de segurança do projeto.

## Quando Usar
- Antes de releases públicos
- Após adicionar dependências
- Periodicamente

## Dependências

### 1. Rust - Vulnerabilidades Conhecidas
```bash
# Instalar cargo-audit
cargo install cargo-audit

# Rodar auditoria
cargo audit
```

### 2. Rust - Dependências Desatualizadas
```bash
# Instalar cargo-outdated
cargo install cargo-outdated

# Verificar versões
cargo outdated
```

### 3. Python
```bash
# Usar pip-audit
pip install pip-audit
pip-audit
```

## Código

### 1. Secrets Hardcoded
```bash
# Buscar por padrões suspeitos
Select-String -Path "**/*.rs","**/*.py" -Pattern "password|secret|api_key|token" -Recurse
```

### 2. Inputs Não Validados
- Verificar inputs de usuário em PyO3 bindings
- Validar paths antes de file I/O
- Sanitizar strings de entrada

### 3. Unsafe Rust
```bash
# Listar blocos unsafe
Select-String -Path "crates/**/*.rs" -Pattern "unsafe" -Recurse
```

## Network

### 1. Conexões Externas
- Verificar URLs hardcoded
- Validar certificados SSL
- Limitar endpoints permitidos

### 2. Serialização
- Evitar deserialização de dados não confiáveis
- Usar formatos seguros (JSON, MessagePack)

## Checklist
- [ ] cargo audit sem vulnerabilidades críticas
- [ ] Sem secrets no código fonte
- [ ] Inputs validados
- [ ] Blocos unsafe revisados
- [ ] Dependências atualizadas


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
