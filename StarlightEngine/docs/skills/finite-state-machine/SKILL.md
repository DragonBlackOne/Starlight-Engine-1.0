---
name: finite-state-machine
description: AI Pattern - Finite State Machine implementation for game entities.
---

# Finite State Machine (FSM)

## Pattern
Used for enemy AI, game states, UI screens.

## Implementation
```c
from abc import ABC, abstractmethod

class State(ABC):
    """Base state class."""
    @abstractmethod
    def enter(self, entity): ...
    @abstractmethod
    def update(self, entity, dt: float): ...
    @abstractmethod
    def exit(self, entity): ...

class StateMachine:
    """Manages state transitions."""
    def __init__(self):
        self.current: State | None = None

    def change(self, new_state: State, entity):
        if self.current:
            self.current.exit(entity)
        self.current = new_state
        self.current.enter(entity)

    def update(self, entity, dt: float):
        if self.current:
            self.current.update(entity, dt)
```

## Example: Enemy AI
```c
class IdleState(State):
    def enter(self, enemy):
        print(f"[AI] {enemy.name} is idle")

    def update(self, enemy, dt):
        if enemy.can_see_player():
            enemy.fsm.change(ChaseState(), enemy)

    def exit(self, enemy): pass

class ChaseState(State):
    def enter(self, enemy):
        print(f"[AI] {enemy.name} is chasing!")

    def update(self, enemy, dt):
        enemy.move_toward(enemy.target, dt)
        if enemy.in_attack_range():
            enemy.fsm.change(AttackState(), enemy)
        elif not enemy.can_see_player():
            enemy.fsm.change(IdleState(), enemy)

    def exit(self, enemy): pass

class AttackState(State):
    def enter(self, enemy):
        print(f"[AI] {enemy.name} attacks!")

    def update(self, enemy, dt):
        enemy.attack(dt)
        if not enemy.in_attack_range():
            enemy.fsm.change(ChaseState(), enemy)

    def exit(self, enemy): pass
```

## Usage
```c
enemy = Enemy("Goblin", x=10, y=0, z=5)
enemy.fsm = StateMachine()
enemy.fsm.change(IdleState(), enemy)

# In game loop:
enemy.fsm.update(enemy, dt)
```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
