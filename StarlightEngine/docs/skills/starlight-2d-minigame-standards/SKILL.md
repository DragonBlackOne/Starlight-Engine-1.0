---
name: starlight-2d-minigame-standards
description: Mantém a consistência visual, física e de progressão para mini-games 2D na Starlight Engine (Mark-1).
---
# SKILL: starlight-2d-minigame-standards

## Regras de Implementação

### 1. Sistema de Cores (Neon)
Sempre use os vetores de cores padronizados:
- **CYAN**: `(vec4){0, 1, 1, 1}`
- **MAGENTA**: `(vec4){1, 0, 1, 1}`
- **YELLOW**: `(vec4){1, 1, 0, 1}`
- **BG_DARK**: `(vec4){0.01, 0.01, 0.03, 1}`

### 2. Física Invariante
NUNCA mude as velocidades base sem justificativa técnica:
- `PLAYER_SPD`: `600.0f`
- `BALL_SPD`: `450.0f` (multiplicador `1.05f` em rebotes)
- `DT_CAP`: `0.05f`

### 3. Estrutura de Hub
Todos os mini-games DEVEM ser implementados como estados (`HubState`) dentro do executável `game_pong_deluxe.c`. 
- **Entrada**: Vencer uma partida de Pong.
- **Saída**: Tela de Recompensa (`ST_HUB_REWARD`) -> Menu Principal.

### 4. Progressão (+18)
Sempre use a bitmask do `save_system.h`:
- `save_system_unlock(game_id, slot_idx)`
- `game_id`: 0=Pong, 1=Invaders, 2=Breakout, 3=Runner.

## Checklist de Consistência
- [ ] O fundo usa a cor `BG_DARK`?
- [ ] O DT está limitado a `0.05f`?
- [ ] As partículas fazem fade-out linear?
- [ ] A vitória ativa o `save_system_unlock`?
