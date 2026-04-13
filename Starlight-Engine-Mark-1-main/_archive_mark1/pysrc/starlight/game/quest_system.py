"""Quest and Objective Tracking System.

Usage:
    qs = QuestSystem()
    quest = Quest("Dragon Slayer", "Defeat the ancient dragon")
    quest.add_objective(Objective("find_cave", "Find the Dragon's Cave"))
    quest.add_objective(Objective("kill_dragon", "Slay the Dragon", required_count=1))
    quest.add_objective(Objective("return", "Return to the King"))
    qs.add_quest(quest)
    qs.start_quest("Dragon Slayer")
    qs.progress_objective("Dragon Slayer", "find_cave")  # Complete find_cave
"""
from __future__ import annotations
from dataclasses import dataclass, field
from enum import Enum
from typing import Any, Callable


class QuestState(Enum):
    LOCKED = "locked"
    AVAILABLE = "available"
    ACTIVE = "active"
    COMPLETED = "completed"
    FAILED = "failed"


@dataclass
class Objective:
    """A single quest objective."""
    obj_id: str
    description: str
    required_count: int = 1
    current_count: int = 0
    optional: bool = False

    @property
    def completed(self) -> bool:
        return self.current_count >= self.required_count

    def progress(self, amount: int = 1) -> bool:
        """Increment progress. Returns True if just completed."""
        was_done = self.completed
        self.current_count = min(self.current_count + amount, self.required_count)
        return not was_done and self.completed


@dataclass
class Quest:
    """A quest containing multiple objectives."""
    name: str
    description: str = ""
    state: QuestState = QuestState.AVAILABLE
    objectives: list[Objective] = field(default_factory=list)
    reward_xp: int = 0
    reward_items: list[str] = field(default_factory=list)
    on_complete: Callable[["Quest"], None] | None = None
    on_fail: Callable[["Quest"], None] | None = None
    prerequisites: list[str] = field(default_factory=list)

    def add_objective(self, obj: Objective) -> None:
        self.objectives.append(obj)

    @property
    def all_required_complete(self) -> bool:
        return all(o.completed for o in self.objectives if not o.optional)

    @property
    def progress_percent(self) -> float:
        required = [o for o in self.objectives if not o.optional]
        if not required:
            return 100.0
        done = sum(1 for o in required if o.completed)
        return (done / len(required)) * 100.0


class QuestSystem:
    """Manages all quests.

    Example:
        qs = QuestSystem()
        qs.add_quest(Quest("Fetch Water", "Bring 3 buckets of water"))
        qs.start_quest("Fetch Water")
    """

    def __init__(self) -> None:
        self._quests: dict[str, Quest] = {}
        self._on_complete: list[Callable[[Quest], None]] = []

    def add_quest(self, quest: Quest) -> None:
        self._quests[quest.name] = quest

    def start_quest(self, name: str) -> bool:
        quest = self._quests.get(name)
        if not quest or quest.state != QuestState.AVAILABLE:
            return False
        # Check prerequisites
        for prereq in quest.prerequisites:
            pq = self._quests.get(prereq)
            if not pq or pq.state != QuestState.COMPLETED:
                return False
        quest.state = QuestState.ACTIVE
        print(f"[Quest] Started: {name}")
        return True

    def progress_objective(self, quest_name: str, objective_id: str, amount: int = 1) -> bool:
        quest = self._quests.get(quest_name)
        if not quest or quest.state != QuestState.ACTIVE:
            return False
        for obj in quest.objectives:
            if obj.obj_id == objective_id:
                just_done = obj.progress(amount)
                if just_done:
                    print(f"[Quest] Objective complete: {obj.description}")
                if quest.all_required_complete:
                    self._complete_quest(quest)
                return True
        return False

    def fail_quest(self, name: str) -> None:
        quest = self._quests.get(name)
        if quest and quest.state == QuestState.ACTIVE:
            quest.state = QuestState.FAILED
            if quest.on_fail:
                quest.on_fail(quest)
            print(f"[Quest] Failed: {name}")

    def get_active(self) -> list[Quest]:
        return [q for q in self._quests.values() if q.state == QuestState.ACTIVE]

    def get_completed(self) -> list[Quest]:
        return [q for q in self._quests.values() if q.state == QuestState.COMPLETED]

    def on_quest_complete(self, callback: Callable[[Quest], None]) -> None:
        self._on_complete.append(callback)

    def _complete_quest(self, quest: Quest) -> None:
        quest.state = QuestState.COMPLETED
        print(f"[Quest] Completed: {quest.name} (+{quest.reward_xp} XP)")
        if quest.on_complete:
            quest.on_complete(quest)
        for cb in self._on_complete:
            cb(quest)

    def to_dict(self) -> dict[str, Any]:
        return {
            name: {
                "state": q.state.value,
                "objectives": [{"id": o.obj_id, "count": o.current_count} for o in q.objectives]
            }
            for name, q in self._quests.items()
        }
