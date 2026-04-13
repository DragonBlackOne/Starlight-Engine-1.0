---
description: Limpeza de arquivos temporários, logs e código morto
---

# Project Cleanup

Skill para limpeza geral do projeto.

## Quando Usar
- Periodicamente para manutenção
- Antes de commits importantes
- Quando espaço em disco for preocupação

## Arquivos Temporários

### 1. Build Artifacts
```bash
# Limpar target Rust (cuidado: rebuild necessário)
cargo clean

# Ou limpar apenas debug
Remove-Item -Recurse -Force target/debug
```

### 2. Logs e Outputs
```bash
# Remover logs
Remove-Item *.log -ErrorAction SilentlyContinue
Remove-Item *.txt -ErrorAction SilentlyContinue

# Remover screenshots de debug
Move-Item *.png debug_output/ -ErrorAction SilentlyContinue
```

### 3. Python Cache
```bash
# Remover __pycache__
Get-ChildItem -Path . -Recurse -Directory -Name "__pycache__" | Remove-Item -Recurse -Force

# Remover .pyc
Get-ChildItem -Path . -Recurse -Filter "*.pyc" | Remove-Item -Force
```

## Código Morto

### 1. Funções Não Utilizadas
```bash
# Buscar funções definidas mas não chamadas
# Usar grep para encontrar definições vs usos
```

### 2. Imports Não Utilizados (Rust)
```bash
cargo check --workspace 2>&1 | Select-String "unused import"
```

### 3. Variáveis Não Utilizadas
```bash
cargo check --workspace 2>&1 | Select-String "unused variable"
```

## .gitignore
Verificar se contém:
```
target/
*.log
*.tmp
__pycache__/
*.pyc
.env
*.pdb
debug_output/
```

## Checklist
- [ ] Arquivos temporários removidos
- [ ] Código morto identificado/removido
- [ ] .gitignore atualizado
- [ ] Assets órfãos verificados


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
