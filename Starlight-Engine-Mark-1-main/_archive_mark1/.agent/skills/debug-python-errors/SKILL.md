---
name: debug-python-errors
description: Diagnosticar e corrigir erros Python em runtime
---

# Debug Python Errors

Skill para diagnosticar e resolver erros Python no Starlight Engine.

## Quando Usar
- Quando demos ou scripts Python falharem
- Quando encontrar AttributeError, TypeError, ImportError
- Quando backend Rust retornar exceções PyO3

## Passos

### 1. Rodar com Traceback Completo
```powershell
python -u demos/my_demo.py 2>&1
```

### 2. Tipos Comuns de Erro

#### ImportError / ModuleNotFoundError
```python
# Verificar sys.path
import sys
sys.path.insert(0, "pysrc")

# Verificar se backend está compilado
from starlight import backend
print(dir(backend))  # Lista funções disponíveis
```

#### AttributeError
```python
# Verificar se objeto existe antes de acessar
if hasattr(entity, 'transform'):
    x = entity.transform.x
```

#### TypeError (argumentos)
```python
# Conferir assinatura da função Rust
# backend.set_sun_color(r, g, b, intensity)  ← 4 args, não 3!
# backend.spawn_character(x, y, z, speed=5.0, max_slope=0.78, offset=0.1)
```

#### PanicException (Rust crash)
```python
# O backend retornou um panic. Causas comuns:
# - Entity ID inválido (entity já foi destruída)
# - Struct alignment errado (ver shader-dev skill)
# - Asset não carregado antes do primeiro frame
```

### 3. Debug com Print
```python
print(f"[DEBUG] Entity ID: {entity_id}")
print(f"[DEBUG] Camera: {backend.get_main_camera_id()}")
print(f"[INFO] Step completed")
```
> **NOTE**: O projeto NÃO usa `loguru`. Use `print()` para debug.

### 4. Verificar Bindings PyO3
```python
# Listar funções disponíveis no backend
from starlight import backend
funcs = [f for f in dir(backend) if not f.startswith('_')]
print(f"Available: {len(funcs)} functions")
for f in sorted(funcs): print(f"  {f}")
```

### 5. Rebuild Backend se Necessário
```powershell
taskkill /F /IM python.exe 2>$null
cargo build --release
sleep 1
Copy-Item "target\release\backend.dll" "pysrc\starlight\backend.pyd" -Force
```

## Arquivos Importantes
- `pysrc/starlight/app.py` — Classe App principal
- `pysrc/starlight/entity.py` — Wrapper de entidades
- `pysrc/starlight/framework.py` — OOP abstractions (Scene, Entity, Camera)
- `crates/engine_core/src/lib.rs` — Todas as funções PyO3
- `demos/` — Scripts de demonstração


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
