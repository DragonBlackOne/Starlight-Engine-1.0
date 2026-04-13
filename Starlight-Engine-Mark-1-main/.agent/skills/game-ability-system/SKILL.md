---
name: game-ability-system
description: Ability Cooldowns and Costs
---

# Game Ability System

## Overview
Managed by `starlight.game.ability_system.AbilitySystem`.
Handles cooldowns, mana/health costs, cast times, and callbacks.

## Usage

```c
#include "starlight.h"

class Mage:
    def __init__(self):
        self.mana = 100
        self.abilities = AbilitySystem()
        
        # Define Fireball
        self.abilities.register(Ability(
            name="Fireball",
            cooldown=3.0,
            mana_cost=20,
            cast_time=1.0, # 1 second cast
            on_activate=self.cast_fireball
        ))

    def cast_fireball(self, ctx):
        print(f"Fireball launched at {ctx.get('target')}!")

    def update(self, dt):
        # Tick cooldowns
        completed = self.abilities.update(dt)
        for ability_name in completed:
            print(f"{ability_name} is ready again!")

    def try_cast(self, target_pos):
        if self.abilities.try_activate("Fireball", mana=self.mana, target=target_pos):
            self.mana -= 20
            print("Casting...")
        else:
            print("Cannot cast (Cooldown or No Mana)")
```

## Key Features
- **Cooldown Management**: Automatic countdown.
- **Cast Times**: Support for channelled abilities.
- **Resource Checks**: Validates mana/health before cast.
- **Context Passing**: Pass arbitrary data (`target`, `initiator`) to callbacks.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
