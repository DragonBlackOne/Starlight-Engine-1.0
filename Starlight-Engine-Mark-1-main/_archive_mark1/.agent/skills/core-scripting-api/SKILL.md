---
name: core-scripting-api
description: API de Alto Nível para Entidades e Componentes (Python).
---

# Core Scripting API

A Scripting API permite controlar objetos de jogo usando Python puro, abstraindo a complexidade de IDs numéricos e chamadas FFI do Rust.

## Classe `Entity`

A unidade fundamental. Encapsula um ID de entidade do ECS e componentes comuns.

### Inicialização
```python
ent = Entity(name="Orc", x=0, y=0, z=0)
```
Cria a entidade no backend imediatamente.

### Transform (Propriedade)
Acesso estilo Unity/Unreal ao componente de transformação.
```python
ent.transform.position = [10, 0, 5]
ent.transform.rotation = [0, 90, 0] # Euler (Graus)
ent.transform.scale = [1, 1, 1]
```
*Nota: A sincronização com o backend acontece automaticamente ao setar propriedades.*

### Métodos Principais
- `set_mesh(mesh_name)`: Define a geometria.
- `set_material(diffuse, normal)`: Define texturas.
- `set_color(r, g, b)`: Cor base (tint).
- `apply_impulse(x, y, z)`: Física (aplica força instantânea).
- `set_velocity(x, y, z)`: Física (define velocidade linear).

## Classe `Camera`

Uma entidade especializada para controle de visão.
- `set_position(x, y, z)`
- `look_at(target_pos)`: Orienta a câmera para um ponto.

## Extensão (Scripting)

Para criar comportamentos ("Scripts"), recomenda-se herdar de `Entity` ou criar componentes Python gerenciados pelo `App`.
**(Futuro: Sistema de Componentes Python nativo)**.

```python
class Player(Entity):
    def update(self, dt):
        if input.is_key_down(Keys.W):
            self.transform.position[2] += 5 * dt
```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
