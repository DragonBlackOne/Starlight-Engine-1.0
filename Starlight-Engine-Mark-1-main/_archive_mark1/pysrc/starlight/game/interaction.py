"""Interaction System — 'Press F to Interact'.

Usage:
    system = InteractionSystem(backend)
    door = Interactable("door_01", position=(5, 1, 0), radius=2.0,
                        prompt="Open Door", on_interact=lambda: open_door())
    system.register(door)
    # In game loop:
    system.update(player_pos=(4, 1, 0))
    if player_pressed_interact:
        system.interact()  # Triggers nearest interactable
"""
from __future__ import annotations
from dataclasses import dataclass, field
from typing import Callable, Any
import math


@dataclass
class Interactable:
    """An interactable world object."""
    obj_id: str
    position: tuple[float, float, float] = (0, 0, 0)
    radius: float = 2.0
    prompt: str = "Interact"
    on_interact: Callable[[], None] | None = None
    enabled: bool = True
    one_shot: bool = False
    _used: bool = False

    @property
    def available(self) -> bool:
        return self.enabled and not (self.one_shot and self._used)


class InteractionSystem:
    """Manages proximity-based interactions.

    Example:
        system = InteractionSystem()
        system.register(Interactable("chest", position=(10, 0, 5), radius=2.0,
                                     prompt="Open Chest", on_interact=open_chest))
        system.update(player_pos)
        if system.can_interact:
            print(f"Press F to {system.current_prompt}")
    """

    def __init__(self) -> None:
        self._interactables: dict[str, Interactable] = {}
        self._nearest: Interactable | None = None
        self._nearest_dist: float = float('inf')

    def register(self, obj: Interactable) -> None:
        self._interactables[obj.obj_id] = obj

    def unregister(self, obj_id: str) -> None:
        self._interactables.pop(obj_id, None)
        if self._nearest and self._nearest.obj_id == obj_id:
            self._nearest = None

    def update(self, player_pos: tuple[float, float, float]) -> None:
        """Find the nearest interactable within range."""
        self._nearest = None
        self._nearest_dist = float('inf')

        for obj in self._interactables.values():
            if not obj.available:
                continue
            dx = player_pos[0] - obj.position[0]
            dy = player_pos[1] - obj.position[1]
            dz = player_pos[2] - obj.position[2]
            dist = math.sqrt(dx * dx + dy * dy + dz * dz)

            if dist <= obj.radius and dist < self._nearest_dist:
                self._nearest = obj
                self._nearest_dist = dist

    def interact(self) -> bool:
        """Trigger the nearest interactable. Returns True if successful."""
        if not self._nearest or not self._nearest.available:
            return False
        if self._nearest.on_interact:
            self._nearest.on_interact()
        if self._nearest.one_shot:
            self._nearest._used = True
        return True

    @property
    def can_interact(self) -> bool:
        return self._nearest is not None and self._nearest.available

    @property
    def current_prompt(self) -> str:
        return self._nearest.prompt if self._nearest else ""

    @property
    def current_target(self) -> Interactable | None:
        return self._nearest
