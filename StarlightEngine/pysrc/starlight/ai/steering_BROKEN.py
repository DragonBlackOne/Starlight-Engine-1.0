"""Steering Behaviors.

Usage:
    steering = SteeringBehavior()
    force = steering.seek(agent_pos, target_pos)
"""
from __future__ import annotations
from dataclasses import dataclass

@dataclass
class SteeringOutput:
    linear: tuple[float, float, float] = (0.0, 0.0, 0.0)
    angular: float = 0.0

class SteeringBehavior:
    def seek(self, position: tuple[float,float,float], target: tuple[float,float,float], max_speed: float) -> tuple[float,float,float]:
        dx = target[0] - position[0]
        dy = target[1] - position[1]
        dz = target[2] - position[2]
        dist = (dx*dx + dy*dy + dz*dz)**0.5
        if dist > 0:
            return (dx/dist * max_speed, dy/dist * max_speed, dz/dist * max_speed)
        return (0.0, 0.0, 0.0)

    def flee(self, position: tuple[float,float,float], target: tuple[float,float,float], max_speed: float) -> tuple[float,float,float]:
        seek = self.seek(position, target, max_speed)
        return (-seek[0], -seek[1], -seek[2])
