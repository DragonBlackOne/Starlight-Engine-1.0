"""Editor Gizmos System.

Usage:
    gizmos.draw_line(start, end, color)
    gizmos.draw_wire_cube(center, size)
"""
from __future__ import annotations
from dataclasses import dataclass
import traceback

class Gizmos:
    """Immediate mode gizmo drawing context."""
    def __init__(self, studio) -> None:
        self.studio = studio
        self.enabled = True

    def draw_line(self, start: tuple[float,float,float] | list[float], 
                  end: tuple[float,float,float] | list[float], 
                  color: tuple[float,float,float] | list[float] = (1.0, 1.0, 0.0)) -> None:
        if not self.enabled or not self.studio.backend: return
        try:
            self.studio.backend.add_gizmo_line(*start, *end, *color)
        except Exception as e:
            pass # ignore if not implemented yet

    def draw_wire_cube(self, center: tuple[float,float,float] | list[float], 
                       size: tuple[float,float,float] | list[float], 
                       color: tuple[float,float,float] | list[float] = (1.0, 1.0, 1.0)) -> None:
        if not self.enabled or not self.studio.backend: return
        cx, cy, cz = center
        hx, hy, hz = size[0]*0.5, size[1]*0.5, size[2]*0.5
        
        # Bottom
        self.draw_line((cx-hx, cy-hy, cz-hz), (cx+hx, cy-hy, cz-hz), color)
        self.draw_line((cx+hx, cy-hy, cz-hz), (cx+hx, cy-hy, cz+hz), color)
        self.draw_line((cx+hx, cy-hy, cz+hz), (cx-hx, cy-hy, cz+hz), color)
        self.draw_line((cx-hx, cy-hy, cz+hz), (cx-hx, cy-hy, cz-hz), color)
        # Top
        self.draw_line((cx-hx, cy+hy, cz-hz), (cx+hx, cy+hy, cz-hz), color)
        self.draw_line((cx+hx, cy+hy, cz-hz), (cx+hx, cy+hy, cz+hz), color)
        self.draw_line((cx+hx, cy+hy, cz+hz), (cx-hx, cy+hy, cz+hz), color)
        self.draw_line((cx-hx, cy+hy, cz+hz), (cx-hx, cy+hy, cz-hz), color)
        # Columns
        self.draw_line((cx-hx, cy-hy, cz-hz), (cx-hx, cy+hy, cz-hz), color)
        self.draw_line((cx+hx, cy-hy, cz-hz), (cx+hx, cy+hy, cz-hz), color)
        self.draw_line((cx+hx, cy-hy, cz+hz), (cx+hx, cy+hy, cz+hz), color)
        self.draw_line((cx-hx, cy-hy, cz+hz), (cx-hx, cy+hy, cz+hz), color)

