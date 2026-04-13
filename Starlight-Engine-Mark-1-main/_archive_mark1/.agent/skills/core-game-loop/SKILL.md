---
name: core-game-loop
description: Arquitetura do Loop de Jogo no Starlight Engine (Rust + Python).
---

# Core Game Loop

O Loop de Jogo do Starlight Engine é híbrido, gerenciado pelo backend em Rust (`winit` event loop) com hooks para Python via PyO3.

## Ciclo de Vida (`starlight.App`)

A classe `starlight.App` é o ponto de entrada. O backend Rust chama seus métodos:

1. **Initialization**: `__init__` (Python) -> `backend.run_engine` (Rust).
2. **Start**: `on_start()` chamado uma vez após a janela abrir.
3. **Loop (Frame-a-Frame)**:
   - **Input Polling**: Rust coleta eventos de janela/teclado.
   - **Update**: `on_update(dt)` (Lógica de jogo).
   - **Late Update**: `on_late_update(dt)` (Câmera, UI tardia).
   - **Physics Fixed Update**: `on_fixed_update(dt)` (Chamado em passos fixos via Rapier no Rust).
   - **Render**: `on_render()` (Opcional, custom draw calls).
   - **Present**: Rust WGPU renderiza a cena e faz swap buffer.

## Exemplo de Implementação

```python
from starlight import App

class MyGame(App):
    def on_start(self):
        print("Game Started")

    def on_update(self, dt):
        # Lógica frame-dependent
        pass

    def on_late_update(self, dt):
        # Mover câmera para seguir jogador
        pass

    def on_fixed_update(self, dt):
        # Física
        pass
```

## Controle de Execução
- `self.exit()`: Encerra o loop.
- `dt`: Delta time em segundos (float).


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
