"""Minimap and Radar System.

Usage:
    minimap = Minimap(size=200, world_range=100)
    minimap.set_player_pos((50, 0, 30), rotation_y=1.2)
    minimap.add_icon(MinimapIcon("enemy_1", (60, 0, 35), color=(1,0,0), icon_type="enemy"))
    minimap.add_icon(MinimapIcon("quest_npc", (45, 0, 20), color=(1,1,0), icon_type="quest"))
    points = minimap.get_render_data()  # Returns screen-space coords for rendering
"""
from __future__ import annotations
from dataclasses import dataclass, field
import math


@dataclass
class MinimapIcon:
    """An icon on the minimap."""
    icon_id: str
    world_pos: tuple[float, float, float] = (0, 0, 0)
    color: tuple[float, float, float] = (1, 1, 1)
    icon_type: str = "default"  # enemy, quest, poi, ally, default
    visible: bool = True
    scale: float = 1.0


@dataclass
class MinimapPoint:
    """Computed screen-space position for rendering."""
    icon_id: str
    screen_x: float
    screen_y: float
    color: tuple[float, float, float]
    icon_type: str
    scale: float
    in_range: bool


class Minimap:
    """2D radar minimap.

    Example:
        mm = Minimap(size=180, world_range=80)
        mm.set_player_pos(player_pos, player_yaw)
        mm.add_icon(MinimapIcon("npc", npc_pos, (0,1,0), "ally"))
        for pt in mm.get_render_data():
            draw_dot(pt.screen_x, pt.screen_y, pt.color)
    """

    def __init__(self, size: int = 200, world_range: float = 100.0,
                 rotate_with_player: bool = True) -> None:
        self.size = size
        self.world_range = world_range
        self.rotate_with_player = rotate_with_player
        self._icons: dict[str, MinimapIcon] = {}
        self._player_x: float = 0
        self._player_z: float = 0
        self._player_rot: float = 0

    def set_player_pos(self, pos: tuple[float, float, float],
                       rotation_y: float = 0) -> None:
        self._player_x = pos[0]
        self._player_z = pos[2]
        self._player_rot = rotation_y

    def add_icon(self, icon: MinimapIcon) -> None:
        self._icons[icon.icon_id] = icon

    def remove_icon(self, icon_id: str) -> None:
        self._icons.pop(icon_id, None)

    def update_icon_pos(self, icon_id: str,
                        world_pos: tuple[float, float, float]) -> None:
        if icon_id in self._icons:
            self._icons[icon_id].world_pos = world_pos

    def get_render_data(self) -> list[MinimapPoint]:
        """Get all icons as screen-space coordinates relative to minimap center."""
        points = []
        half = self.size / 2
        scale = half / self.world_range

        for icon in self._icons.values():
            if not icon.visible:
                continue

            # Offset from player
            dx = icon.world_pos[0] - self._player_x
            dz = icon.world_pos[2] - self._player_z

            # Rotate to player's facing direction
            if self.rotate_with_player:
                cos_r = math.cos(-self._player_rot)
                sin_r = math.sin(-self._player_rot)
                rx = dx * cos_r - dz * sin_r
                rz = dx * sin_r + dz * cos_r
            else:
                rx, rz = dx, dz

            screen_x = half + rx * scale
            screen_y = half - rz * scale  # Invert Z for screen coords

            dist = math.sqrt(dx * dx + dz * dz)
            in_range = dist <= self.world_range

            # Clamp to minimap circle
            dist_screen = math.sqrt((screen_x - half) ** 2 + (screen_y - half) ** 2)
            if dist_screen > half:
                angle = math.atan2(screen_y - half, screen_x - half)
                screen_x = half + math.cos(angle) * (half - 3)
                screen_y = half + math.sin(angle) * (half - 3)

            points.append(MinimapPoint(
                icon_id=icon.icon_id,
                screen_x=screen_x,
                screen_y=screen_y,
                color=icon.color,
                icon_type=icon.icon_type,
                scale=icon.scale,
                in_range=in_range,
            ))

        return points

    def clear(self) -> None:
        self._icons.clear()
