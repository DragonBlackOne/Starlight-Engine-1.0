"""Sensory Perception System (Vision & Hearing).

Usage:
    sense = PerceptionSystem(owner_entity)
    vision = Sense(name="Vision", range=20.0, angle=90.0)
    sense.add_sense(vision)
    
    # In loop:
    sense.register_stimulus(Stimulus("noise", pos=(10,0,0), strength=1.0))
    sense.update(dt)
    if sense.has_perceived("Player"):
        print("I see you!")
"""
from __future__ import annotations
from dataclasses import dataclass, field
import math


@dataclass
class Stimulus:
    """Anything perceivable (sound, visible object)."""
    tag: str # "Player", "Explosion", "Footstep"
    position: tuple[float, float, float]
    strength: float = 1.0 # 0..1
    velocity: tuple[float, float, float] = (0, 0, 0)
    lifetime: float = 0.5 # stimulus duration


@dataclass
class VisibleTarget:
    """Helper for cached visibility state."""
    entity_id: int
    position: tuple[float, float, float]
    last_seen_time: float


class Sense:
    """Config for a specific sense (Vision, Hearing)."""
    def __init__(self, name: str, range: float, angle_degrees: float = 360.0,
                 acuity: float = 1.0) -> None:
        self.name = name
        self.range = range
        self.angle_cos = math.cos(math.radians(angle_degrees / 2.0))
        self.acuity = acuity # Multiplier for detection speed


class PerceptionSystem:
    """Manages senses and memory of perceived targets.

    Example:
        ps = PerceptionSystem(owner_id)
        ps.add_sense(Sense("Vision", 20, 90))
        ps.process_stimuli(all_stimuli)
    """

    def __init__(self, owner_position_func: Callable[[], tuple[float,float,float]], 
                 owner_forward_func: Callable[[], tuple[float,float,float]]) -> None:
        self.get_pos = owner_position_func
        self.get_fwd = owner_forward_func
        self.senses: list[Sense] = []
        self.memory: dict[str, Any] = {} # tag -> data
        self.current_stimuli: list[Stimulus] = []

    def add_sense(self, sense: Sense) -> None:
        self.senses.append(sense)

    def register_stimulus(self, stim: Stimulus) -> None:
        self.current_stimuli.append(stim)

    def update(self, dt: float) -> None:
        owner_pos = self.get_pos()
        owner_fwd = self.get_fwd()

        seen_tags = set()

        for stim in self.current_stimuli:
            dx = stim.position[0] - owner_pos[0]
            dy = stim.position[1] - owner_pos[1]
            dz = stim.position[2] - owner_pos[2]
            dist_sq = dx*dx + dy*dy + dz*dz
            
            for sense in self.senses:
                # Range Check
                if dist_sq > sense.range * sense.range:
                    continue
                
                # Angle Check
                if sense.angle_cos > -0.99: # If not 360
                    dist = math.sqrt(dist_sq)
                    if dist > 0.001:
                        # Dot product for angle
                        dot = (dx/dist)*owner_fwd[0] + (dy/dist)*owner_fwd[1] + (dz/dist)*owner_fwd[2]
                        if dot < sense.angle_cos:
                            continue

                # Detection successful
                self.memory[stim.tag] = {
                    "pos": stim.position,
                    "time": 0.0 # Time since seen
                }
                seen_tags.add(stim.tag)

        # Decay memory
        for tag in list(self.memory.keys()):
            self.memory[tag]["time"] += dt
            if self.memory[tag]["time"] > 10.0: # Forget after 10s
                del self.memory[tag]
                
        # Clear stimuli for next frame
        self.current_stimuli.clear()

    def has_perceived(self, tag: str) -> bool:
        return tag in self.memory

    def get_last_known_pos(self, tag: str) -> tuple[float, float, float] | None:
        if tag in self.memory:
            return self.memory[tag]["pos"]
        return None
