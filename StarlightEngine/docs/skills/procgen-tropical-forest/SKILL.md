---
description: Gerar floresta tropical procedural com 5 camadas
---

# Procedural Tropical Forest

Skill para gerar floresta tropical realista com vegetação procedural.

## Quando Usar
- Criar cenas de floresta tropical
- Demonstrar geração procedural
- Testar performance com muitas entidades

## Componentes

### 1. Vegetação (5 Camadas)
```c
#include "starlight.h"

spawner = VegetationSpawner(app, seed=42)
spawner.spawn_forest(
    area_size=150.0,
    clearing_radius=10.0,
    layers=[
        FloraLayer.EMERGENT,   # 45-65m
        FloraLayer.CANOPY,     # 25-40m
        FloraLayer.UNDERSTORY, # 10-20m
        FloraLayer.SHRUB,      # 1-5m
        # FloraLayer.FLOOR,    # 0-1m (muitas entidades)
    ]
)
```

### 2. Fauna Animada
```c
#include "starlight.h"

fauna = FaunaSystem(app, area_size=150.0)
fauna.initialize()

# No update loop:
fauna.set_time_of_day(0.5)  # 0-1
fauna.update(dt)
```

### 3. Atmosfera
- Neblina verde-escuro: `backend.set_fog(0.025, 0.05, 0.08, 0.05)`
- Sol filtrado: `backend.set_sun_direction(0.3, -0.7, -0.4)`
- Céu: `backend.set_skybox_color(0.05, 0.1, 0.05)`

## Tipos de Flora
| Nome | Camada | Altura | Densidade |
|------|--------|--------|-----------|
| kapok_tree | Emergente | 45-65m | 0.5/100m² |
| canopy_tree | Dossel | 25-40m | 3.0/100m² |
| palm_tree | Dossel | 20-30m | 2.0/100m² |
| fern_large | Arbustos | 1.5-4m | 15/100m² |
| heliconia | Arbustos | 1-3m | 8/100m² |

## Tipos de Fauna
| Tipo | Altura | Velocidade | Ativo |
|------|--------|------------|-------|
| Pássaros | 15-45m | 5-12 m/s | Dia |
| Borboletas | 0.5-10m | 1-3 m/s | Dia |
| Vagalumes | 1-8m | 0.5-2 m/s | Noite |
| Libélulas | 0.5-5m | 3-8 m/s | Dia |

## Demo
```bash
python demos/forest_magic.py
```

Controles:
- **WASD**: Movimento
- **Mouse**: Olhar
- **F3**: Debug info
- **P**: Pausar
- **F**: Acelerar tempo
- **ESC**: Sair


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
