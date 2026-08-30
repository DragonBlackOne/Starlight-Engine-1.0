"""Flexbox-like Layout Engine.

Usage:
    root = FlexNode(w=100, h=100, direction="row")
    child1 = FlexNode(w=50, h=50)
    root.add_child(child1)
    root.layout()
"""
from __future__ import annotations
from dataclasses import dataclass

@dataclass
class Rect:
    x: float = 0
    y: float = 0
    w: float = 0
    h: float = 0

class FlexNode:
    """A layout node."""
    def __init__(self, w: float = 0, h: float = 0, 
                 direction: str = "column", # row, column
                 justify: str = "start", # start, center, end, space-between
                 align: str = "start", # start, center, end, stretch
                 padding: float = 0.0) -> None:
        self.computed = Rect(0,0,w,h)
        self.requested_w = w
        self.requested_h = h
        self.direction = direction
        self.justify = justify
        self.align = align
        self.padding = padding
        self.children: list[FlexNode] = []
        
    def add(self, child: FlexNode) -> None:
        self.children.append(child)

    def layout(self, x: float, y: float, w: float, h: float) -> None:
        # Very basic implementation
        self.computed.x = x
        self.computed.y = y
        self.computed.w = w if w > 0 else self.requested_w
        self.computed.h = h if h > 0 else self.requested_h
        
        # Determine content area
        cx = x + self.padding
        cy = y + self.padding
        cw = self.computed.w - self.padding * 2
        ch = self.computed.h - self.padding * 2
        
        cursor_x, cursor_y = cx, cy
        
        for child in self.children:
            child_w = child.requested_w
            child_h = child.requested_h
            
            # Simple stack
            child.layout(cursor_x, cursor_y, child_w, child_h)
            
            if self.direction == "column":
                cursor_y += child.computed.h
            else:
                cursor_x += child.computed.w
