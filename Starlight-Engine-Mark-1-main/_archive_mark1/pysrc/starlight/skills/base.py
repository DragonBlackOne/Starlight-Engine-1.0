from abc import ABC, abstractmethod
from typing import Optional, Any
from ..entity import Entity
from .. import math as pymath

class Skill(ABC):
    """
    Abstract Base Class for all In-Game Skills.
    """
    def __init__(self, name: str, cooldown: float, mana_cost: float = 0.0):
        self.name = name
        self.cooldown_max = cooldown
        self.cooldown_timer = 0.0
        self.mana_cost = mana_cost
        self.icon = None # path string or texture id

    def is_ready(self) -> bool:
        return self.cooldown_timer <= 0.0

    def update(self, dt: float):
        if self.cooldown_timer > 0.0:
            self.cooldown_timer -= dt

    def try_cast(self, caster: Entity, target: Optional[list[float]] = None, **kwargs) -> bool:
        if not self.is_ready():
            # print(f"{self.name} is on cooldown: {self.cooldown_timer:.1f}s")
            return False
        
        if self.cast(caster, target, **kwargs):
            self.cooldown_timer = self.cooldown_max
            return True
        return False

    @abstractmethod
    def cast(self, caster: Entity, target: Optional[list[float]] = None, **kwargs) -> bool:
        """
        Implementation of the skill logic. Return True if cast was successful.
        """
        pass
