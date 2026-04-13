---
name: core-input-manager
description: Sistema de Input híbrido (Rust Winit -> Python)
---

# Core Input Manager

O sistema de input captura eventos de hardware via `winit` no Rust e expõe o estado atual para o Python através da classe `starlight.Input`.

## Arquitetura

1. **Backend Rust**: Intercepta `GenericWindowEvent` e atualiza um hashmap interno de teclas pressionadas e posição do mouse.
2. **Python Proxy**: `starlight.App` possui uma instância de `Input` (acessível via `self.input`).
3. **Polling vs Events**: O sistema atual é baseado em **Polling** (verificar estado a cada frame).

## API (`self.input`)

### Teclado
- `is_key_down(key_code)`: Retorna `True` se a tecla está pressionada neste frame.
- `is_key_pressed(key_code)`: Retorna `True` apenas no frame em que a tecla foi pressionada (Rising Edge).
- `is_key_released(key_code)`: Retorna `True` no frame de soltura.

### Mouse
- `get_mouse_pos()`: Retorna `(x, y)` em coordenadas de tela.
- `is_mouse_button_down(btn)`: 0 (Esq), 1 (Dir), 2 (Meio).

## Códigos de Tecla (`starlight.Keys`)
Use a classe `Keys` para constantes:
```python
from starlight import Keys

if self.input.is_key_down(Keys.W):
    player.move_forward()
```
Lista completa disponível em `pysrc/starlight/keys.py`.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
