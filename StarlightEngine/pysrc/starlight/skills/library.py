from .base import Skill
from ..entity import Entity
from .. import backend
from loguru import logger

class GravityFlipSkill(Skill):
    """
    Skill that inverts the global gravity on the Y axis.
    """
    def __init__(self):
        super().__init__("Gravity Flip", cooldown=2.0)
        self.inverted = False

    def cast(self, caster: Entity, target: list[float] | None = None, **kwargs) -> bool:
        """
        Toggles gravity between 9.81 and -9.81.
        """
        self.inverted = not self.inverted
        y_g = 9.81 if self.inverted else -9.81
        
        logger.info(f"CAST: Gravity set to Y={y_g}")
        if hasattr(backend, 'set_gravity'):
            backend.set_gravity(0.0, y_g, 0.0)
            return True
        return False

class ZeroGravitySkill(Skill):
    """
    Skill that disables gravity entirely.
    """
    def __init__(self):
        super().__init__("Zero Gravity", cooldown=5.0)

    def cast(self, caster: Entity, target: list[float] | None = None, **kwargs) -> bool:
        """
        Sets gravity to (0, 0, 0).
        """
        logger.info("CAST: Zero Gravity!")
        if hasattr(backend, 'set_gravity'):
            backend.set_gravity(0.0, 0.0, 0.0)
            return True
        return False

class HeavyGravitySkill(Skill):
    """
    Skill that increases gravity to extreme levels.
    """
    def __init__(self):
        super().__init__("Heavy World", cooldown=5.0)

    def cast(self, caster: Entity, target: list[float] | None = None, **kwargs) -> bool:
        """
        Sets gravity to -50.0 on Y.
        """
        logger.info("CAST: Heavy Gravity (-50.0)!")
        if hasattr(backend, 'set_gravity'):
            backend.set_gravity(0.0, -50.0, 0.0)
            return True
        return False
