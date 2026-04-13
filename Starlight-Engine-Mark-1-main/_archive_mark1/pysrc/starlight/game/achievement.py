"""Achievement Tracking System.

Usage:
    achievements = AchievementSystem()
    achievements.register(Achievement("first_kill", "First Blood", "Defeat your first enemy"))
    achievements.register(Achievement("explorer", "Explorer", "Visit 10 locations", target=10))
    achievements.unlock("first_kill")
    achievements.progress("explorer", 1)  # +1 location
"""
from __future__ import annotations
from dataclasses import dataclass, field
from typing import Any, Callable


@dataclass
class Achievement:
    """An achievement definition."""
    ach_id: str
    name: str
    description: str = ""
    target: int = 1
    progress: int = 0
    unlocked: bool = False
    hidden: bool = False
    icon: str = ""
    reward_xp: int = 0
    on_unlock: Callable[["Achievement"], None] | None = None

    @property
    def percent(self) -> float:
        return min(100.0, (self.progress / max(1, self.target)) * 100.0)


class AchievementSystem:
    """Achievement manager with progress tracking.

    Example:
        achs = AchievementSystem()
        achs.register(Achievement("collector", "Collector", "Collect 50 items", target=50))
        achs.progress("collector", 5)  # 5/50
    """

    def __init__(self) -> None:
        self._achievements: dict[str, Achievement] = {}
        self._on_unlock: list[Callable[[Achievement], None]] = []

    def register(self, ach: Achievement) -> None:
        self._achievements[ach.ach_id] = ach

    def unlock(self, ach_id: str) -> bool:
        """Immediately unlock an achievement."""
        ach = self._achievements.get(ach_id)
        if not ach or ach.unlocked:
            return False
        ach.progress = ach.target
        ach.unlocked = True
        self._fire_unlock(ach)
        return True

    def progress(self, ach_id: str, amount: int = 1) -> bool:
        """Add progress toward an achievement. Returns True if just unlocked."""
        ach = self._achievements.get(ach_id)
        if not ach or ach.unlocked:
            return False
        ach.progress = min(ach.progress + amount, ach.target)
        if ach.progress >= ach.target:
            ach.unlocked = True
            self._fire_unlock(ach)
            return True
        return False

    def is_unlocked(self, ach_id: str) -> bool:
        ach = self._achievements.get(ach_id)
        return ach.unlocked if ach else False

    def get_all(self) -> list[Achievement]:
        return list(self._achievements.values())

    def get_unlocked(self) -> list[Achievement]:
        return [a for a in self._achievements.values() if a.unlocked]

    def get_locked(self) -> list[Achievement]:
        return [a for a in self._achievements.values() if not a.unlocked]

    @property
    def total_count(self) -> int:
        return len(self._achievements)

    @property
    def unlocked_count(self) -> int:
        return sum(1 for a in self._achievements.values() if a.unlocked)

    @property
    def completion_percent(self) -> float:
        if not self._achievements:
            return 0.0
        return (self.unlocked_count / self.total_count) * 100.0

    def on_unlock(self, callback: Callable[[Achievement], None]) -> None:
        self._on_unlock.append(callback)

    def _fire_unlock(self, ach: Achievement) -> None:
        print(f"[Achievement] Unlocked: {ach.name}!")
        if ach.on_unlock:
            ach.on_unlock(ach)
        for cb in self._on_unlock:
            cb(ach)

    def to_dict(self) -> dict[str, Any]:
        return {
            ach_id: {"progress": a.progress, "unlocked": a.unlocked}
            for ach_id, a in self._achievements.items()
        }

    def from_dict(self, data: dict[str, Any]) -> None:
        for ach_id, vals in data.items():
            if ach_id in self._achievements:
                self._achievements[ach_id].progress = vals.get("progress", 0)
                self._achievements[ach_id].unlocked = vals.get("unlocked", False)
