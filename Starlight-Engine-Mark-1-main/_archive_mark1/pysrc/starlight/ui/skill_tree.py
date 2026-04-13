"""Skill Tree Visualization data model.

Usage:
    tree = SkillTree()
    tree.add_node(SkillNode("fireball", "Fireball", tier=0, cost=1))
    tree.add_node(SkillNode("fire_mastery", "Fire Mastery", tier=1, cost=2,
                            prerequisites=["fireball"]))
    tree.add_node(SkillNode("inferno", "Inferno", tier=2, cost=3,
                            prerequisites=["fire_mastery"]))
    tree.unlock("fireball", skill_points=5)  # Returns True, costs 1 point
    tree.unlock("fire_mastery", skill_points=4)  # Returns True, costs 2
"""
from __future__ import annotations
from dataclasses import dataclass, field
from typing import Any, Callable


@dataclass
class SkillNode:
    """A node in the skill tree."""
    skill_id: str
    name: str
    description: str = ""
    tier: int = 0
    cost: int = 1
    max_rank: int = 1
    current_rank: int = 0
    prerequisites: list[str] = field(default_factory=list)
    icon: str = ""
    on_unlock: Callable[["SkillNode"], None] | None = None
    # Visual layout
    grid_x: int = 0
    grid_y: int = 0

    @property
    def unlocked(self) -> bool:
        return self.current_rank > 0

    @property
    def maxed(self) -> bool:
        return self.current_rank >= self.max_rank

    @property
    def rank_text(self) -> str:
        return f"{self.current_rank}/{self.max_rank}"


class SkillTree:
    """Manages a skill tree with prerequisites and unlock logic.

    Example:
        tree = SkillTree()
        tree.add_node(SkillNode("block", "Shield Block", tier=0, cost=1, grid_x=0, grid_y=0))
        tree.add_node(SkillNode("bash", "Shield Bash", tier=1, cost=2,
                                prerequisites=["block"], grid_x=0, grid_y=1))
        tree.unlock("block", skill_points=3)
    """

    def __init__(self) -> None:
        self._nodes: dict[str, SkillNode] = {}
        self._on_unlock: list[Callable[[SkillNode], None]] = []

    def add_node(self, node: SkillNode) -> None:
        self._nodes[node.skill_id] = node

    def get_node(self, skill_id: str) -> SkillNode | None:
        return self._nodes.get(skill_id)

    def can_unlock(self, skill_id: str, skill_points: int = 0) -> bool:
        """Check if a skill can be unlocked."""
        node = self._nodes.get(skill_id)
        if not node or node.maxed:
            return False
        if skill_points < node.cost:
            return False
        # Check prerequisites
        for prereq in node.prerequisites:
            pn = self._nodes.get(prereq)
            if not pn or not pn.unlocked:
                return False
        return True

    def unlock(self, skill_id: str, skill_points: int = 0) -> int:
        """Try to unlock/upgrade a skill. Returns points consumed (0 if failed)."""
        if not self.can_unlock(skill_id, skill_points):
            return 0
        node = self._nodes[skill_id]
        node.current_rank += 1
        print(f"[SkillTree] Unlocked: {node.name} (rank {node.rank_text})")
        if node.on_unlock:
            node.on_unlock(node)
        for cb in self._on_unlock:
            cb(node)
        return node.cost

    def reset(self) -> int:
        """Reset all skills and return refunded points."""
        refund = 0
        for node in self._nodes.values():
            refund += node.current_rank * node.cost
            node.current_rank = 0
        return refund

    def on_unlock(self, callback: Callable[[SkillNode], None]) -> None:
        self._on_unlock.append(callback)

    @property
    def all_nodes(self) -> list[SkillNode]:
        return list(self._nodes.values())

    @property
    def unlocked_nodes(self) -> list[SkillNode]:
        return [n for n in self._nodes.values() if n.unlocked]

    def get_connections(self) -> list[tuple[str, str]]:
        """Get prerequisite connections for rendering lines."""
        connections = []
        for node in self._nodes.values():
            for prereq in node.prerequisites:
                connections.append((prereq, node.skill_id))
        return connections

    @property
    def tiers(self) -> dict[int, list[SkillNode]]:
        """Group nodes by tier for layout."""
        result: dict[int, list[SkillNode]] = {}
        for node in self._nodes.values():
            if node.tier not in result:
                result[node.tier] = []
            result[node.tier].append(node)
        return dict(sorted(result.items()))

    def to_dict(self) -> dict[str, int]:
        return {sid: n.current_rank for sid, n in self._nodes.items()}

    def from_dict(self, data: dict[str, int]) -> None:
        for sid, rank in data.items():
            if sid in self._nodes:
                self._nodes[sid].current_rank = rank
