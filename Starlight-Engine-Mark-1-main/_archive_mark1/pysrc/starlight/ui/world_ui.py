"""World Space UI (3D Widgets).

Usage:
    wui = WorldUIPanel()
    wui.set_target(entity_pos)
    wui.update(camera)
    if wui.is_visible: render_2d_at(wui.screen_pos)
"""
from __future__ import annotations
import math

class WorldUIPanel:
    """A UI panel attached to a 3D location."""
    def __init__(self) -> None:
        self.world_pos = (0.0, 0.0, 0.0)
        self.offset = (0.0, 2.0, 0.0) # Floating above head
        self.screen_pos = (0.0, 0.0)
        self.visible = False
        self.scale = 1.0

    def update(self, camera_pos: tuple[float,float,float], 
               view_matrix, projection_matrix, screen_size: tuple[int,int]) -> None:
        # Project world_pos + offset to screen
        # Simplified logic:
        
        # 1. Dist check
        dx = self.world_pos[0] - camera_pos[0]
        dy = self.world_pos[1] - camera_pos[1]
        dz = self.world_pos[2] - camera_pos[2]
        dist = math.sqrt(dx*dx + dy*dy + dz*dz)
        
        if dist > 50.0 or dist < 0.1:
            self.visible = False
            return
            
        self.visible = True
        self.scale = 1.0 / dist * 5.0 # Scale by distance
        
        # 2. Projection (Placeholder)
        # In real engine, mul matrix vector
        # self.screen_pos = project(...)
        self.screen_pos = (screen_size[0]/2, screen_size[1]/2) # Center for now
