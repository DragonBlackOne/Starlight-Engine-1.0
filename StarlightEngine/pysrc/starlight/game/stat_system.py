"""RPG Stat System with modifiers, buffs, and computed values.

Usage:
    stats = StatSystem()
    stats.add_stat("health", base=100, min_val=0, max_val=200)
    stats.add_stat("attack", base=15)
    stats.add_modifier("attack", Modifier("sword_bonus", 10, ModType.FLAT))
    stats.add_modifier("attack", Modifier("rage_buff", 0.2, ModType.PERCENT))
    print(stats.get("attack"))  # 15 + 10 = 25, then * 1.2 = 30
"""
from __future__ import annotations
from enum import Enum
from dataclasses import dataclass, field
from typing import Any


class ModType(Enum):
    """Modifier application order: FLAT first, then PERCENT."""
    FLAT = "flat"
    PERCENT = "percent"


@dataclass
class Modifier:
    """A named modifier that changes a stat value."""
    name: str
    value: float
    mod_type: ModType = ModType.FLAT
    duration: float = -1.0  # -1 = permanent
    elapsed: float = 0.0

    @property
    def expired(self) -> bool:
        return self.duration > 0 and self.elapsed >= self.duration


@dataclass
class Stat:
    """A single stat with base value, bounds, and modifiers."""
    name: str
    base: float
    min_val: float = 0.0
    max_val: float = 999999.0
    modifiers: list[Modifier] = field(default_factory=list)

    @property
    def value(self) -> float:
        """Compute final value: base + flat mods, then * percent mods."""
        flat = sum(m.value for m in self.modifiers if m.mod_type == ModType.FLAT and not m.expired)
        pct = sum(m.value for m in self.modifiers if m.mod_type == ModType.PERCENT and not m.expired)
        result = (self.base + flat) * (1.0 + pct)
        return max(self.min_val, min(self.max_val, result))

    def add_modifier(self, mod: Modifier) -> None:
        self.modifiers.append(mod)

    def remove_modifier(self, name: str) -> bool:
        before = len(self.modifiers)
        self.modifiers = [m for m in self.modifiers if m.name != name]
        return len(self.modifiers) < before

    def clear_expired(self) -> int:
        before = len(self.modifiers)
        self.modifiers = [m for m in self.modifiers if not m.expired]
        return before - len(self.modifiers)


class StatSystem:
    """Manages all stats for an entity or player.

    Example:
        stats = StatSystem()
        stats.add_stat("health", base=100, max_val=200)
        stats.add_stat("mana", base=50)
        stats.add_modifier("health", Modifier("armor", 25, ModType.FLAT))
        print(stats.get("health"))  # 125
    """

    def __init__(self) -> None:
        self._stats: dict[str, Stat] = {}

    def add_stat(self, name: str, base: float = 0.0,
                 min_val: float = 0.0, max_val: float = 999999.0) -> Stat:
        stat = Stat(name=name, base=base, min_val=min_val, max_val=max_val)
        self._stats[name] = stat
        return stat

    def get(self, name: str) -> float:
        """Get computed value of a stat."""
        if name not in self._stats:
            raise KeyError(f"Stat '{name}' not found")
        return self._stats[name].value

    def get_base(self, name: str) -> float:
        return self._stats[name].base

    def set_base(self, name: str, value: float) -> None:
        self._stats[name].base = value

    def add_modifier(self, stat_name: str, mod: Modifier) -> None:
        if stat_name not in self._stats:
            raise KeyError(f"Stat '{stat_name}' not found")
        self._stats[stat_name].add_modifier(mod)

    def remove_modifier(self, stat_name: str, mod_name: str) -> bool:
        if stat_name not in self._stats:
            return False
        return self._stats[stat_name].remove_modifier(mod_name)

    def update(self, dt: float) -> None:
        """Tick all timed modifiers and remove expired ones."""
        for stat in self._stats.values():
            for mod in stat.modifiers:
                if mod.duration > 0:
                    mod.elapsed += dt
            stat.clear_expired()

    def to_dict(self) -> dict[str, Any]:
        """Serialize for save system."""
        return {
            name: {"base": s.base, "min": s.min_val, "max": s.max_val}
            for name, s in self._stats.items()
        }

    def from_dict(self, data: dict[str, Any]) -> None:
        """Restore from save data."""
        for name, vals in data.items():
            self.add_stat(name, vals["base"], vals.get("min", 0), vals.get("max", 999999))

    def __repr__(self) -> str:
        lines = [f"StatSystem({len(self._stats)} stats):"]
        for name, stat in self._stats.items():
            lines.append(f"  {name}: {stat.value:.1f} (base={stat.base}, mods={len(stat.modifiers)})")
        return "\n".join(lines)
