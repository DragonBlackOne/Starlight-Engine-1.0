"""Ability System with cooldowns, resource costs, and effects.

Usage:
    abilities = AbilitySystem()
    fireball = Ability("Fireball", cooldown=3.0, mana_cost=25,
                       on_activate=lambda ctx: print(f"FIRE at {ctx['target']}!"))
    abilities.register(fireball)
    abilities.try_activate("Fireball", mana=50, target=(10, 0, 5))  # Returns True
    abilities.try_activate("Fireball", mana=50)  # Returns False (on cooldown)
"""
from __future__ import annotations
from dataclasses import dataclass, field
from typing import Callable, Any


@dataclass
class Ability:
    """Defines an ability with cooldown and cost."""
    name: str
    cooldown: float = 1.0
    mana_cost: float = 0.0
    health_cost: float = 0.0
    cast_time: float = 0.0
    on_activate: Callable[[dict[str, Any]], None] | None = None
    on_complete: Callable[[dict[str, Any]], None] | None = None
    _cooldown_remaining: float = 0.0
    _casting_remaining: float = 0.0
    _is_casting: bool = False

    @property
    def ready(self) -> bool:
        return self._cooldown_remaining <= 0 and not self._is_casting

    @property
    def cooldown_percent(self) -> float:
        if self.cooldown <= 0:
            return 0.0
        return max(0.0, self._cooldown_remaining / self.cooldown)


class AbilitySystem:
    """Manages abilities for an entity.

    Example:
        system = AbilitySystem()
        system.register(Ability("Heal", cooldown=5.0, mana_cost=30,
                                on_activate=lambda ctx: heal(ctx['caster'], 50)))
        system.update(dt)
        system.try_activate("Heal", mana=100, caster=player)
    """

    def __init__(self) -> None:
        self._abilities: dict[str, Ability] = {}

    def register(self, ability: Ability) -> None:
        self._abilities[ability.name] = ability

    def unregister(self, name: str) -> None:
        self._abilities.pop(name, None)

    def get(self, name: str) -> Ability | None:
        return self._abilities.get(name)

    def try_activate(self, name: str, mana: float = 999, health: float = 999,
                     **context) -> bool:
        """Try to activate an ability. Returns True if successful."""
        ability = self._abilities.get(name)
        if not ability or not ability.ready:
            return False
        if mana < ability.mana_cost or health < ability.health_cost:
            return False

        if ability.cast_time > 0:
            ability._is_casting = True
            ability._casting_remaining = ability.cast_time
        else:
            if ability.on_activate:
                ability.on_activate(context)
            ability._cooldown_remaining = ability.cooldown

        return True

    def update(self, dt: float) -> list[str]:
        """Tick cooldowns and cast timers. Returns list of abilities that completed casting."""
        completed = []
        for ability in self._abilities.values():
            if ability._cooldown_remaining > 0:
                ability._cooldown_remaining = max(0, ability._cooldown_remaining - dt)

            if ability._is_casting:
                ability._casting_remaining -= dt
                if ability._casting_remaining <= 0:
                    ability._is_casting = False
                    ability._cooldown_remaining = ability.cooldown
                    if ability.on_complete:
                        ability.on_complete({})
                    elif ability.on_activate:
                        ability.on_activate({})
                    completed.append(ability.name)

        return completed

    def get_all_cooldowns(self) -> dict[str, float]:
        """Get cooldown percentages for UI display."""
        return {name: a.cooldown_percent for name, a in self._abilities.items()}

    def reset_all(self) -> None:
        for a in self._abilities.values():
            a._cooldown_remaining = 0
            a._is_casting = False

    @property
    def abilities(self) -> list[str]:
        return list(self._abilities.keys())
