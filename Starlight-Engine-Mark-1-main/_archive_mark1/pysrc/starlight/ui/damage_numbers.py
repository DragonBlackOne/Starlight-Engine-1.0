"""Floating Damage Numbers System.

Usage:
    dmg = DamageNumbers()
    dmg.spawn(position=(5, 2, 0), value=42, color=(1, 0, 0))
    dmg.spawn(position=(5, 2, 0), value=100, color=(1, 1, 0), crit=True)
    # In game loop:
    dmg.update(dt)
    for popup in dmg.active:
        render_text(popup.text, popup.screen_x, popup.screen_y, popup.alpha)
"""
from __future__ import annotations
from dataclasses import dataclass, field
import random
import math


@dataclass
class DamagePopup:
    """A single floating damage number."""
    value: int
    x: float
    y: float
    z: float
    color: tuple[float, float, float] = (1.0, 1.0, 1.0)
    crit: bool = False
    lifetime: float = 1.2
    elapsed: float = 0.0
    velocity_y: float = 3.0
    velocity_x: float = 0.0
    scale: float = 1.0

    @property
    def alive(self) -> bool:
        return self.elapsed < self.lifetime

    @property
    def alpha(self) -> float:
        t = self.elapsed / self.lifetime
        return max(0.0, 1.0 - t * t)

    @property
    def text(self) -> str:
        prefix = "CRIT " if self.crit else ""
        return f"{prefix}{self.value}"

    @property
    def current_scale(self) -> float:
        if self.crit:
            t = self.elapsed / self.lifetime
            return self.scale * (1.3 - 0.3 * t)
        return self.scale


class DamageNumbers:
    """Manages floating damage popups.

    Example:
        dn = DamageNumbers()
        dn.spawn((10, 2, 5), 50, color=(1, 0.2, 0.2))
        dn.update(dt)
    """

    def __init__(self, max_popups: int = 50) -> None:
        self._popups: list[DamagePopup] = []
        self.max_popups = max_popups

    def spawn(self, position: tuple[float, float, float], value: int,
              color: tuple[float, float, float] = (1, 1, 1),
              crit: bool = False) -> None:
        if len(self._popups) >= self.max_popups:
            self._popups.pop(0)

        popup = DamagePopup(
            value=value,
            x=position[0] + random.uniform(-0.3, 0.3),
            y=position[1],
            z=position[2] + random.uniform(-0.3, 0.3),
            color=color,
            crit=crit,
            velocity_y=random.uniform(2.5, 4.0),
            velocity_x=random.uniform(-1.0, 1.0),
            scale=1.5 if crit else 1.0,
        )
        self._popups.append(popup)

    def update(self, dt: float) -> None:
        for p in self._popups:
            p.elapsed += dt
            p.y += p.velocity_y * dt
            p.x += p.velocity_x * dt
            p.velocity_y -= 2.0 * dt  # Gravity-like deceleration

        self._popups = [p for p in self._popups if p.alive]

    @property
    def active(self) -> list[DamagePopup]:
        return self._popups

    def clear(self) -> None:
        self._popups.clear()
