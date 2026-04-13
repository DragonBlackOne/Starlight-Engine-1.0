from typing import Optional
from .base import Skill
from ..entity import Entity
from ..keys import Keys
from ..input import Input
from .. import backend
from ..math import get_forward_vector

class SkillManager:
    """
    Manages a collection of skills for an entity.

    Attributes:
        owner (Entity): The entity that possesses these skills.
        input (Input): Reference to the application input system.
        skills (list[Skill]): List of learned skills.
        key_bindings (dict[str, int]): Mapping of Key Names to Skill Indices.
    """
    def __init__(self, owner: Entity, app_input: Input):
        self.owner = owner
        self.input = app_input
        self.skills: list[Skill] = []
        self.key_bindings: dict[str, int] = {} # Key Name -> Skill Index

    def add_skill(self, skill: Skill) -> int:
        """
        Adds a skill to the manager.

        Args:
            skill (Skill): The skill instance to add.

        Returns:
            int: The index of the added skill.
        """
        self.skills.append(skill)
        return len(self.skills) - 1

    def bind_key(self, key: str, skill_index: int) -> None:
        """
        Binds a keyboard key to a specific skill index.

        Args:
            key (str): The key name (e.g., 'Q', 'Space').
            skill_index (int): The index of the skill in self.skills.
        """
        self.key_bindings[key] = skill_index

    def update(self, dt: float) -> None:
        """
        Updates all skills (cooldowns) and checks for input triggers.

        Args:
            dt (float): Delta time in seconds.
        """
        # Update cooldowns
        for skill in self.skills:
            skill.update(dt)

        # check inputs
        for key, index in self.key_bindings.items():
            if self.input.is_key_just_pressed(getattr(Keys, key, key)):
                if 0 <= index < len(self.skills):
                    # Raycast for target
                    target = None
                    target_entity = None

                    if self.owner.transform:
                        try:
                            # Get rotation from backend directly since Entity doesn't expose getter
                            rot = backend.get_rotation(self.owner.id)
                            fwd = get_forward_vector(rot.x, rot.y, rot.z)

                            # Raycast from slightly above center (approx eye level)
                            ox, oy, oz = self.owner.transform.x, self.owner.transform.y + 0.5, self.owner.transform.z
                            dx, dy, dz = fwd[0], fwd[1], fwd[2]

                            # Max distance 100.0, solid check, ignore self
                            hit = backend.raycast(ox, oy, oz, dx, dy, dz, 100.0, True, self.owner.id)

                            if hit:
                                hit_id, _, hx, hy, hz = hit
                                target = [hx, hy, hz]
                                target_entity = Entity(hit_id)
                            else:
                                # Cast at max distance point
                                target = [ox + dx * 100.0, oy + dy * 100.0, oz + dz * 100.0]
                        except Exception:
                            # Fallback if something fails (e.g. rotation missing)
                            pass

                    self.skills[index].try_cast(self.owner, target, target_entity=target_entity)

    def get_skill(self, name: str) -> Optional[Skill]:
        """
        Retrieves a skill by name.

        Args:
            name (str): The name of the skill.

        Returns:
            Optional[Skill]: The skill instance or None if not found.
        """
        for s in self.skills:
            if s.name == name:
                return s
        return None

