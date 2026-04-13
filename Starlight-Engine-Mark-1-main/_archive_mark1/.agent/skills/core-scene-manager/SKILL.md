---
name: core-scene-manager
description: Gerenciamento de Cenas e Grafo de Objetos.
---

# Core Scene Manager

A classe `starlight.Scene` gerencia o grafo de objetos e configurações globais de renderização. Ela atua como uma fachada para o ECS do backend.

## Funcionalidades

### Gestão de Entidades
- `add(entity)`: Registra uma entidade Python no ECS do backend. Chama `entity.update_transform()` inicial.
- `remove(entity)`: (Planejado) Remove entidade.

### Configuração de Ambiente (Lighting)
A cena controla a iluminação global que afeta todos os objetos PBR.
- `set_sun(direction, color, intensity)`: Configura a luz direcional principal (Sol) e sombras.
- `set_ambient(intensity)`: Luz ambiente base (hemisférica).
- `set_fog(density, color)`: Neblina volumétrica simplificada.
- `set_skybox(folder_path)`: (Via backend direto por enquanto).

### Post-Processing
Controle de efeitos de câmera globais.
- `set_post_process(exposure, gamma, bloom_intensity, bloom_threshold)`: Ajusta o pipeline HDR -> LDR.

## Exemplo

```python
scene = Scene()
scene.set_sun(direction=(0.5, -0.5, 0.5), color=(1.0, 1.0, 0.9), intensity=2.0)
scene.set_post_process(exposure=1.2, bloom_intensity=0.8)

player = Entity("Player")
scene.add(player)
```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
